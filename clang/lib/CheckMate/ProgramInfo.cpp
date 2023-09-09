//=--ProgramInfo.cpp----------------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// Implementation of ProgramInfo methods.
//===----------------------------------------------------------------------===//

#include "clang/CheckMate/ProgramInfo.h"
#include "clang/CheckMate/CheckMateGlobalOptions.h"
#include "clang/CheckMate/MappingVisitor.h"
#include "clang/CheckMate/Utils.h"
#include "llvm/Support/JSON.h"
#include <sstream>

using namespace clang;

ProgramInfo::ProgramInfo() : Persisted(true) {
}

// Populate Variables, VarDeclToStatement, RVariables, and DepthMap with
// AST data structures that correspond do the data stored in PDMap and
// ReversePDMap.
void ProgramInfo::enterCompilationUnit(ASTContext &Context) {
  assert(Persisted);
  // Get a set of all of the PersistentSourceLoc's we need to fill in.
  std::set<PersistentSourceLoc> P;
  //for (auto I : PersistentVariables)
  //  P.insert(I.first);

  // Resolve the PersistentSourceLoc to one of Decl,Stmt,Type.
  MappingVisitor V(P, Context);
  TranslationUnitDecl *TUD = Context.getTranslationUnitDecl();
  for (const auto &D : TUD->decls())
    V.TraverseDecl(D);

  Persisted = false;
  return;
}

// Remove any references we maintain to AST data structure pointers.
// After this, the Variables, VarDeclToStatement, RVariables, and DepthMap
// should all be empty.
void ProgramInfo::exitCompilationUnit() {
  assert(!Persisted);
  Persisted = true;
  return;
}

void ProgramInfo::registerTranslationUnits(
    const std::vector<std::unique_ptr<clang::ASTUnit>> &ASTs) {
  assert(TranslationUnitIdxMap.empty());
  unsigned int Idx = 0;
  for (const auto &AST : ASTs) {
    TranslationUnitIdxMap[&(AST->getASTContext())] = Idx;
    Idx++;
  }
}

void ProgramInfo::storeTaintedDecl(FunctionDecl* declaration){
  this->TaintedDecls.push_back(declaration);
}