//=--DeclRewriter.cpp---------------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "clang/CheckMate/DeclRewriter.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Stmt.h"
#include "clang/CheckMate/CheckMateGlobalOptions.h"
#include "clang/CheckMate/RewriteUtils.h"
#include "clang/CheckMate/Utils.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <ostream>
#include <unistd.h>

using namespace llvm;
using namespace clang;

void eraseSubStr(std::string & MainStr, const std::string & ToErase)
{
  // Search for the substring in string
  size_t pos = MainStr.find(ToErase);
  if (pos != std::string::npos)
  {
    // If found then erase it from string
    MainStr.erase(pos, ToErase.length());
  }
}

SourceLocation
endOfTheEnd(SourceLocation const & StartOfEnd, SourceManager & Sm){
  LangOptions lopt;
  return Lexer::getLocForEndOfToken(StartOfEnd, 0, Sm, lopt);
}

enum CurrDeclType{
  CD_VarStructDecl,
  CD_GenVarDecl,
  CD_FuncDecl,
  CD_VarTypeDefDecl,
  CD_Macro,
  CD_Enum,
  CD_CallbackFuncDecl,
  CD_Unknown
};

bool initializeTaintedFileForDeclIfNeeded(ASTContext &Context, ProgramInfo &Info,
                                          Rewriter &R, Decl* D = nullptr,
                                          RecordDecl* RD = nullptr,
                                          TypedefDecl* TD = nullptr,
                                          EnumDecl* ED = nullptr,
                                          RewriteBuffer* RB = nullptr,
                                          std::string
                                              TaintedFileAttemptingToBeGenerated = "",
                                          CurrDeclType CD = CD_Unknown) {
  std::error_code ErrorCode;
  bool IsNeedsToBeInitialized = false;
  if (CD == CD_GenVarDecl) {
    assert(D !=nullptr);
    TaintedFileAttemptingToBeGenerated = Info.TaintMirroredVarDecls[D];
  }
  else if (CD == CD_VarStructDecl) {
    assert(RD !=nullptr);
    TaintedFileAttemptingToBeGenerated = Info.TaintMirroredVarStructDecls[RD];
  }
  else if (CD == CD_VarTypeDefDecl) {
    assert(TD !=nullptr);
    TaintedFileAttemptingToBeGenerated = Info.TaintMirroredTypedefDecls[TD];
  }
  else if (CD == CD_Enum)
  {
    assert(ED !=nullptr);
    TaintedFileAttemptingToBeGenerated = Info.TaintMirroredEnumDecls[ED];
  }
  else if (CD == CD_FuncDecl) {
    assert(D !=nullptr);
    TaintedFileAttemptingToBeGenerated = Info.TaintedFuncStreamWriter[D];
  }
  else if (CD == CD_CallbackFuncDecl) {
    assert(D !=nullptr);
    TaintedFileAttemptingToBeGenerated = Info.CallbackFuncStreamWriter[D->getAsFunction()];
  }

  llvm::raw_fd_ostream OutFile(TaintedFileAttemptingToBeGenerated, ErrorCode,
                               llvm::sys::fs::OF_Append);
  auto findIter = std::find(Info.TaintedOutfiles.begin(),
                            Info.TaintedOutfiles.end(),
                            TaintedFileAttemptingToBeGenerated);
  if (findIter ==
      Info.TaintedOutfiles.end())
    IsNeedsToBeInitialized = true;
  if (IsNeedsToBeInitialized) {
    /*
     * This is a new file being written, hence Initialize it
     */

    RB->Initialize("/* This file is Auto-Generated Using CheckMate.\n Please Do Not Directly Modify."
                   "\n */ \n");
    RB->write(OutFile);
    Info.TaintedOutfiles.push_back(TaintedFileAttemptingToBeGenerated);
  }
}

bool copyTaintedStructDefToTaintedFile(ASTContext &Context, ProgramInfo &Info,
                                       Rewriter &R, RecordDecl* RD, RewriteBuffer* RB){



  SourceRange DeclRange(RD->getSourceRange());
  SourceLocation DeclBegin(DeclRange.getBegin());
  SourceLocation DeclStartEnd(DeclRange.getEnd());
  std::error_code ErrorCode;
  SourceLocation DeclEndEnd(endOfTheEnd(DeclStartEnd,
                                        Context.getSourceManager()));
  /*
     * Now you can fetch the pointers to the text replacement
     */
  const char* BuffBegin(Context.getSourceManager().getCharacterData(DeclBegin));
  const char* BuffEnd(Context.getSourceManager().getCharacterData(DeclEndEnd));
  std::string const VarString(BuffBegin, BuffEnd);
  RB->Initialize("\n" + VarString + ";\n");
  llvm::raw_fd_ostream OutFile(Info.TaintMirroredVarStructDecls[
                                   RD],
                               ErrorCode, llvm::sys::fs::OF_Append);
  RB->write(OutFile);
  return true;
}

bool copyMacroDefToTaintedFile(ASTContext &Context, ProgramInfo &Info,
                               Rewriter &R, std::string TaintedFile,
                               std::string MacroDef,
                               RewriteBuffer* RB){




  std::error_code ErrorCode;
  // Undefs will be avoided for now
  if(MacroDef.find("undef") != std::string::npos)
    return true;

  RB->Initialize("\n" + MacroDef + "\n");
  llvm::raw_fd_ostream OutFile(TaintedFile,
                               ErrorCode, llvm::sys::fs::OF_Append);
  RB->write(OutFile);
  return true;
}

