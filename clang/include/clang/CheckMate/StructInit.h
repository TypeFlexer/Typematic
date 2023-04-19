//=--StructInit.h-----------------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// This class contains functions and classes that deal with
// adding initializers to struct variables during the rewriting phase
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_CheckMate_STRUCTINIT_H
#define LLVM_CLANG_CheckMate_STRUCTINIT_H

#include "clang/CheckMate/ConstraintResolver.h"
#include "clang/CheckMate/MappingVisitor.h"
#include "clang/CheckMate/RewriteUtils.h"
#include "clang/CheckMate/StructInit.h"
#include "clang/CheckMate/Utils.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Tooling/Transformer/SourceCode.h"
#include "llvm/Support/raw_ostream.h"
#include <sstream>

using namespace clang;
using namespace llvm;

class TStructVariableInitializer
    : public RecursiveASTVisitor<TStructVariableInitializer> {
public:
  explicit TStructVariableInitializer(ASTContext *C, ProgramInfo &I, RSet &R)
      : Context(C), I(I), RewriteThese(R), RecordsWithCPointers() {}

  bool VisitVarDecl(VarDecl *VD);
  // The VarDecl Passed to this must be the newly generated _TPtr<TStruct> type
  bool InitializeTStructthathasTStructMembers(VarDecl *VD);
private:
  bool hasCheckedMembers(DeclaratorDecl *DD);
  bool TStructHasTStructMembers(DeclaratorDecl *DD);

  ASTContext *Context;
  ProgramInfo &I;
  RSet &RewriteThese;
  std::set<RecordDecl *> RecordsWithCPointers;
  std::set<RecordDecl *> TStructRecordsWithTStructMembers;
};
#endif
