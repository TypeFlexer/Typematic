//=--RewriteUtils.h-----------------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See hCheckMateps://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// This class contains functions and classes that deal with
// rewriting the source file After Appropriate CheckCBox Conversion .
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_CheckMate_PLANTC4_H
#define LLVM_CLANG_CheckMate_PLANTC4_H

#include "clang/CheckMate/ProgramInfo.h"

using namespace clang;

class PlantC4 : public ProgramInfo {
public:
  explicit PlantC4(ProgramInfo &I) : Info(I) {}

  bool ConvertTaintedToVanilla();
private:
  ProgramInfo &Info;

  char *getFileNameFromPath(const char *path);
  void fetch_path_from_file_path(std::string &file_path,
                                 const std::string &fileName);
};

#endif // LLVM_CLANG_CheckMate_PLANTC4_H
