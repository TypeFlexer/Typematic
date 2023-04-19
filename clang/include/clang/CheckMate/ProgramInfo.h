//=--ProgramInfo.h------------------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// This class represents all the information about a source file
// collected by the converter.
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_CheckMate_PROGRAMINFO_H
#define LLVM_CLANG_CheckMate_PROGRAMINFO_H

#include "clang/CheckMate/MultiDecls.h"
#include "clang/CheckMate/PersistentSourceLoc.h"
#include "clang/CheckMate/Utils.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

class ProgramVariableAdder {
public:

};

// The pair of CVs are the type param constraint and an optional
// constraint used to get the generic index. A better solution would have
// generic constraints saved within ConstraintVariables, but those don't
// exist at this time.
struct TypeParamConstraint {
  ConstraintVariable *MainConstraint;
  ConstraintVariable *GenericAddition;
  TypeParamConstraint() :
      MainConstraint(nullptr), GenericAddition(nullptr) {}
  TypeParamConstraint(ConstraintVariable *M, ConstraintVariable *G) :
      MainConstraint(M), GenericAddition(G) {}
  // Fast. Whether `getConstraint` will return something other than nullptr.
  bool isConsistent() const { return MainConstraint != nullptr; }
};

class ProgramInfo : public ProgramVariableAdder {
public:

  // This map holds similar information as the type variable map in
  // ConstraintBuilder.cpp, but it is stored in a form that is usable during
  // rewriting.
  typedef std::map<unsigned int, TypeParamConstraint> CallTypeParamBindingsT;
  std::vector<Decl*>TaintedDecls;

  /*
   * This is the mapping between the FunctionDecl's actual file name to the
   * tainted file to which its tainted generic C definition will be written
   */
  std::vector<std::string> TaintedRewriteFileVector;
  //the below vector will help in close output stream that are still not closed
  /*
   * You gotta extend this from being just a vector to a map
   */
  std::list<std::string> TaintedOutfiles;
  std::vector<llvm::raw_fd_ostream*> W2cOutfiles;
  /*
   * Stream writers are to map between the Decl and its corresponding file
   */
  std::map<Decl*, std::string> TaintedFuncStreamWriter;
  std::map<FunctionDecl*, std::string>CallbackFuncStreamWriter;
  std::map<Decl*, std::string> TaintedVarDeclStreamWriter;
  std::map<Decl*, std::string> TaintedStructDeclStreamWriter;
  /*
   * Unordered List
   */
  std::map<std::string, Decl*> ListOfInstrumentedFiles;

  std::map<Decl*, std::string> W2cHeaderStreamWriter;
  std::map<Decl*, std::string> TaintMirroredVarDecls;
  std::map<RecordDecl*, std::string> TaintMirroredVarStructDecls;
  std::map<TypedefDecl*, std::string> TaintMirroredTypedefDecls;
  std::map<EnumDecl*, std::string> TaintMirroredEnumDecls;
  std::map<std::string, std::string> MapTaintedFile2IncludeStmt;

  ProgramInfo();

  void print(llvm::raw_ostream &O) const;
  void dump() const { print(llvm::errs()); }
  void dumpJson(llvm::raw_ostream &O) const;
  void dumpStats(const std::set<std::string> &F) {
    printStats(F, llvm::errs());
  }
  void printStats(const std::set<std::string> &F, llvm::raw_ostream &O,
                  bool OnlySummary = false, bool JsonFormat = false);

  void printAggregateStats(const std::set<std::string> &F,
                           llvm::raw_ostream &O);

  // Populate Variables, VarDeclToStatement, RVariables, and DepthMap with
  // AST data structures that correspond do the data stored in PDMap and
  // ReversePDMap.
  void enterCompilationUnit(clang::ASTContext &Context);

  // Remove any references we maintain to AST data structure pointers.
  // After this, the Variables, VarDeclToStatement, RVariables, and DepthMap
  // should all be empty.
  void exitCompilationUnit();

  void storeTaintedDecl(FunctionDecl* Declaration);
  bool hasPersistentConstraints(clang::Expr *E, ASTContext *C) const;
  // Get only constraint vars from the persistent contents of the
  // expression E.
  void removePersistentConstraints(Expr *E, ASTContext *C);

  // Called when we are done adding constraints and visiting ASTs.
  // Links information about global symbols together and adds
  // constraints where appropriate.
  bool link();

  const VariableMap &getVarMap() const { return Variables; }

  bool computeInterimConstraintState(const std::set<std::string> &FilePaths);

  void setTypeParamBinding(CallExpr *CE, unsigned int TypeVarIdx,
                           ConstraintVariable *CV,
                           ConstraintVariable* Ident, ASTContext *C);
  bool hasTypeParamBindings(CallExpr *CE, ASTContext *C) const;
  const CallTypeParamBindingsT &getTypeParamBindings(CallExpr *CE,
                                                     ASTContext *C) const;

  // Store mapping from ASTContexts to a unique index in the ASTs vector in
  // the ProgramInfo object. This function must be called prior to any AST
  // traversals so that the map is populated.
  void registerTranslationUnits(
      const std::vector<std::unique_ptr<clang::ASTUnit>> &ASTs);

  ProgramMultiDeclsInfo TheMultiDeclsInfo;

private:
  // List of constraint variables for declarations, indexed by their location in
  // the source. This information persists across invocations of the constraint
  // analysis from compilation unit to compilation unit.
  VariableMap Variables;

  // A pair containing an AST node ID and an index that uniquely identifies the
  // translation unit. Translation unit identifiers are drawn from the
  // TranslationUnitIdxMap. Used as a key to index expression in the following
  // maps.
  typedef std::pair<int64_t, unsigned int> IDAndTranslationUnit;
  IDAndTranslationUnit getExprKey(clang::Expr *E, clang::ASTContext *C) const;


  // For each expr stored in the ExprConstraintVars, also store the source
  // location for the expression. This is used to emit diagnostics. It is
  // expected that multiple entries will map to the same source location.
  std::map<IDAndTranslationUnit, PersistentSourceLoc> ExprLocations;

  // This map holds similar information as the type variable map in
  // ConstraintBuilder.cpp, but it is stored in a form that is usable during
  // rewriting.
  typedef std::map<IDAndTranslationUnit, CallTypeParamBindingsT>
      TypeParamBindingsT;


  // Is the ProgramInfo persisted? Only tested in asserts. Starts at true.
  bool Persisted;

  // Map of global decls for which we don't have a definition, the keys are
  // names of external vars, the value is whether the def
  // has been seen before.
  std::map<std::string, bool> ExternGVars;

  // For each call to a generic function, remember how the type parameters were
  // instantiated so they can be inserted during rewriting.
  TypeParamBindingsT TypeParamBindings;

  // Maps each ASTContext to a unique index in the vector of ASTs being
  // processed. This is used to uniquely determine the translation unit an AST
  // belongs to given its corresponding ASTContext. By using this index instead
  // of the name of the main file, this uniquely identifies the translation unit
  // even when a file is the main file of multiple translation units. The values
  // in this map are used as part of the IDAndTranslationUnit which is the type
  // used as keys for maps from ASTNodes.
  std::map<ASTContext *, unsigned int> TranslationUnitIdxMap;

  void insertIntoPtrSourceMap(PersistentSourceLoc PSL, ConstraintVariable *CV);

  void computePtrLevelStats();
};

#endif
