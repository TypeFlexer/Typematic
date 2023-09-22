//=--ConstraintBuilder.cpp----------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// Implementation of visitor methods for the FunctionVisitor class. These
// visitors create constraints based on the AST of the program.
//===----------------------------------------------------------------------===//

#include "clang/CheckMate/ConstraintBuilder.h"
#include "clang/CheckMate/CheckMateGlobalOptions.h"
#include "clang/CheckMate/CheckMateStats.h"
#include "clang/CheckMate/ConstraintResolver.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include <algorithm>
#include <iostream>

using namespace llvm;
using namespace clang;

class MyASTVisitor : public clang::RecursiveASTVisitor<MyASTVisitor> {
public:
  explicit MyASTVisitor(ASTContext *C, ProgramInfo &I)
      : Context(C), Info(I), CB(Info, Context) {}

  std::string fetchMacroExpanseForDecl(SourceRange SR,
                                       SourceManager *SM)
  {
       auto FullSourceLocation = FullSourceLoc(SR.getBegin(),
                                            *SM);
       /*
        * Now in a loop walk backwards the source until you find the pre-
        * processor token (#)
        */

       signed int Counter = 0;
       auto MacroLoc = FullSourceLocation.getSpellingLoc();
       char CurrToken = (char)MacroLoc.getCharacterData()[0];
       auto IntermediateMacroLoc = MacroLoc;
       while(CurrToken != '#')
       {
         Counter = Counter - 1;
         auto TempLoc = MacroLoc.getSpellingLoc()
                                 .getLocWithOffset(Counter);
         IntermediateMacroLoc = FullSourceLoc(TempLoc, *SM);
         CurrToken = (char)IntermediateMacroLoc.getCharacterData()[0];
       }

       const auto *Spelling = FullSourceLoc(IntermediateMacroLoc, *SM).getSpellingLoc()
                           .getCharacterData();
       /*
        * Now that you point to the right location fetch the sourceLoc string
        */
       /*
        * Support for multiline macros is a little different
        * if a escape character is preceeded by a backslash, it means the macro is continued
        * through the next line
        *
        * Hence You basically look for the location that has no preceeding non-space character as
        * backslash
        *
        */
       bool MultiLineMacroNotFound = true;
       int pos = 0;
       int MacroTerminatingPos = std::string(Spelling).find("\n");

       do{
         pos = std::string(Spelling).find("\n", pos);
         int Counter = 1;
         auto PreceedingNonSpaceChar =
             std::string(Spelling).find("\n", pos)-Counter;
         while(isSpace(reinterpret_cast<char>(
                    std::string(Spelling).at(PreceedingNonSpaceChar))))
         {
           Counter = Counter - 1;
           PreceedingNonSpaceChar = std::string(Spelling).find("\n", pos)-Counter;
         }
         if (reinterpret_cast<char>(
                 std::string(Spelling).at(PreceedingNonSpaceChar)) !=
             '\\')
        {
           MacroTerminatingPos = std::string(Spelling).find("\n", pos);
           MultiLineMacroNotFound = false;
        }
        else
         {
          pos =  std::string(Spelling).find("\n", pos+1);
         }
       }while(MultiLineMacroNotFound);

       return std::string(Spelling).substr(0,
                                           MacroTerminatingPos);
  }

  bool VisitStmt(Stmt *S)
  {
    if(S->getBeginLoc().isMacroID() && S->getTaintedScopeSpecifier() != clang::Tainted_None)
    {
      std::string IncludeDirective = fetchMacroExpanseForDecl(
          S->getSourceRange(),
          &(Context->getSourceManager()));
      CB.storeIncludeStatement(
          Context->getFullLoc(S->getBeginLoc()).getSpellingLoc(), Context->getSourceManager(),
          IncludeDirective);
    }
    return true;
  }

  bool VisitExpr(Expr *S)
  {
    if(S->getBeginLoc().isMacroID() && S->getTaintedScopeSpecifier() != clang::Tainted_None)
    {
      std::string IncludeDirective = fetchMacroExpanseForDecl(
          S->getSourceRange(),
          &(Context->getSourceManager()));
      CB.storeIncludeStatement(
          Context->getFullLoc(S->getBeginLoc()).getSpellingLoc(), Context->getSourceManager(),
          IncludeDirective);
    }
    return true;
  }