bool copyTaintedTypeDefToTaintedFile(ASTContext &Context, ProgramInfo &Info,
                                     Rewriter &R, TypedefDecl* TD, RewriteBuffer* RB){
  SourceRange DeclRange(TD->getSourceRange());
  SourceLocation DeclBegin(DeclRange.getBegin());
  SourceLocation DeclStartEnd(DeclRange.getEnd());
  std::error_code ErrorCode;
  SourceLocation DeclEndEnd(endOfTheEnd(DeclStartEnd,
                                        Context.getSourceManager()));
  /*
     * Now you can fetch the pointers to the text replacement
     */
  const char* BuffBegin(Context.getSourceManager().getCharacterData(DeclBegin));
  const char* BuffEnd(Context.getSourceManager().getCharacterData(DeclEndEnd));
  std::string const VarString(BuffBegin, BuffEnd);
  RB->Initialize("\n" + VarString + ";\n");
  llvm::raw_fd_ostream OutFile(Info.TaintMirroredTypedefDecls[TD],
                               ErrorCode, llvm::sys::fs::OF_Append);
  RB->write(OutFile);
  return true;
}

bool copyTaintedEnumToTaintedFile(ASTContext &Context, ProgramInfo &Info,
                                  Rewriter &R, EnumDecl* TD, RewriteBuffer* RB){
  SourceRange DeclRange(TD->getSourceRange());
  SourceLocation DeclBegin(DeclRange.getBegin());
  SourceLocation DeclStartEnd(DeclRange.getEnd());
  std::error_code ErrorCode;
  SourceLocation DeclEndEnd(endOfTheEnd(DeclStartEnd,
                                        Context.getSourceManager()));
  /*
     * Now you can fetch the pointers to the text replacement
     */
  const char* BuffBegin(Context.getSourceManager().getCharacterData(DeclBegin));
  const char* BuffEnd(Context.getSourceManager().getCharacterData(DeclEndEnd));
  std::string const VarString(BuffBegin, BuffEnd);
  RB->Initialize("\n" + VarString + ";\n");
  llvm::raw_fd_ostream OutFile(Info.TaintMirroredEnumDecls[TD],
                               ErrorCode, llvm::sys::fs::OF_Append);
  RB->write(OutFile);
  return true;
}

bool copyTaintedVarDefToTaintedFile(ASTContext &Context, ProgramInfo &Info,
                                    Rewriter &R, Decl* VD, RewriteBuffer* RB){



  SourceRange DeclRange(VD->getSourceRange());
  SourceLocation DeclBegin(DeclRange.getBegin());
  SourceLocation DeclStartEnd(DeclRange.getEnd());
  std::error_code ErrorCode;
  SourceLocation DeclEndEnd(endOfTheEnd(DeclStartEnd,
                                        Context.getSourceManager()));
  /*
     * Now you can fetch the pointers to the text replacement
     */
  const char* BuffBegin(Context.getSourceManager().getCharacterData(DeclBegin));
  const char* BuffEnd(Context.getSourceManager().getCharacterData(DeclEndEnd));
  std::string const VarString(BuffBegin, BuffEnd);
  RB->Initialize("\n" + VarString + ";\n");
  llvm::raw_fd_ostream OutFile(Info.TaintMirroredVarDecls[VD],
                               ErrorCode, llvm::sys::fs::OF_Append);
  RB->write(OutFile);
  return true;
}

bool copyTaintedDefToTaintedFile(ASTContext &Context, ProgramInfo &Info,
                                 Rewriter &R, Decl* FD, RewriteBuffer* RB){

  SourceRange DeclRange(FD->getSourceRange());
  SourceLocation DeclBegin(DeclRange.getBegin());
  SourceLocation DeclStartEnd(DeclRange.getEnd());
  std::error_code ErrorCode;
  SourceLocation DeclEndEnd(endOfTheEnd(DeclStartEnd,
                                        Context.getSourceManager()));
  /*
     * Now you can fetch the pointers to the text replacement
     */
  const char* BuffBegin(Context.getSourceManager().getCharacterData(DeclBegin));
  const char* BuffEnd(Context.getSourceManager().getCharacterData(DeclEndEnd));
  std::string const FuncString(BuffBegin, BuffEnd);
  llvm::raw_fd_ostream OutFile(Info.TaintedFuncStreamWriter[FD],
                               ErrorCode, llvm::sys::fs::OF_Append);
  if(FD->getAsFunction()->isThisDeclarationADefinition())
    RB->Initialize("\n" + FuncString + "\n");
  else
    RB->Initialize("\n" + FuncString + ";\n");

  RB->write(OutFile);
  /*
   * If this declaration is a definition, this means that this file will
   * have atleast one tainted w2c instrumentation.
   */
  if((FD->getAsFunction()->isThisDeclarationADefinition())
      && (Info.ListOfInstrumentedFiles.find(Info.TaintedFuncStreamWriter[FD])
          == Info.ListOfInstrumentedFiles.end()))
  {
    Info.ListOfInstrumentedFiles[Info.TaintedFuncStreamWriter[FD]] = FD;
  }
  return true;
}

bool copyCallbackDefToTaintedFile(ASTContext &Context, ProgramInfo &Info,
                                  Rewriter &R, FunctionDecl* FD, RewriteBuffer* RB){

  SourceRange DeclRange(FD->getSourceRange());
  SourceLocation DeclBegin(DeclRange.getBegin());
  SourceLocation DeclStartEnd(DeclRange.getEnd());
  std::error_code ErrorCode;
  SourceLocation DeclEndEnd(endOfTheEnd(DeclStartEnd,
                                        Context.getSourceManager()));
  /*
     * Now you can fetch the pointers to the text replacement
     */
  const char* BuffBegin(Context.getSourceManager().getCharacterData(DeclBegin));
  const char* BuffEnd(Context.getSourceManager().getCharacterData(DeclEndEnd));
  std::string const FuncString(BuffBegin, BuffEnd);
  llvm::raw_fd_ostream OutFile(Info.CallbackFuncStreamWriter[FD],
                               ErrorCode, llvm::sys::fs::OF_Append);
  if(FD->getAsFunction()->isThisDeclarationADefinition())
    RB->Initialize("\n" + FuncString + "\n");
  else
    RB->Initialize("\n" + FuncString + ";\n");

  RB->write(OutFile);
  return true;
}

