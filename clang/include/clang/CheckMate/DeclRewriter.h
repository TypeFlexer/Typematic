//=--DeclRewriter.h-----------------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// This file contains the DeclRewriter class which is used to rewrite variable
// declarations in a program using the checked pointers types solved for by the
// the conversion tool.
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_CheckMate_DECLREWRITER_H
#define LLVM_CLANG_CheckMate_DECLREWRITER_H

#include "clang/CheckMate/ConstraintBuilder.h"
#include "clang/CheckMate/RewriteUtils.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Stmt.h"
#include "clang/Rewrite/Core/Rewriter.h"

using namespace llvm;
using namespace clang;

class DeclRewriter {
public:
  DeclRewriter(Rewriter &R, ProgramInfo &Info, ASTContext &A)
      : R(R), Info(Info), A(A) {}

  // The publicly accessible interface for performing declaration rewriting.
  // All declarations for variables with checked types in the variable map of
  // Info parameter are rewritten.
  static void rewriteDecls(ASTContext &Context, ProgramInfo &Info, Rewriter &R);
  static void rewriteBody(ASTContext &Context, ProgramInfo &Info, Rewriter &R);
  // The publicly accessible interface for performing function body rewriting
  // The actual Body of the function will be commented out
  // The passed argument types will be instrumented as per the current Sandbox
  // option chosen. And a call with be issued to the tainted implementation of the
  // actual function
  static void rewriteFunctionBody(ASTContext &Context, ProgramInfo &Info, Rewriter &R);

private:
  Rewriter &R;
  ProgramInfo &Info;
  ASTContext &A;

  // List of TagDecls that were split from multi-decls and should be moved out
  // of an enclosing RecordDecl to avoid a compiler warning. Filled during
  // multi-decl rewriting and processed by denestTagDecls.
  std::vector<TagDecl *> TagDeclsToDenest;

  // Visit each Decl in ToRewrite and apply the appropriate pointer type
  // to that Decl. ToRewrite is the set of all declarations to rewrite.
  void rewrite(RSet &ToRewrite);

  //void rewriteMultiDecl(MultiDeclInfo &MDI, RSet &ToRewrite);
  //void doDeclRewrite(SourceRange &SR, DeclReplacement *N);
  void doBodyRewrite(SourceRange &SR, DeclReplacement *N);
  void rewriteFunctionBody(FunctionDeclReplacement *N);
  // Emit supplementary declarations _after_ the token that begins at Loc.
  // Inserts a newline before the first supplementary declaration but not after
  // the last supplementary declaration. This is suitable if Loc is expected to
  // be the last token on a line or if rewriteMultiDecl will insert a newline
  // after the supplementary declarations later.
  void emitSupplementaryDeclarations(const std::vector<std::string> &SDecls,
                                     SourceLocation Loc);
  SourceLocation getNextCommaOrSemicolon(SourceLocation L);
  void denestTagDecls();
  void emitTaintedFunctionInstrumentation(const std::vector<std::string> &TInsts,
                                          SourceLocation Loc);
};

// Visits function declarations and adds entries with their new rewritten
// declaration to the RSet RewriteThese.
class FunctionBodyBuilder : public RecursiveASTVisitor<FunctionBodyBuilder> {
public:
  explicit FunctionBodyBuilder(ASTContext *C, ProgramInfo &I, RSet &DR)
      : Context(C), Info(I), RewriteThese(DR),
        VisitedSet() {}

  bool VisitFunctionDecl(FunctionDecl *);
  bool isFunctionVisited(std::string FuncName);

protected:
  ASTContext *Context;
  ProgramInfo &Info;
  RSet &RewriteThese;

  // Set containing the names of all functions visited in the AST traversal.
  // Used to ensure the new signature is only computed once for each function.
  std::set<std::string> VisitedSet;

  // Get existing itype string from constraint variables.
  //std::string getExistingIType(ConstraintVariable *DeclC);

};
#endif // LLVM_CLANG_CheckMate_DECLREWRITER_H
