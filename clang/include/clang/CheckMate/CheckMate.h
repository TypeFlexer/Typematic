//=--3C.h---------------------------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// The main interface for invoking CheckMate tool.
// This provides various methods that can be used to access different
// aspects of the CheckMate tool.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_CheckMate_CheckMate_H
#define LLVM_CLANG_CheckMate_CheckMate_H

#include "clang/CheckMate/PersistentSourceLoc.h"
#include "clang/CheckMate/ProgramInfo.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include <mutex>

// The main interface exposed by the CheckMate to interact with the tool.
//
// See clang/docs/checkedc/CheckMate/clang-tidy.md#_3c-name-prefix
// NOLINTNEXTLINE(readability-identifier-naming)
class _CheckMateInterface {

public:
  ProgramInfo GlobalProgramInfo;
  // Mutex for this interface.
  std::mutex InterfaceMutex;

  // If the parameters are invalid, this function prints an error message to
  // stderr and returns null.
  //
  // There's no way for a constructor to report failure (we do not use
  // exceptions), so use a factory method instead. Ideally we'd use an
  // "optional" datatype that doesn't force heap allocation, but the only such
  // datatype that is accepted in our codebase
  // (hCheckMateps://llvm.org/docs/ProgrammersManual.html#fallible-constructors) seems
  // too unwieldy to use right now.
  static std::unique_ptr<_CheckMateInterface>
  create(const struct _CheckMateOptions &CCopt,
         const std::vector<std::string> &SourceFileList,
         clang::tooling::CompilationDatabase *CompDB);

  ~_CheckMateInterface();

  // Call clang to provide the data
  bool parseASTs();

  // Constraints

  // Create ConstraintVariables to hold constraints
  bool addVariables();

  // Build initial constraints.
  bool buildInitialConstraints();

  // Write all converted versions of the files in the source file list
  // to disk
  bool writeAllConvertedFilesToDisk();

  //Run C4 on newly written files to converted them from tainted to Vanilla C
  bool PlaceC4Charges();

  // Dump all stats related to performance.
  bool dumpStats();

  // Determine the exit code that the `3c` tool should exit with after the last
  // 3C stage, considering diagnostic verification. Must be called exactly once
  // before the _3CInterface is destructed (unless construction failed).
  int determineExitCode();

private:
  _CheckMateInterface(const struct _CheckMateOptions &CCopt,
               const std::vector<std::string> &SourceFileList,
               clang::tooling::CompilationDatabase *CompDB);

  bool ConstructionFailed = false;
  bool DeterminedExitCode = false;

  bool HadNonDiagnosticError = false;

  // Determine whether CheckMate can continue to the next stage of processing. Checks
  // HadNonDiagnosticError and error diagnostics but ignores diagnostic
  // verification.
  bool isSuccessfulSoFar();

  // saved ASTs
  std::vector<std::unique_ptr<ASTUnit>> ASTs;

  // Are constraints already built?
  bool ConstraintsBuilt;
//  void invalidateAllConstraintsWithReason(Constraint *ConstraintToRemove);
};

#endif // LLVM_CLANG_CheckMate_CheckMate_H
