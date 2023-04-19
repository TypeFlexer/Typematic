//=--3CGlobalOptions.h--------------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Tool options that are visible to all the components.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_CheckMate_CheckMateGLOBALOPTIONS_H
#define LLVM_CLANG_CheckMate_CheckMateGLOBALOPTIONS_H

#include "llvm/Support/CommandLine.h"

// Options used to initialize CheckMate tool.
//
// See clang/docs/checkedc/CheckMate/clang-tidy.md#_3c-name-prefix
// NOLINTNEXTLINE(readability-identifier-naming)

struct _CheckMateOptions {
  bool DumpIntermediate;
  bool Verbose;
  std::string OutputPostfix;
  std::string OutputDir;
  std::string ConstraintOutputJson;
  bool DumpStats;
  std::string StatsOutputJson;
  std::string WildPtrInfoJson;
  std::string PerWildPtrInfoJson;
  std::vector<std::string> AllocatorFunctions;
  bool HandleVARARGS;
  std::string BaseDir;
  bool AllowSourcesOutsideBaseDir;
  bool AllTypes;
  bool AddCheckedRegions;
  std::string AddSandbox;
  std::string TaintedDefDir;
  bool EnableCCTypeChecker;
  bool WarnRootCause;
  bool WarnAllRootCause;
  bool DumpUnwritableChanges;
  bool AllowUnwritableChanges;
  bool AllowRewriteFailures;
  bool ItypesForExtern;
  bool InferTypesForUndefs;
  bool DebugSolver;
  bool OnlyGreatestSol;
  bool OnlyLeastSol;
  bool DisableRDs;
  bool DisableFunctionEdges;
  bool DisableInfDecls;
  bool DisableArrH;
  bool DebugArrSolver;
#ifdef FIVE_C
  bool RemoveItypes;
  bool ForceItypes;
#endif
};

// NOLINTNEXTLINE(readability-identifier-naming)
extern struct _CheckMateOptions _CheckMateOpts;

#endif // LLVM_CLANG_CheckMate_CheckMateGLOBALOPTIONS_H
