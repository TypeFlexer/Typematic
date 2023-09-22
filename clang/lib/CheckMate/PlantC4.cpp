//=--RewriteUtils.cpp---------------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// This class contains implementation of the functions and
// classes of PlantC4.h
//===----------------------------------------------------------------------===//

#include "clang/CheckMate/PlantC4.h"
#include "clang/CheckMate/CheckMateGlobalOptions.h"
#include "clang/Tooling/Transformer/SourceCode.h"
#include <stdlib.h>
#include <unistd.h>
using namespace llvm;
using namespace clang;
char* PlantC4::getFileNameFromPath(const char* path )
{
  if( path == NULL )
    return NULL;

  char * pFileName = const_cast<char*>(path);
  for( char * pCur = const_cast<char*>(path); *pCur != '\0'; pCur++)
  {
    if( *pCur == '/' || *pCur == '\\' )
      pFileName = pCur+1;
  }

  return pFileName;
}

void PlantC4::fetch_path_from_file_path(std::string & file_path, const std::string & fileName)
{
  // Search for the substring in string
  size_t pos = file_path.find(fileName);
  if (pos != std::string::npos)
  {
    // If found then erase it from string
    file_path.erase(pos, fileName.length());
  }
}

bool PlantC4::ConvertTaintedToVanilla() {

  auto ListOfTaintedFiles = Info.TaintedRewriteFileVector;
  for(auto TaintedFilePath : ListOfTaintedFiles){
    // switch to C4 directory to execute the script
    chdir("../tools/C4");

    std::string Command ="";
    Command = "./exec.sh " + TaintedFilePath;
    std::system(Command.c_str());
  }
  return true;
}