  bool VisitCallExpr(clang::CallExpr *callexpr)
  {
    auto RefDec = callexpr->getCallee()->getReferencedDeclOfCallee();
    if( RefDec != NULL && RefDec->getAsFunction() != NULL &&
        callexpr->getBeginLoc().isMacroID() && (callexpr->getTaintedScopeSpecifier() != clang::Tainted_None))
    {
      std::cout<<"Visiting a MACRO call expression: " <<
          RefDec->getAsFunction()->getName().str()<<std::endl;
      std::string IncludeDirective = fetchMacroExpanseForDecl(

          callexpr->getSourceRange(),
          &(Context->getSourceManager()));
      CB.storeIncludeStatement(
          Context->getFullLoc(callexpr->getBeginLoc()).getSpellingLoc(), Context->getSourceManager(),
          IncludeDirective);
    }

    return true;
  }

  bool VisitUnaryOperator(clang::UnaryOperator* unary)
  {
    auto decl = unary->getReferencedDeclOfCallee();
    if(decl != NULL)
    {
      auto Vardecl = dyn_cast<VarDecl>(decl);
      if(Vardecl != NULL && Vardecl->getLocation().isMacroID()
          && (unary->getTaintedScopeSpecifier() != clang::Tainted_None))
      {
        /*
         * fetch the macro expanse for this Macro
         */
        std::string IncludeDirective = fetchMacroExpanseForDecl(
            Vardecl->getSourceRange(),
            &(Context->getSourceManager()));
        std::cout<<"Include directive: "<< IncludeDirective<<std::endl;
        CB.storeIncludeStatement(
            Context->getFullLoc(unary->getExprLoc()).getSpellingLoc(), Context->getSourceManager(),
            IncludeDirective);
      }
    }
    return true;
  }

  bool VisitBinaryOperator(clang::BinaryOperator *bo)
  {
    auto LHSExpr = bo->getLHS();
    auto RHSExpr = bo->getRHS();

    if((RHSExpr == NULL) || (LHSExpr == NULL))
      return true;

    auto RhsD = RHSExpr->getReferencedDeclOfCallee();
    auto LhsD = LHSExpr->getReferencedDeclOfCallee();
    if (RhsD != NULL) {
      auto *Vardec = dyn_cast<VarDecl>(RhsD);
      if(Vardec != NULL && RhsD->getLocation().isMacroID() &&
          bo->getTaintedScopeSpecifier() != clang::Tainted_None)
      {
        std::cout<<"Visiting a Binary expression where your RHS is "
                  << Vardec->getNameAsString()<<std::endl;
        std::string IncludeDirective = fetchMacroExpanseForDecl(
            Vardec->getSourceRange(),
            &(Context->getSourceManager()));
        CB.storeIncludeStatement(
            Context->getFullLoc(RHSExpr->getExprLoc()).getSpellingLoc(), Context->getSourceManager(),
            IncludeDirective);
      }
    }

    if (LhsD != NULL) {
      auto *Vardec = dyn_cast<VarDecl>(LhsD);
      if(Vardec != NULL && LhsD->getLocation().isMacroID() &&
          LHSExpr->getTaintedScopeSpecifier() != Tainted_None)
      {
        std::cout<<"Visiting a Binary expression where your LHS is "
                  << Vardec->getNameAsString()<<std::endl;
        std::string IncludeDirective = fetchMacroExpanseForDecl(
            Vardec->getSourceRange(),
            &(Context->getSourceManager()));
        CB.storeIncludeStatement(
            Context->getFullLoc(LHSExpr->getExprLoc()).getSpellingLoc(), Context->getSourceManager(),
            IncludeDirective);
      }
    }
    return true;
  }

private:
  ASTContext *Context;
  ProgramInfo &Info;
  FunctionDecl *Function;
  ConstraintResolver CB;
};
// This class visits functions and adds constraints to the
// Constraints instance assigned to it.
// Each VisitXXX method is responsible for looking inside statements
// and imposing constraints on variables it uses
class FunctionVisitor : public RecursiveASTVisitor<FunctionVisitor> {
public:
  explicit FunctionVisitor(ASTContext *C, ProgramInfo &I, FunctionDecl *FD)
      : Context(C), Info(I), Function(FD), CB(Info, Context) {}