char* getFileNameFromPath(const char* path )
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
void fetch_path_from_file_path(std::string & file_path, const std::string & fileName)
{
  // Search for the substring in string
  size_t pos = file_path.find(fileName);
  if (pos != std::string::npos)
  {
    // If found then erase it from string
    file_path.erase(pos, fileName.length());
  }
}
bool CopyW2CDefToW2CFile(ASTContext &Context, ProgramInfo &Info,
                         Rewriter &R, Decl* FD, std::string func_string){

  std::error_code error_code;
  std::string FilePath = Info.TaintedFuncStreamWriter[FD];
  std::string InstrumentedFileName =
      getFileNameFromPath(Info.TaintedFuncStreamWriter[FD].c_str());
  fetch_path_from_file_path(FilePath,
                            getFileNameFromPath(FilePath.c_str()));
  eraseSubStr(InstrumentedFileName, "Tainted");
  auto Wasm2CFilePath = FilePath + "WASM2C" + InstrumentedFileName;
  Wasm2CFilePath[Wasm2CFilePath.size()-1] = 'h';
  llvm::raw_fd_ostream OutFile(Wasm2CFilePath,
                               error_code, llvm::sys::fs::OF_Append);
  RewriteBuffer RB;
  if(std::find(Info.W2cOutfiles.begin(),
                Info.W2cOutfiles.end(),
                const_cast<llvm::raw_fd_ostream*>(&OutFile)) == Info.W2cOutfiles.end()){
    /*
       * This is a new file being written, hence Initialize it
       */

   std::string WASMLIBSCOPE = "/* This file is Auto-Generated Using CheckCBox Converter."
        " Please Do Not Directly Modify."
                  "\n */ \n""#pragma TLIB_SCOPE push\n"
                  "#pragma TLIB_SCOPE on\n";
    RB.Initialize(WASMLIBSCOPE);
    RB.write(OutFile);
  }

  RB.Initialize("\n" + func_string + ";\n");

  RB.write(OutFile);
  /*
 * Nice technique to see if the current element is last element
  for (auto& elem : item_vector) {
    if (&elem != &item_vector.back()) printf(", ");
    // ...
  }
*/
  if(FD == Info.TaintedDecls.back())
  {
    /*
     * Close the pragma as we are at the end of the Tainted file
     */
    RB.Initialize("#pragma TLIB_SCOPE pop");
    RB.write(OutFile);
  }
  Info.W2cOutfiles.push_back(&OutFile);



  return true;
}

typedef enum {
  WASM_RT_I32,
  WASM_RT_I64,
  WASM_RT_F32,
  WASM_RT_F64,
} wasm_rt_type_t;

wasm_rt_type_t ReturnWasmArgType(ParmVarDecl *PV){
  if(PV->getType()->isVoidType()){
    return wasm_rt_type_t::WASM_RT_I32;
  }
  else if(PV->getType()->isTaintedPointerType()){
    return wasm_rt_type_t::WASM_RT_I32;
  }
  else if((PV->getType()->isIntegerType() || PV->getType()->isEnumeralType()))
  {
    return wasm_rt_type_t::WASM_RT_I64;
  }
  else if(PV->getType()->isFloatingType()) /* There should also be future support for 64 bit types*/
  {
    return wasm_rt_type_t::WASM_RT_F64;
  }
  else
  {
    assert("Unknown Callback Type Encountered");
  }
}

wasm_rt_type_t ReturnWasmArgType(QualType QV){
  if(QV.getTypePtr()->isVoidType()){
    return wasm_rt_type_t::WASM_RT_I32;
  }
  else if(QV.getTypePtr()->isTaintedPointerType()){
    return wasm_rt_type_t::WASM_RT_I32;
  }
  else if((QV.getTypePtr()->isIntegerType() || QV.getTypePtr()->isEnumeralType()))
  {
    return wasm_rt_type_t::WASM_RT_I64;
  }
  else if(QV.getTypePtr()->isFloatingType()) /* There should also be future support for 64 bit types*/
  {
    return wasm_rt_type_t::WASM_RT_F64;
  }
  else
  {
    assert("Unknown Callback argument Type Encountered");
  }
}

std::string WasmEnumToString(wasm_rt_type_t wasm_type)
{
  switch(wasm_type){
  case WASM_RT_I32:
    return "unsigned int";
  case WASM_RT_I64:
    return "unsigned long int";
  case WASM_RT_F32:
    return "float";
  case WASM_RT_F64:
    return "double";
  default:
    assert("Unknown Type WASM RT Type Encountered!!");
  }
}
/*
 * Return RetParamObject
 * Purpose:
 * This object would be useful in generating the callback trampoline function
 * output:
 * std::vector<int> RetParamObject --> this object Holds the signature of the
 * callback trampoline
 * ----> First Arg --> Return Type (WASM)
 * ----> Subsequent Args --> params (WASM)
 * input: ASTContext &Context, ProgramInfo &Info,
    Rewriter &R, VarDecl* FD
 */
