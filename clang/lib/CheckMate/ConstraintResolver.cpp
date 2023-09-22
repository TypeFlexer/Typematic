//=--ConstraintResolver.cpp---------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// Implementation of methods in ConstraintResolver.h that help in fetching
// constraints for a given expression.
//===----------------------------------------------------------------------===//

#include "clang/CheckMate/ConstraintResolver.h"
#include "clang/CheckMate/CheckMateGlobalOptions.h"
#include <map>

using namespace llvm;
using namespace clang;

ConstraintResolver::~ConstraintResolver() {}

// Force all ConstraintVariables in this set to be WILD

std::string base_name(std::string const & path)
{
  return path.substr(path.find_last_of("/\\") + 1);
}

inline std::string resolve_base_name(std::string const & path)
{
  return base_name(path);
}
void ConstraintResolver::storeTaintMirroredVarDecl(VarDecl* VD) {
  std::string TaintedDirPath = _CheckMateOpts.TaintedDefDir;
  if(TaintedDirPath.find_last_not_of("/\\"))
    TaintedDirPath += "/";

  auto VdSourceFileName = resolve_base_name(VD->getASTContext().getSourceManager()
                                           .getFilename(VD->getLocation()).str());

  std::string FinalPathWithFileName = TaintedDirPath + "Tainted"
                                      + VdSourceFileName;
  Info.TaintMirroredVarDecls[VD] = FinalPathWithFileName;

}

void ConstraintResolver::storeTaintMirroredStructVarDecl(RecordDecl* RD) {

  std::string TaintedDirPath = _CheckMateOpts.TaintedDefDir;
  if(TaintedDirPath.find_last_not_of("/\\"))
    TaintedDirPath += "/";

  auto VdSourceFileName = resolve_base_name(RD->getASTContext().getSourceManager()
                                                .getFilename(RD->getLocation()).str());

  std::string FinalPathWithFileName = TaintedDirPath + "Tainted"
                                      + VdSourceFileName;
  Info.TaintMirroredVarStructDecls[RD] = FinalPathWithFileName;

}

void ConstraintResolver::storeTaintMirroredTypedefDecl(TypedefDecl* TD) {

  std::string TaintedDirPath = _CheckMateOpts.TaintedDefDir;
  if(TaintedDirPath.find_last_not_of("/\\"))
    TaintedDirPath += "/";

  auto VdSourceFileName = resolve_base_name(TD->getASTContext().getSourceManager()
                                                .getFilename(TD->getLocation()).str());

  std::string FinalPathWithFileName = TaintedDirPath + "Tainted"
                                      + VdSourceFileName;
  Info.TaintMirroredTypedefDecls[TD] = FinalPathWithFileName;

}
void ConstraintResolver::storeTaintMirroredEnumDecl
    (EnumDecl* ED) {

  std::string TaintedDirPath = _CheckMateOpts.TaintedDefDir;
  if(TaintedDirPath.find_last_not_of("/\\"))
    TaintedDirPath += "/";

  auto VdSourceFileName = resolve_base_name(ED->getASTContext().getSourceManager()
                                                .getFilename(ED->getLocation()).str());

  std::string FinalPathWithFileName = TaintedDirPath + "Tainted"
                                      + VdSourceFileName;
  Info.TaintMirroredEnumDecls[ED] = FinalPathWithFileName;

}
void ConstraintResolver::storeIncludeStatement(SourceLocation SLC,
                                               SourceManager &SM,
                                               std::string IncludeStmt) {
  std::string TaintedDirPath = _CheckMateOpts.TaintedDefDir;
  if(TaintedDirPath.find_last_not_of("/\\"))
    TaintedDirPath += "/";

  auto SourceFileName = resolve_base_name(SM.getFilename(SLC).str());

  std::string FinalPathWithFileName = TaintedDirPath + "Tainted"
                                      + SourceFileName;
  /*
   * Insert only if IncludeStmt has never been inserted before
   */

  errs()<<"Inserting macro: " << IncludeStmt<<"\n";
  if (Info.MapTaintedFile2IncludeStmt.find(IncludeStmt)
      == Info.MapTaintedFile2IncludeStmt.end()) {
    Info.MapTaintedFile2IncludeStmt.insert(std::pair<std::string, std::string>(
        IncludeStmt, FinalPathWithFileName));
  }
}

void ConstraintResolver::storeTaintedFunctionDecl(FunctionDecl* FD){
  Info.storeTaintedDecl(FD);
  /*
   * Check if the map has an entry(filename) corresponding to this FD's source file
   */
  std::string FdSourceFileName;

  /*
   * This FD belongs to a new file, hence a new file has to be created for
   * this tainted decl
  */
  //fetch the tainted directory path -->
  std::string TaintedDirPath = _CheckMateOpts.TaintedDefDir;
  if(TaintedDirPath.find_last_not_of("/\\"))
    TaintedDirPath += "/";

  FdSourceFileName = resolve_base_name(FD->getASTContext().getSourceManager()
                                              .getFilename(FD->getLocation()).str());

  std::string FinalPathWithFileName = TaintedDirPath + "Tainted"
                                          + FdSourceFileName;

  if(std::find(Info.TaintedRewriteFileVector.begin(),
                                            Info.TaintedRewriteFileVector.end(),
                FinalPathWithFileName.c_str())
      == Info.TaintedRewriteFileVector.end()){

    Info.TaintedRewriteFileVector.push_back(FinalPathWithFileName.c_str());
  }

  Info.TaintedFuncStreamWriter.insert({FD, FinalPathWithFileName});
}

void ConstraintResolver::storeCallbackFunctionDecl(FunctionDecl* FD){
  /*
   * Check if the map has an entry(filename) corresponding to this FD's source file
   */
  std::string FdSourceFileName;

  /*
   * This FD belongs to a new file, hence a new file has to be created for
   * this tainted decl
  */
  //fetch the tainted directory path -->
  std::string TaintedDirPath = _CheckMateOpts.TaintedDefDir;
  if(TaintedDirPath.find_last_not_of("/\\"))
    TaintedDirPath += "/";

  FdSourceFileName = resolve_base_name(FD->getASTContext().getSourceManager()
                                           .getFilename(FD->getLocation()).str());

  std::string FinalPathWithFileName = TaintedDirPath + "Tainted"
                                      + FdSourceFileName;
  Info.CallbackFuncStreamWriter.insert({FD, FinalPathWithFileName});
}

static bool getSizeOfArg(Expr *Arg, QualType &ArgTy) {
  Arg = Arg->IgnoreParenImpCasts();
  if (auto *SizeOf = dyn_cast<UnaryExprOrTypeTraitExpr>(Arg))
    if (SizeOf->getKind() == UETT_SizeOf) {
      ArgTy = SizeOf->getTypeOfArgument();
      return true;
    }
  return false;
}

// Returns a pair of set of ConstraintVariables and set of BoundsKey
// after evaluating the expression E. Will explore E recursively, but will
// ignore parts of it that do not contribute to the final result.