  // T x = e
  /*
   * This basically visits every decl involved in each and every statement
   */
  bool VisitDeclStmt(DeclStmt *S) {
    // Process inits even for non-pointers because structs and union values
    // can contain pointers
    for (const auto &D : S->decls()) {
      if(FunctionDecl *FD = dyn_cast<FunctionDecl>(D)){
        std::cout<< FD->getName().str() << std::endl;
        FD->getBody()->dump();
        FD->getReturnType()->dump();
        std::cout << "Is Tainted: " << getDeclaration(FD)->isTainted();
        for(int i = 0 ; i < FD->getNumParams(); i++)
        {
          FD->getParamDecl(i)->dump();
        }
      }
    }
    return true;
  }

  // (T)e
  bool VisitCStyleCastExpr(CStyleCastExpr *C) {
    return true;
  }

  // x += e
  bool VisitCompoundAssignOperator(CompoundAssignOperator *O) {
    switch (O->getOpcode()) {
    case BO_AddAssign:
    case BO_SubAssign:
      arithBinop(O, true);
      break;
    // rest shouldn't happen on pointers, so we ignore
    default:
      break;
    }
    return true;
  }

  // e(e1,e2,...)
  bool VisitCallExpr(CallExpr *E) {
    if (E != NULL && E->getReferencedDeclOfCallee() != NULL
        && E->getReferencedDeclOfCallee()->getAsFunction() != NULL
        && E->getTaintedScopeSpecifier() != clang::Tainted_None)
    {
      auto FD = E->getReferencedDeclOfCallee()->getAsFunction();
      if(FD->getLocation().isMacroID()) {
        auto FullSourceLocation =
            FullSourceLoc(FD->getSourceRange().getBegin(),
                          FD->getASTContext().getSourceManager());
        auto MacroLoc =
            FullSourceLocation.getSpellingLoc().getLocWithOffset(-25);
        FullSourceLocation =
            FullSourceLoc(MacroLoc, FD->getASTContext().getSourceManager());
        auto spellingLoc =
            FullSourceLocation.getSpellingLoc().getCharacterData();
        auto refinedSpellingLoc = std::string(spellingLoc).substr(0,
        std::string(spellingLoc).find("\n"));
        std::cout<<"Macro in use: "<<refinedSpellingLoc<<std::endl;
      }
      else
      {
        std::cout<<FD->getNameAsString()<<" is not a Macro"<<std::endl;
      }
    }
    return true;
  }

  // e1[e2]
  bool VisitArraySubscriptExpr(ArraySubscriptExpr *E) {

    return true;
  }

  // return e;
  bool VisitReturnStmt(ReturnStmt *S) {
    // Get function variable constraint of the body
    return true;
  }

  // ++e, --e, e++, and e--
  bool VisitUnaryOperator(UnaryOperator *O) {
    auto E = O->getExprStmt();
    if (E != NULL && E->getReferencedDeclOfCallee() != NULL
        && O->getTaintedScopeSpecifier() != clang::Tainted_None)
    {
      auto FD = dyn_cast<VarDecl>(E->getReferencedDeclOfCallee());
      if(FD->getLocation().isMacroID()) {
        auto FullSourceLocation =
            FullSourceLoc(FD->getSourceRange().getBegin(),
                          FD->getASTContext().getSourceManager());
        auto MacroLoc =
            FullSourceLocation.getSpellingLoc().getLocWithOffset(-25);
        FullSourceLocation =
            FullSourceLoc(MacroLoc, FD->getASTContext().getSourceManager());
        auto spellingLoc =
            FullSourceLocation.getSpellingLoc().getCharacterData();
        auto refinedSpellingLoc = std::string(spellingLoc).substr(0,
                                                                  std::string(spellingLoc).find("\n"));
        std::cout<<"Macro in use: "<<refinedSpellingLoc<<std::endl;
      }
      else
      {
        std::cout<<FD->getNameAsString()<<" is not a Macro"<<std::endl;
      }
    }
    return true;
  }