std::vector<int> ReturnRetParamObject(ASTContext &Context, ProgramInfo &Info,
                                      Rewriter &R, VarDecl* FD)
{
  /*
   * Value types. Used to define function signatures.
  typedef enum {
    WASM_RT_I32,
    WASM_RT_I64,
    WASM_RT_F32,
    WASM_RT_F64,
  } wasm_rt_type_t;
  */

  std::string ArgString;
  std::vector<int> ret_param_entries;
  /*
   * First check the return type
   */
  const clang::PointerType *Pt = FD->getType()->getAs<clang::PointerType>();
  const clang::FunctionProtoType *Ft = nullptr;
  if(FD->getType()->isFunctionPointerType())
    Ft = Pt->getPointeeType()->getAs<clang::FunctionProtoType>();
  else if(FD->getType()->isFunctionType())
    Ft = FD->getType()->getAs<clang::FunctionProtoType>();

  if (Ft == nullptr)
    assert("Function pointer argument to Tainted function not prototyped yet!");
  ret_param_entries.push_back(ReturnWasmArgType(Ft->getReturnType()));
  for(int i = 0 ; i < Ft->getNumParams(); i++)
  {
    ret_param_entries.push_back(ReturnWasmArgType(Ft->getParamType(i)));
  }
  return ret_param_entries;
}
std::string ReturnRetParamType(ASTContext &Context, ProgramInfo &Info,
                               Rewriter &R, VarDecl* FD){

  /*
   * Value types. Used to define function signatures.
  typedef enum {
    WASM_RT_I32,
    WASM_RT_I64,
    WASM_RT_F32,
    WASM_RT_F64,
  } wasm_rt_type_t;
  */

  std::string ArgString;
  std::vector<std::string> RetParamEntries;
  /*
   * First check the return type
   */
  const clang::PointerType *Pt = FD->getType()->getAs<clang::PointerType>();
  const clang::FunctionProtoType *Ft = Pt->getPointeeType()->getAs<clang::FunctionProtoType>();
  RetParamEntries.push_back(itostr(ReturnWasmArgType(Ft->getReturnType())));
  for(int i = 0 ; i < Ft->getNumParams(); i++)
  {
    RetParamEntries.push_back(itostr(ReturnWasmArgType(Ft->getParamType(i))));
  }

  /*
   * Now iterate through all the argument types and generate the array string
   */
  for(auto Arg: RetParamEntries){
    ArgString += Arg;
    ArgString += ",";
  }
  /*
   * Remove the trailing character
   */
  ArgString.pop_back();
  return ArgString;
}

/*
 * Func Name: GenerateCallbackInterceptor
 * Purpose --> Generate the callback interceptor function.
 * This function would be called from the tainted region.
 * WASM-2->C type-conversion would be carried and the call to the actual
 * callback function would be performed with the converted arguments.
 *
 * Return Type --> bool --> Successful conversion
 *
 * Params --> ASTContext &Context, ProgramInfo &Info,
 * Rewriter &R, string callback_func_name, SourceLocation,
 * std::vector<int> func_signature
 *
 * Location: This "trampoline" function would be inserted just before the tainted
 * function definition
 */
bool GenerateCallbackInterceptor(ASTContext &Context, ProgramInfo &Info,
                                 Rewriter &R, std::string CallbackFuncName,
                                 SourceLocation InsertionPt,
                                 VarDecl *FD,
                                 std::vector<int> FuncSignature)
{
  /*
 * Generate an entire string of the function and insert it before the InsertionPt
 *
 */
  const clang::PointerType *Pt = FD->getType()->getAs<clang::PointerType>();
  const clang::FunctionProtoType *FPT = Pt->getPointeeType()->getAs<clang::FunctionProtoType>();
  /*
 * First generate the return type
 */
  std::string ReturnType = "\n" + WasmEnumToString(
                                      static_cast<wasm_rt_type_t>(FuncSignature[0]));

  std::string TrampolineFuncName = CallbackFuncName + "_trampoline";

  std::string Params;

  std::string Body;

  std::string Function;
  //variable declarations (NOTE: must be inserted with ; and \n trailing characters)
  std::vector<std::string> VarDecls;
  /*
   * Iterate through the func_signature vector and start generating the
   * params block
   *
   */
  for(int I = 1; I < FuncSignature.size(); I++){
    if(I == 1)
      Params = Params + " (";
    Params = Params + WasmEnumToString(
                          static_cast<wasm_rt_type_t>(FuncSignature[I]));
    // Insert the name of the arg
    std::string ArgName = "arg_"+ itostr(I);
    Params = Params + " " + ArgName;
    // Insert comma
    if(I < FuncSignature.size() - 1)
      Params = Params + ",\n";
    else
      Params = Params + ") _Unchecked";
  }

  /*
   * Now generate the function body --->
   */
  Body += "\n{\n";

  /*
   * Check if function returns a pointer
   */
  std::string CallContent;
//  if(FPT->getReturnType()->isTaintedPointerType()){
//    CallContent =  CallContent + "\treturn " +
//                  "c_fetch_pointer_offset ((void*)\n" + CallbackFuncName;
//  }
//  else
    CallContent = CallContent +
                  "\treturn (int)" +
                  CallbackFuncName;

  /*
   * Now pass the converted arguments as parameters
   */
  CallContent += " (";
  for(int i = 0 ; i < FPT->getNumParams(); i ++)
  {
    auto CallbackFuncParam = FPT->getParamType(i);
//    if(CallbackFuncParam->isTaintedPointerType())
//    {
//      /*
//       * You have to enter some sort of a reverse cast operation in here
//       */
//      CallContent =  CallContent +
//                    "(" + tyToStr(CallbackFuncParam.getTypePtr(), "") + ") "
//                    + "c_fetch_pointer_from_offset(" + "arg_" + itostr(i+1) + ")";
//    }
//    else
    {
      CallContent = CallContent +
                    "(" + tyToStr(CallbackFuncParam.getTypePtr(), "")
                    + ") " + "arg_" + itostr(i+1);
    }
    if( i < FPT->getNumParams()-1)
      CallContent += ",\n";
  }

  CallContent += ");";

  Body = Body + CallContent + "\n}\n";
  /*
   * Now generate the final function string
   */
  Function += "_TLIB ";
  Function += ReturnType;
  Function = Function + " " + TrampolineFuncName;
  Function += Params;
  for(auto Variables : VarDecls){
    Function += Variables;
  }
  Function += Body;
  Function += "\n";

  /*
   * Now the std::string function is all ready to be written into the file
   * This trampoline function must be inserted before the actual tainted function
   */
  R.InsertTextBefore(InsertionPt, Function);
  return true;
}

