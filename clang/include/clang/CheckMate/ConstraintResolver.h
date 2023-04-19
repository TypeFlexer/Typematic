//=--ConstraintResolver.h-----------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// Class that helps in resolving constraints for various expressions.
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_CheckMate_CONSTRAINTRESOLVER_H
#define LLVM_CLANG_CheckMate_CONSTRAINTRESOLVER_H

#include "clang/CheckMate/ProgramInfo.h"
#include "clang/AST/ASTConsumer.h"

using namespace llvm;
using namespace clang;

// Class that handles building constraints from various AST artifacts.
class ConstraintResolver {

public:
  ConstraintResolver(ProgramInfo &I, ASTContext *C) : Info(I), Context(C) {}

  virtual ~ConstraintResolver();

  void storeTaintedFunctionDecl(FunctionDecl* FD);

  bool isCastofGeneric(CastExpr *C);

  // Handle assignment of RHS expression to LHS expression using the
  // given action.
  static bool canFunctionBeSkipped(const std::string &FN);

  void storeTaintMirroredVarDecl(VarDecl *VD);

  void storeTaintMirroredStructVarDecl(RecordDecl *RD);
  void storeTaintMirroredTypedefDecl(TypedefDecl *TD);
  void storeIncludeStatement(SourceLocation SLC,
                        SourceManager &SM,
                        std::string IncludeStmt);

  void storeTaintMirroredEnumDecl(EnumDecl *ED);

  void storeCallbackFunctionDecl(FunctionDecl *FD);

private:
  ProgramInfo &Info;
  ASTContext *Context;
  static bool snapshotTaintedFunction(Decl* declaration);

  bool isNonPtrType(QualType &TE);
};

#endif // LLVM_CLANG_CheckMate_CONSTRAINTRESOLVER_H