  bool VisitBinaryOperator(BinaryOperator *O) {
    if(O->getTaintedScopeSpecifier() == clang::Tainted_None)
      return true;

    auto R = O->getRHS();
    if (R != NULL && R->getReferencedDeclOfCallee() != NULL)
    {
      auto FD = dyn_cast<VarDecl>(
          R->getReferencedDeclOfCallee());
      if(FD->getLocation().isMacroID()) {
        auto FullSourceLocation =
            FullSourceLoc(FD->getSourceRange().getBegin(),
                          FD->getASTContext().getSourceManager());
        auto MacroLoc =
            FullSourceLocation.getSpellingLoc().getLocWithOffset(-25);
        FullSourceLocation =
            FullSourceLoc(MacroLoc, FD->getASTContext().getSourceManager());
        auto spellingLoc =
            FullSourceLocation.getSpellingLoc().getCharacterData();
        auto refinedSpellingLoc = std::string(spellingLoc).substr(0,
                                                                  std::string(spellingLoc).find("\n"));
        std::cout<<"Macro in use: "<<refinedSpellingLoc<<std::endl;
      }
      else
      {
        std::cout<<FD->getNameAsString()<<" is not a Macro"<<std::endl;
      }
    }
    auto L = O->getLHS();
    if (L != NULL && L->getReferencedDeclOfCallee() != NULL)
    {
      auto FD = dyn_cast<VarDecl>(
          L->getReferencedDeclOfCallee());
      if(FD->getLocation().isMacroID()) {
        auto FullSourceLocation =
            FullSourceLoc(FD->getSourceRange().getBegin(),
                          FD->getASTContext().getSourceManager());
        auto MacroLoc =
            FullSourceLocation.getSpellingLoc().getLocWithOffset(-25);
        FullSourceLocation =
            FullSourceLoc(MacroLoc, FD->getASTContext().getSourceManager());
        auto spellingLoc =
            FullSourceLocation.getSpellingLoc().getCharacterData();
        auto refinedSpellingLoc = std::string(spellingLoc).substr(0,
                                                                  std::string(spellingLoc).find("\n"));
        std::cout<<"Macro in use: "<<refinedSpellingLoc<<std::endl;
      }
      else
      {
        std::cout<<FD->getNameAsString()<<" is not a Macro"<<std::endl;
      }
    }
    return true;
  }

private:
  // Here the flag, ModifyingExpr indicates if the arithmetic operation
  // is modifying any variable.
  void arithBinop(BinaryOperator *O, bool ModifyingExpr = false) {
  }

  // Pointer arithmetic constrains the expression to be at least ARR,
  // unless it is on a function pointer. In this case the function pointer
  // is WILD.

  ASTContext *Context;
  ProgramInfo &Info;
  FunctionDecl *Function;
  ConstraintResolver CB;
};

// This class visits a global declaration, generating constraints
// for functions, variables, types, etc. that are visited.
class ConstraintGenVisitor : public RecursiveASTVisitor<ConstraintGenVisitor> {
public:
  explicit ConstraintGenVisitor(ASTContext *Context, ProgramInfo &I)
      : Context(Context), Info(I), CB(Info, Context) {}

  bool VisitVarDecl(VarDecl *G) {

    if(G->hasGlobalStorage() && (G->isTaintedDecl() || G->isMirrorDecl()))
    {
      /*
       * You need to check if this variable has been declared as a typedef or not
       * If this variable is a typedef, then no need to pass it once again
       */
      CB.storeTaintMirroredVarDecl(G);
      if (_CheckMateOpts.Verbose)
        errs() << "Analyzing function " << G->getNameAsString() << "\n";

    }
    return true;
  }