bool GenerateW2CDef(ASTContext &Context, ProgramInfo &Info,
                    Rewriter &R, std::string CallbackFuncName,
                    VarDecl *FD,
                    std::vector<int> FuncSignature)
{
  /*
 * Generate an entire string of the function and insert it before the InsertionPt
 *
 */


  /*
 * First generate the return type
 */
  std::string ReturnType = "\n" + WasmEnumToString(
                                      static_cast<wasm_rt_type_t>(FuncSignature[0]));

  std::string FuncParams;

  std::string Function;
  //variable declarations (NOTE: must be inserted with ; and \n trailing characters)

  /*
   * Iterate through the func_signature vector and start generating the
   * params block
   *
   */
  FuncParams = FuncParams + " (void* sandbox ";
  for(int I = 1; I < FuncSignature.size(); I++){

    if (I == 1)
      FuncParams += ", \n";
    FuncParams = FuncParams + WasmEnumToString(
                                  static_cast<wasm_rt_type_t>(FuncSignature[I]));
    // Insert the name of the arg
    std::string ArgName = "arg_"+ itostr(I);
    FuncParams = FuncParams + " " + ArgName;
    // Insert comma
    if( I != FuncSignature.size()-1)
      FuncParams =  FuncParams + ", " + "\n";
  }
  FuncParams = FuncParams + ") ";

  /*
   * Now generate the final function string
   */
  Function += ReturnType;
  Function = Function + " " + CallbackFuncName;
  Function += FuncParams;


  /*
   * Now the std::string function is all ready to be written into the file
   * This trampoline function must be inserted before the actual tainted function
   */
  CopyW2CDefToW2CFile(Context, Info, R, FD, Function);
  return true;
}