  bool VisitInitListExpr(InitListExpr *E) {
    if (E->getType()->isTaintedStructureType()) {
      /*
       * Here is where you need to Create a map and insert the record decl
       */
      /*
     * If the variable is marked as _Tainted or _Mirror, it must be copy pasted to the Tainted file
     *
     */

      E = E->getSemanticForm();
      const RecordDecl *Definition =
          E->getType()->getAsStructureType()->getDecl()->getDefinition();

      if (Definition->getParentFunctionOrMethod() == nullptr)
        CB.storeTaintMirroredStructVarDecl(
            const_cast<RecordDecl *>(Definition));
      else
        return true;

//      unsigned int InitIdx = 0;
//      const auto Fields = Definition->fields();
//      for (auto It = Fields.begin();
//           InitIdx < E->getNumInits() && It != Fields.end(); InitIdx++, It++) {
//        Expr *InitExpr = E->getInit(InitIdx);
//        CB.constrainLocalAssign(nullptr, *It, InitExpr, Same_to_Same, true);
//      }
    }
    return true;
  }

  bool VisitFunctionDecl(FunctionDecl *D) {
    FullSourceLoc FL = Context->getFullLoc(D->getBeginLoc());


    if((D->isTainted()) || (D->isMirror())){
      if (_CheckMateOpts.Verbose)
        errs() << "Storing Tainted function " << D->getName() << "\n";
      CB.storeTaintedFunctionDecl(D);
    }

    if(D->isCallback() && !D->isThisDeclarationADefinition())
    {
      /*
       * For Callbacks We only want the Prototype declaration
       */
      errs() << "Storing Callback function " << D->getName() << "\n";
      CB.storeCallbackFunctionDecl(D);
    }


    /*

    if (FL.isValid()) { // TODO: When would this ever be false?
      if (D->hasBody() && D->isThisDeclarationADefinition()) {
        Stmt *Body = D->getBody();
        FunctionVisitor FV = FunctionVisitor(Context, Info, D);
        FV.TraverseStmt(Body);
        if (_CheckMateOpts.AllTypes) {
          // Only do this, if all types is enabled.
          LengthVarInference LVI(Info, Context, D);
          LVI.Visit(Body);
        }

      }
    }
    */
    if (_CheckMateOpts.Verbose)
      errs() << "Done analyzing function\n";

    return true;
  }
  bool VisitTypedefDecl(TypedefDecl *TD) {
    // TODO: need to handle here
    if(TD->isTaintedDecl() || TD->isMirrorDecl())
    {
      CB.storeTaintMirroredTypedefDecl(TD);
      errs() << "Tainted/Mirrored Typedef Decl: "<<TD->getNameAsString()<<"\n";
    }
    return true;
  }

  bool VisitEnumDecl(EnumDecl *D)
  {
    if(D->isTaintedDecl() || D->isMirrorDecl() )
    {
        CB.storeTaintMirroredEnumDecl(D);
        errs()<< "Printing Enum"<< D->getName()<<"\n";
    }
  }

  bool VisitRecordDecl(RecordDecl *Declaration) {
    if (Declaration->isThisDeclarationADefinition()) {
      if(Declaration->isTaintedStruct())
      {
        /*
         * Yes this works
         */
        CB.storeTaintMirroredStructVarDecl(Declaration);
        errs()<< "Printing Tstruct"<< Declaration->getName()<<"\n";
      }

//      RecordDecl *Definition = Declaration->getDefinition();
//      assert("Declaration is a definition, but getDefinition() is null?" &&
//             Definition);
//      FullSourceLoc FL = Context->getFullLoc(Definition->getBeginLoc());
//      if (FL.isValid())
//        for (auto *const D : Definition->fields())
//          addVariable(D);
//
    }

    return true;
  }

private:
  ASTContext *Context;
  ProgramInfo &Info;
  ConstraintResolver CB;
};

// Some information about variables in the program is required by the type
// variable analysis and constraint building passes. This is gathered by this
// visitor which is executed before both of the other visitors.
class VariableAdderVisitor : public RecursiveASTVisitor<VariableAdderVisitor> {
public:
  explicit VariableAdderVisitor(ASTContext *Context, ProgramVariableAdder &VA)
      : Context(Context), VarAdder(VA) {}