bool WasmSandboxRewriteOp(ASTContext &Context, ProgramInfo &Info,
                          Rewriter &R)
{
  RewriteBuffer RB;
  /*
   * First Iterate through the Macros and insert them
   *
   */

  for(auto TaintedFileWithPath : Info.TaintedOutfiles)
  {
    // first fetch the filename from the path
    std::string TaintedFileName = getFileNameFromPath(
        TaintedFileWithPath.c_str());
    if(TaintedFileName.find_last_of('h') != TaintedFileName.npos)
    {
      /*
         * This is a autogenerated header file
         */
      std::string TaintedHeaderFileName = TaintedFileName;
      std::string TaintedCFileNameWithPath = TaintedFileWithPath;
      TaintedCFileNameWithPath.replace(
          TaintedCFileNameWithPath.size()-1, 1, "c");
      std::string TaintedPath = TaintedFileWithPath;
      fetch_path_from_file_path(TaintedPath,
                                TaintedHeaderFileName);
      /*
         * Now fetch the begin location of this file
         */
      RewriteBuffer RB;
      std::string IncludeHeaderFileWithPath = TaintedHeaderFileName;
      std::string IncludeStmt = "#include \"" +
                                IncludeHeaderFileWithPath + "\"\n";
      std::error_code ErrorCode;
      llvm::raw_fd_ostream OutFile(TaintedCFileNameWithPath,
                                   ErrorCode, llvm::sys::fs::OF_Append);
      OutFile.seek(0);
      RB.Initialize(IncludeStmt);
      RB.write(OutFile);
    }
  }

  for (auto MacroToBeInserted : Info.MapTaintedFile2IncludeStmt)
  {
    initializeTaintedFileForDeclIfNeeded(Context,Info, R,
                                         nullptr,
                                         nullptr,
                                         nullptr,
                                         nullptr,
                                         &RB,
                                         MacroToBeInserted.second,
                                         CD_Macro);
    copyMacroDefToTaintedFile(Context, Info, R, MacroToBeInserted.second,
                              MacroToBeInserted.first,
                              &RB);
  }
  /*
   * iterate through the list of Tainted Structs and copy them
   */


  for (auto TaintedStructDecls : Info.TaintMirroredVarStructDecls)
  {

    initializeTaintedFileForDeclIfNeeded(Context,Info, R,
                                         nullptr,
                                         TaintedStructDecls.first,
                                         nullptr,
                                         nullptr,
                                         &RB,
                                         "",
                                         CD_VarStructDecl);
    copyTaintedStructDefToTaintedFile(Context, Info, R, TaintedStructDecls.first,
                                      &RB);
  }

  for (auto TaintedEnumDecls : Info.TaintMirroredEnumDecls)
  {
    initializeTaintedFileForDeclIfNeeded(Context,Info, R,
                                         nullptr,
                                         nullptr,
                                         nullptr,
                                         TaintedEnumDecls.first,&RB,
                                         "",
                                         CD_Enum);
    copyTaintedEnumToTaintedFile(Context, Info, R, TaintedEnumDecls.first,
                                 &RB);
  }


  for (auto TaintedVarDecls : Info.TaintMirroredVarDecls)
  {

    initializeTaintedFileForDeclIfNeeded(Context,Info, R,
                                         TaintedVarDecls.first,
                                         nullptr,
                                         nullptr,
                                         nullptr,
                                         &RB,
                                         "",
                                         CD_GenVarDecl);
    copyTaintedVarDefToTaintedFile(Context, Info, R,
                                   TaintedVarDecls.first,
                                   &RB);
  }

  for (auto TaintedTypeDefDecls : Info.TaintMirroredTypedefDecls)
  {
    initializeTaintedFileForDeclIfNeeded(Context,Info, R,
                                         nullptr,
                                         nullptr,
                                         TaintedTypeDefDecls.first,
                                         nullptr,
                                         &RB,
                                         "",
                                         CD_VarTypeDefDecl);
    copyTaintedTypeDefToTaintedFile(Context, Info, R, TaintedTypeDefDecls.first,
                                    &RB);
  }

  for (auto CallbackFunctionDecls : Info.CallbackFuncStreamWriter)
  {

    initializeTaintedFileForDeclIfNeeded(Context,Info, R,
                                         CallbackFunctionDecls.first,
                                         nullptr,
                                         nullptr,
                                         nullptr,
                                         &RB,
                                         "",
                                         CD_CallbackFuncDecl);

    copyCallbackDefToTaintedFile(Context, Info, R, CallbackFunctionDecls.first, &RB);
  }
  for(auto *TaintedFunctionDecls : Info.TaintedDecls){

    /*
     *The Tainted function definition must move to a different file in order to
     * be compilable to wasm-readable definitions
     * Hence before writing anything, copy the entire original function to a different file
    */

    initializeTaintedFileForDeclIfNeeded(Context,Info, R,
                                         TaintedFunctionDecls,
                                         nullptr,
                                         nullptr,
                                         nullptr,
                                         &RB,
                                         "",
                                         CD_FuncDecl);

    copyTaintedDefToTaintedFile(Context, Info, R, TaintedFunctionDecls, &RB);
    if((!TaintedFunctionDecls->getAsFunction()->isThisDeclarationADefinition()) ||
        (!TaintedFunctionDecls->hasBody()))
    {
      /* If this Tainted Decl is a Function Prototype, we are done here
       *
       */
      continue;
    }

    /*
     * if the Tainted_function_decl is _Mirror annotated, then we are done here
     * We need not insert any type of instrumentation
     */

    if(TaintedFunctionDecls->getAsFunction()->isMirror())
      continue;

    auto FunctionBodyStartLoc = TaintedFunctionDecls->getBody()->getBeginLoc();
    auto FunctionBodyEndLoc = TaintedFunctionDecls->getBody()->getEndLoc();
    rewriteSourceRange(R,
                       SourceRange(
                           FunctionBodyStartLoc.getLocWithOffset(1),
                           FunctionBodyEndLoc.getLocWithOffset(1)),"\n}");
    /*
     * The below code is to comment the function body
     *
     */

    //R.InsertTextAfterToken(tainted_function_decls->getBody()->getBeginLoc()
    //                          , "\n/*");
    //R.InsertText(tainted_function_decls->getBody()->getEndLoc(), "\n*/",
    //             false, true);
    std::map<VarDecl*, std::string> MapOfParams;
    for(int I = 0 ; I < TaintedFunctionDecls->getAsFunction()->getNumParams(); I++)
    {
      MapOfParams.insert(std::pair<VarDecl*, std::string>(TaintedFunctionDecls->getAsFunction()->getParamDecl(I),
                                                           TaintedFunctionDecls->getAsFunction()->getNameAsString()));
    }
    //fetch the function name -->
    std::string FunctionName = TaintedFunctionDecls->getAsFunction()->getNameAsString();
    //for wasm -->
    // append this with the w2c_ extension
    std::string WasmFunctionName = "w2c_"+FunctionName;
    //now generate the return type -->
    std::string ReturnArg;
    std::vector<std::string> VarDecls ;
    auto RetType = TaintedFunctionDecls->getAsFunction()->getReturnType();

    if(RetType->isTaintedPointerType()){
      //ReturnArg = "c_fetch_pointer_from_offset(";
        ReturnArg = "(";
    }

    //now generate the parameter list in form of a string -->
    std::string FinalParamString;
    std::vector<std::string> SetOfParams;
    /*
     * The first argument to the tainted function call will always be a
     * fetch_sandbox_address()
     */
    SetOfParams.push_back("c_fetch_sandbox_address()");

    for(auto Params: MapOfParams)
    {
      std::string SbxInstrumentedParam;
      if(Params.first->getType()->isTaintedPointerType())
      {
        if(Params.first->getType()->getPointeeType()->isFunctionType()){
          /*
         * There a more than a couple of steps involved here
         * 1.)  We need to create a callback trampoline function
         * 2.) Need to register this callback trampoline function with the WASM sbx
         *
         */
          /*
         * CALLBACK TRAMPOLINE FUNCTION -->
         * 1.) has an extra argument called the sandbox (void*) which is
         * basically is uint32
         * 2.) performed uint32 to pointer type conversions and inserts a call
         * to the actual function
         * 3.) performs return type conversions too
         * 4.) the void* is totally ignored.
         */
          /*
         * This argument is a function pointer callback
         * Fetch the callback_trampoline index for this function name and
         * pass it as an argument
         */
          auto FuncSignature = ReturnRetParamObject(Context,Info,R,
                                                    Params.first);
          if(!GenerateCallbackInterceptor(Context, Info,
                                           R, Params.first->getNameAsString(),
                                           TaintedFunctionDecls->getBeginLoc(),
                                           Params.first,
                                           FuncSignature)){
            assert("Callback Interceptor Generation Failed!!");
          }

          const clang::PointerType *Pt = Params.first->getType()->getAs<clang::PointerType>();
          const clang::FunctionProtoType *FPT = Pt->getPointeeType()->getAs<clang::FunctionProtoType>();

          VarDecls.push_back("\nint ret_param_types[] = {"+
                             ReturnRetParamType(Context,Info,R,
                                                Params.first->getInitializingDeclaration())+"};\n");

          int RetParam = 0;
          if(!FPT->getReturnType()->isVoidType())
            RetParam = 1;

          SbxInstrumentedParam = "sbx_register_callback("+
                                 Params.first->getNameAsString()
                                 + "_trampoline"
                                 +" , "
                                 + itostr(FPT->getNumParams())
                                 + " , "
                                 + itostr(RetParam)
                                 + " , "
                                 + "ret_param_types)";
        }
        else {
//          SbxInstrumentedParam =
//              "c_fetch_pointer_offset((void*)" + Params.first->getNameAsString() + ")";
                SbxInstrumentedParam = "(int)" + Params.first->getNameAsString();
        }
      }
      else if(isTaintedStruct(Params.first))
      {

      }
      else{
        SbxInstrumentedParam = Params.first->getNameAsString();
      }
      SetOfParams.push_back(SbxInstrumentedParam);
    }

    //now keep iterating and entering commas
    for(int I = 0; I < SetOfParams.size(); I ++)
    {
      FinalParamString = FinalParamString + " " + SetOfParams[I];
      if(I < (SetOfParams.size()-1)){
        FinalParamString = FinalParamString + ",\n";
      }
    }

    //by here you must have a proper final_param_string -->

    // Now append it to required items and form the final call -->
    std::string FinalBoardingCall;
    for (auto InitDecl : VarDecls)
    {
      FinalBoardingCall += InitDecl;
    }
    /*
     * If the Tainted function returns a Tainted pointer, there must be an extra
     * rparen, else single rparen
     */
    /*
     * Perform Return Cast
     *
     */
    std::string TypeStr;
    std::string RetCastStr = tyToStr(RetType.getTypePtr(), TypeStr);
    if (!RetType->isTaintedPointerType()) {
      FinalBoardingCall += "\n\n return (" + RetCastStr + ")" + ReturnArg +
                           WasmFunctionName + "(" +
                           FinalParamString + ");\n";
    }
    else {

      FinalBoardingCall += "\n\n return (" + RetCastStr + ")" + ReturnArg +
                           WasmFunctionName + "(" + FinalParamString + "));\n";
    }
    R.InsertTextAfter(TaintedFunctionDecls->getBody()->getEndLoc(), FinalBoardingCall);
    /*
     * Generate the prototype for the w2c function name and store it somewhere
     */
    auto FuncSignature = ReturnRetParamObject(Context,Info,R,
                                              static_cast<VarDecl*>(TaintedFunctionDecls));
    GenerateW2CDef(Context, Info, R, WasmFunctionName,
                   static_cast<VarDecl*>(TaintedFunctionDecls), FuncSignature);
  }
  for (auto InstrumentedFileWithPath : Info.ListOfInstrumentedFiles)
  {
    /*
       * Generate the w2c Header file name from the tainted file name
       * Instrumented File Name --> FileName.c
       * Tainted definitions for the above file --> tainted_FileName.c
       * corresponding w2c header file --> w2c_FileName.c
       */
    std::string InstrumentedFile = getFileNameFromPath(
        InstrumentedFileWithPath.first.c_str());
    auto FilePath = InstrumentedFileWithPath.first;
    fetch_path_from_file_path(FilePath, InstrumentedFile);
    eraseSubStr(InstrumentedFile,
                "Tainted");
    auto W2CFileName = "WASM2C" + InstrumentedFile;
    W2CFileName[W2CFileName.size()-1] = 'h';

    auto W2CFileNameWithPath = FilePath + W2CFileName;

    auto InsertLocation = InstrumentedFileWithPath.second->
                          getASTContext().getSourceManager()
                              .getLocForStartOfFile(
                                  InstrumentedFileWithPath.second->
                                  getASTContext().getSourceManager().getFileID(
                                          InstrumentedFileWithPath.second->getBeginLoc()
                                              ));
    std::string IncludeStmt = "#include \"" +
                              W2CFileNameWithPath + "\"\n";
    IncludeStmt += "#include <checkcbox_extensions.h>\n";
    R.InsertTextAfter(InsertLocation, IncludeStmt);
  }
}

std::string getcwdString( void ) {
  char Buff[PATH_MAX];
  getcwd( Buff, PATH_MAX );
  std::string cwd( Buff );
  return cwd;
}

bool SandboxSpecificRewriteOp(ASTContext &Context, ProgramInfo &Info,
                              Rewriter &R) {
  if (_CheckMateOpts.AddSandbox == "wasm") {
    /*
     * Before we start writing, We shall clean the Tainted Directory
     * by removing all files
     *
     */
    std::string TaintedDir = _CheckMateOpts.TaintedDefDir;
    std::string currPath = getcwdString();
    chdir(TaintedDir.c_str());
    std::string Command ="";
    Command = "rm -rf *";
    std::system(Command.c_str());
    chdir(currPath.c_str());
    return WasmSandboxRewriteOp(Context, Info, R);
  }
}
// This function is the public entry point for function body rewriting.
void DeclRewriter::rewriteBody(ASTContext &Context, ProgramInfo &Info,
                               Rewriter &R) {
  SandboxSpecificRewriteOp(Context, Info, R);
}

void DeclRewriter::rewrite(RSet &ToRewrite) {
  for (auto Pair : ToRewrite) {
    DeclReplacement *N = Pair.second;
    assert(N->getDecl() != nullptr);

    errs() << "Replacing type of decl:\n";
    N->getDecl()->dump();
    errs() << "with " << N->getReplacement() << "\n";


    // Exact rewriting procedure depends on declaration type
    if (auto *MR = dyn_cast<MultiDeclMemberReplacement>(N)) {
      //      MultiDeclInfo *MDI =
      //          Info.TheMultiDeclsInfo.findContainingMultiDecl(MR->getDecl());
      //      assert("Missing MultiDeclInfo for multi-decl member" && MDI);
      //      // A multi-decl can only be rewritten as a unit. If at least one member
      //      // needs rewriting, then the first MultiDeclMemberReplacement in iteration
      //      // order of ToRewrite (which need not have anything to do with member
      //      // order of the multi-decl) triggers rewriting of the entire multi-decl,
      //      // and rewriteMultiDecl checks ToRewrite for a MultiDeclMemberReplacement
      //      // for each member of the multi-decl and applies it if found.
      //      if (!MDI->AlreadyRewritten)
      //        //rewriteMultiDecl(*MDI, ToRewrite);
      //
    } else if (auto *FR = dyn_cast<FunctionDeclReplacement>(N)) {
      //rewriteFunctionDecl(FR);
    } else {
      assert(false && "Unknown replacement type");
    }
  }
}
//TagDecl Represents the declaration of a class/union/struct/TStruct
void DeclRewriter::denestTagDecls() {
  // When there are multiple levels of nested TagDecls, we need to process
  // all the children of a TagDecl TD before TD itself so that (1) the
  // definitions of the children end up before the definition of TD (since the
  // rewriter preserves order of insertions) and (2) the definitions of the
  // children have been removed from the body of TD before we read the body of
  // TD to move it. In effect, we want to process the TagDecls in postorder.
  // The easiest way to achieve this is to process them in order of their _end_
  // locations.
  std::sort(TagDeclsToDenest.begin(), TagDeclsToDenest.end(),
            [&](TagDecl *TD1, TagDecl *TD2) {
              return A.getSourceManager().isBeforeInTranslationUnit(
                  TD1->getEndLoc(), TD2->getEndLoc());
            });
  for (TagDecl *TD : TagDeclsToDenest) {
    // rewriteMultiDecl replaced the final "}" in the original source range with
    // "};\n", so the new content of the source range should include the ";\n",
    // which is what we want here. Except the rewriter has a bug where it
    // adjusts the token range to include the final token _after_ mapping the
    // offset to account for previous edits (it should be before). We work
    // around the bug by adjusting the token range before calling the rewriter
    // at all.
    CharSourceRange CSR = Lexer::makeFileCharRange(
        CharSourceRange::getTokenRange(TD->getSourceRange()), R.getSourceMgr(),
        R.getLangOpts());
    std::string DefinitionStr = R.getRewrittenText(CSR);
    // Delete the definition from the old location.
    rewriteSourceRange(R, CSR, "");
    // We want to find the highest ancestor DeclContext of TD that is a TagDecl
    // (call it TopTagDecl) and insert TD just before TopTagDecl.
    //
    // As of this writing, it seems that if TD is named, `TD->getDeclContext()`
    // returns the parent of TopTagDecl due to the code at
    // https://github.com/correctcomputation/checkedc-clang/blob/fd4d8af4383d40af10ee8bc92b7bf88061a11035/clang/lib/Sema/SemaDecl.cpp#L16980-L16981,
    // But that code doesn't run if TD is unnamed (which makes some sense
    // because name visibility isn't an issue for TagDecls that have no name),
    // and we want to de-nest TagDecls with names we assigned just like ones
    // that were originally named, so we can't just use `TD->getDeclContext()`.
    // In any event, maybe we wouldn't want to rely on this kind of internal
    // Clang behavior.
    TagDecl *TopTagDecl = TD;
    while (TagDecl *ParentTagDecl =
               dyn_cast<TagDecl>(TopTagDecl->getLexicalDeclContext()))
      TopTagDecl = ParentTagDecl;
    // If TopTagDecl is preceded by qualifiers, ideally we'd like to insert TD
    // before those qualifiers. If TopTagDecl is actually part of a multi-decl
    // with at least one member, then we can just use the begin location of that
    // multi-decl as the insertion point.
    //
    // If there are no members (so the qualifiers have no effect and generate a
    // compiler warning), then there isn't an easy way for us to find the source
    // location before the qualifiers. In that case, we just insert TD at the
    // begin location of TopTagDecl (after the qualifiers) and hope for the
    // best. In the cases we're aware of so far (storage qualifiers, including
    // `typedef`), this just means that the qualifiers end up applying to the
    // first TagDecl de-nested from TopTagDecl instead of to TopTagDecl itself,
    // and they generate the same compiler warning as before but on a different
    // TagDecl. However, we haven't confirmed that there aren't any obscure
    // cases that could lead to an error, such as if a qualifier is valid on one
    // kind of TagDecl but not another.
    SourceLocation InsertLoc;
    if (MultiDeclInfo *MDI =
            Info.TheMultiDeclsInfo.findContainingMultiDecl(TopTagDecl))
      InsertLoc = MDI->Members[0]->getBeginLoc();
    else
      InsertLoc = TopTagDecl->getBeginLoc();
    // TODO: Use a wrapper like rewriteSourceRange that tries harder with
    // macros, reports failure, etc.
    // (https://github.com/correctcomputation/checkedc-clang/issues/739)
    R.InsertText(InsertLoc, DefinitionStr);
  }
}

void DeclRewriter::emitSupplementaryDeclarations(
    const std::vector<std::string> &SDecls, SourceLocation Loc) {
  // There are no supplementary declarations to emit. The AllDecls String
  // will remain empty, so insertText should no-op, but it's still an error to
  // insert an empty string at an invalid source location, so short circuit here
  // to be safe.
  if (SDecls.empty())
    return;

  std::string AllDecls;
  for (std::string D : SDecls)
    AllDecls += "\n" + D;

  R.InsertText(getLocationAfterToken(Loc, R.getSourceMgr(), R.getLangOpts()),
               AllDecls);
}