  // Defining this function lets the visitor traverse implicit function
  // declarations. Without this, we wouldn't see declarations for implicit
  // functions. Instead, we would have to create a FVConstraint when we first
  // encountered a call to the function. This became a problem when the call
  // to ProgramInfo::link() was moved to before the ConstraintBuilder pass.
  bool shouldVisitImplicitCode() const { return true; }

  bool VisitTypedefDecl(TypedefDecl *TD) {
    auto PSL = PersistentSourceLoc::mkPSL(TD, *Context);
    return true;
  }

  //  if ((G->isTaintedDecl() || G->isMirrorDecl()))
  //  {
  //    if ((G->getParentFunctionOrMethod() == nullptr))
  //      CB.storeTaintMirroredVarDecl(G);
  //  }
  //  else if (isTaintedStruct(G))
  //  {
  //    if (G->getParentFunctionOrMethod() == nullptr)
  //      CB.storeTaintMirroredStructVarDecl(G);
  //  }
  bool VisitVarDecl(VarDecl *D) {

    FullSourceLoc FL = Context->getFullLoc(D->getBeginLoc());
    // ParmVarDecls are skipped here, and are added in ProgramInfo::addVariable
    // as it processes a function
    // TODO: Need to handle here too
    if (FL.isValid() && !isa<ParmVarDecl>(D)){

    }
    return true;
  }

  bool VisitFunctionDecl(FunctionDecl *D) {

    return true;
  }

  bool VisitRecordDecl(RecordDecl *Declaration) {
    if (Declaration->isThisDeclarationADefinition()) {
      RecordDecl *Definition = Declaration->getDefinition();
      assert("Declaration is a definition, but getDefinition() is null?" &&
             Definition);
      FullSourceLoc FL = Context->getFullLoc(Definition->getBeginLoc());
      if (FL.isValid())
        for (auto *const D : Definition->fields())
        {}

    }
    return true;
  }

private:
  ASTContext *Context;
  ProgramVariableAdder &VarAdder;
};

void VariableAdderConsumer::HandleTranslationUnit(ASTContext &C) {
  Info.enterCompilationUnit(C);
  if (_CheckMateOpts.Verbose) {
    SourceManager &SM = C.getSourceManager();
    FileID MainFileId = SM.getMainFileID();
    const FileEntry *FE = SM.getFileEntryForID(MainFileId);
    if (FE != nullptr)
      errs() << "Analyzing file " << FE->getName() << "\n";
    else
      errs() << "Analyzing\n";
  }

  VariableAdderVisitor VAV = VariableAdderVisitor(&C, Info);
  TranslationUnitDecl *TUD = C.getTranslationUnitDecl();
  // Collect Variables.
  for (const auto &D : TUD->decls()) {
    VAV.TraverseDecl(D);
  }

  if (_CheckMateOpts.Verbose)
    errs() << "Done analyzing\n";

  Info.exitCompilationUnit();
  return;
}

void ConstraintBuilderConsumer::HandleTranslationUnit(ASTContext &C) {
  Info.enterCompilationUnit(C);
    SourceManager &SM = C.getSourceManager();
    FileID MainFileId = SM.getMainFileID();
    const FileEntry *FE = SM.getFileEntryForID(MainFileId);
    if (FE != nullptr)
      errs() << "Analyzing file " << FE->getName() << "\n";
    else
      errs() << "Analyzing\n";

  ConstraintResolver CSResolver(Info, &C);
  ConstraintGenVisitor GV = ConstraintGenVisitor(&C, Info);
  MyASTVisitor MyASS = MyASTVisitor(&C, Info);
  TranslationUnitDecl *TUD = C.getTranslationUnitDecl();
  StatsRecorder SR(&C, &Info);

  // Generate constraints.
  for (const auto &D : TUD->decls()) {
    // The order of these traversals CANNOT be changed because the constraint
    // gen visitor requires the type variable information gathered in the type
    // variable traversal.

  }


  for (const auto &D : TUD->decls()) {
    GV.TraverseDecl(D);
    MyASS.TraverseDecl(D);
    SR.TraverseDecl(D);
  }

  if (_CheckMateOpts.Verbose)
    errs() << "Done analyzing\n";
  Info.exitCompilationUnit();
  return;
}
