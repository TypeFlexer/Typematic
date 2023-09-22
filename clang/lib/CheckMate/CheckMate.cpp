//=--CheckMate.cpp-------------------------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Implementation of various method in CheckMate.h
//
//===----------------------------------------------------------------------===//

#include "clang/CheckMate/CheckMate.h"
#include "clang/CheckMate/CheckMateGlobalOptions.h"
#include "clang/CheckMate/ConstraintBuilder.h"
#include "clang/CheckMate/RewriteUtils.h"
#include "clang/CheckMate/PlantC4.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/VerifyDiagnosticConsumer.h"
#include "clang/Tooling/ArgumentsAdjusters.h"
#include "llvm/Support/TargetSelect.h"

using namespace clang::driver;
using namespace clang::tooling;
using namespace clang;
using namespace llvm;

// Suffixes for constraint output files.ParameterGatherer
#define INITIAL_OUTPUT_SUFFIX "_initial_constraints"
#define FINAL_OUTPUT_SUFFIX "_final_output"
#define BEFORE_SOLVING_SUFFIX "_before_solving_"
#define AFTER_SUBTYPING_SUFFIX "_after_subtyping_"

std::set<std::string> FilePaths;

struct _CheckMateOptions _CheckMateOpts;

static CompilationDatabase *CurrCompDB = nullptr;
static tooling::CommandLineArguments SourceFiles;

class _CheckMateDiagnosticConsumer : public DiagnosticConsumer {
  std::unique_ptr<DiagnosticConsumer> UnderlyingConsumer;

  // In general, a DiagnosticConsumer can be used for more than one source file,
  // but we use _CheckMateDiagnosticConsumer for only one file, so we can use this
  // simple state machine.
  enum State { S_Startup, S_ASTParsing, S_CheckMateAnalysis, S_Done };
  State CurrentState = S_Startup;

public:
  _CheckMateDiagnosticConsumer(DiagnosticsEngine &Engine)
      : UnderlyingConsumer(Engine.takeClient()) {
    // This code currently only supports the default LibTooling setup in which
    // the ClangTool has no global DiagnosticConsumer, so
    // CompilerInstance::createDiagnostics creates one owned by the
    // DiagnosticsEngine. If we needed to support the case in which the
    // DiagnosticConsumer isn't owned, we could use an "UnderlyingConsumerOwner"
    // pattern like VerifyDiagnosticConsumer does.
    assert(UnderlyingConsumer);
  }

  void BeginSourceFile(const LangOptions &LangOpts,
                       const Preprocessor *PP) override {
    assert(CurrentState == S_Startup);
    CurrentState = S_ASTParsing;
    UnderlyingConsumer->BeginSourceFile(LangOpts, PP);
  }

  void EndSourceFile() override {
    assert(CurrentState == S_ASTParsing);
    CurrentState = S_CheckMateAnalysis;
    // Delay forwarding the EndSourceFile callback.
  }

  void HandleDiagnostic(DiagnosticsEngine::Level DiagLevel,
                        const Diagnostic &Info) override {
    // Count the originally generated diagnostics.
    DiagnosticConsumer::HandleDiagnostic(DiagLevel, Info);
    UnderlyingConsumer->HandleDiagnostic(DiagLevel, Info);
  }

  // Returns true if this translation unit was successful after diagnostic
  // verification, i.e., verification is enabled and it succeeded, or
  // verification is disabled and there were no errors. The caller must check
  // _CheckMateInterface::HadNonDiagnosticError separately.
  bool finishCheckMateAnalysis() {
    assert(CurrentState == S_CheckMateAnalysis);
    CurrentState = S_Done;
    UnderlyingConsumer->EndSourceFile();
    return UnderlyingConsumer->getNumErrors() == 0;
  }

  ~_CheckMateDiagnosticConsumer() override {
    // We considered asserting that the state is S_Done here, but if
    // ASTUnit::LoadFromCompilerInvocation fails and returns null, the
    // _CheckMateDiagnosticConsumer may be destructed without reaching S_Done. However,
    // even without such an assertion, we're still well protected against
    // forgetting to finish verification and missing an error because if the
    // ASTUnit is null, _CheckMateInterface::parseASTs will record a "non-diagnostic
    // error", while if the ASTUnit is non-null, it will get added to
    // _CheckMateInterface::ASTs and determineExitCode will call finishCheckMateAnalysis. (And
    // the _CheckMateInterface destructor enforces that determineExitCode is called.)
  }
};

// Based on LibTooling's ASTBuilderAction but does several custom things that we
// need.
//
// See clang/docs/checkedc/CheckMate/clang-tidy.md#_CheckMate-name-prefix
// NOLINTNEXTLINE(readability-identifier-naming)
class _CheckMateASTBuilderAction : public ToolAction {
  std::vector<std::unique_ptr<ASTUnit>> &ASTs;

public:
  _CheckMateASTBuilderAction(std::vector<std::unique_ptr<ASTUnit>> &ASTs)
      : ASTs(ASTs) {}

  bool runInvocation(std::shared_ptr<CompilerInvocation> Invocation,
                     FileManager *Files,
                     std::shared_ptr<PCHContainerOperations> PCHContainerOps,
                     DiagnosticConsumer *DiagConsumer) override {

    // Adjust some compiler options. This is similar to what we could do with
    // a LibTooling ArgumentsAdjuster, but we access the options in their parsed
    // data structure rather than as strings, so it is much more robust.

    if (!_CheckMateOpts.EnableCCTypeChecker)
      // Corresponds to the -fCheckMate-tool compiler option.
      Invocation->LangOpts->_CheckMate = true;

    // Re-canonicalize the path of the main source file, in case it was
    // overridden by the compilation database after it was originally
    // canonicalized by the _CheckMateInterface constructor. This completely fixes
    // https://github.com/correctcomputation/checkedc-clang/issues/515 and
    // https://github.com/correctcomputation/checkedc-clang/issues/604 for the
    // main source file.
    //
    // If https://github.com/correctcomputation/checkedc-clang/issues/604 were
    // fixed another way, then making the path absolute would be sufficient to
    // fix https://github.com/correctcomputation/checkedc-clang/issues/515; the
    // path wouldn't need to be canonical.
    SmallVectorImpl<FrontendInputFile> &Inputs =
        Invocation->getFrontendOpts().Inputs;
    for (FrontendInputFile *Iter = Inputs.begin(); Iter < Inputs.end();
         Iter++) {
      FrontendInputFile &OldInput = *Iter;
      // getFile will assert that the input is a file, which should be true for
      // CheckMate.
      std::string OldPath = std::string(OldInput.getFile()), NewPath;
      std::error_code EC = tryGetCanonicalFilePath(OldPath, NewPath);
      if (EC) {
        // If the compilation database specifies a bogus, inaccessible file,
        // that will normally be caught by Driver::DiagnoseInputExistence before
        // we get here.
        errs() << "CheckMate error: Failed to re-canonicalize source file path "
               << OldPath << " during compiler invocation: " << EC.message()
               << "\n";
        return false;
      }
      *Iter =
          FrontendInputFile(NewPath, OldInput.getKind(), OldInput.isSystem());
    }

    // Canonicalize -I paths to address the same two issues for `#include`d
    // files. The situation is analogous except that this is not a complete fix
    // for https://github.com/correctcomputation/checkedc-clang/issues/604
    // because the portion of the path in the `#include` directive may be
    // non-canonical.
    HeaderSearchOptions &HeaderOpts = *Invocation->HeaderSearchOpts;
    std::vector<HeaderSearchOptions::Entry> NewUserEntries;
    for (auto It = HeaderOpts.UserEntries.begin();
         It != HeaderOpts.UserEntries.end();) {
      HeaderSearchOptions::Entry &Entry = *It;
      std::string OldPath = Entry.Path, NewPath;
      std::error_code EC = tryGetCanonicalFilePath(OldPath, NewPath);
      if (EC) {
        // Normally, if an -I directory isn't accessible, Clang seems to ignore
        // it with no diagnostic. Hopefully, removing it from the list here will
        // have the same effect and not break anything. If we kept the
        // non-canonical entry, we might benefit from any diagnostic that Clang
        // might issue in the future, but it's harder to reason about whether
        // that might re-expose the original bugs.
        It = HeaderOpts.UserEntries.erase(It);
      } else {
        Entry.Path = NewPath;
        It++;
      }
    }

    // Set up diagnostics.
    IntrusiveRefCntPtr<DiagnosticsEngine> DiagEngine =
        CompilerInstance::createDiagnostics(&Invocation->getDiagnosticOpts(),
                                            DiagConsumer,
                                            /*ShouldOwnClient=*/false);
    // The _CheckMateDiagnosticConsumer takes ownership of and wraps the engine's
    // previous DiagnosticConsumer, i.e., the one created by
    // CompilerInstance::createDiagnostics above (which will be a
    // VerifyDiagnosticConsumer if requested via the options).
    DiagEngine->setClient(new _CheckMateDiagnosticConsumer(*DiagEngine));

    // Finally, actually build the AST. This part is the same as in
    // ASTBuilderAction::runInvocation.

    std::unique_ptr<ASTUnit> AST = ASTUnit::LoadFromCompilerInvocation(
        Invocation, std::move(PCHContainerOps), DiagEngine, Files);
    if (!AST)
      return false;

    handleExtraProgramAction(Invocation->getFrontendOpts(),
                             AST->getASTContext());

    ASTs.push_back(std::move(AST));
    return true;
  }

private:
  void handleExtraProgramAction(FrontendOptions &Opts,
                                ASTContext &C) {
    // The Opts.ProgramAction field is normally used only by `clang -cc1` to
    // select a FrontendAction (see CreateFrontendBaseAction in
    // ExecuteCompilerInvocation.cpp) and is ignored by LibTooling tools, which
    // perform a custom FrontendAction. But we want to support at least AST
    // dumping (as an addition to CheckMate's normal workflow) since it's useful for
    // debugging CheckMate, and we prefer to honor the standard `-Xclang -ast-dump`
    // option rather than define our own tool-level option like clang-check
    // does. We could add support for more `-ast-*` options here if desired.
    switch (Opts.ProgramAction) {
    case frontend::ParseSyntaxOnly:
      // Nothing extra to do.
      break;
    case frontend::ASTDump: {
      // Code copied from ASTDumpAction::CreateASTConsumer since we don't have a
      // good way to actually use ASTDumpAction from here. :/
      //
      // XXX: Maybe we'd prefer to output this somewhere other than stdout to
      // separate it from the updated main file written to stdout? This doesn't
      // look trivial because ASTPrinter requires ownership of the output
      // stream, and it probably isn't important for the intended debugging use
      // case.
      std::unique_ptr<ASTConsumer> Dumper =
          CreateASTDumper(nullptr /*Dump to stdout.*/, Opts.ASTDumpFilter,
                          Opts.ASTDumpDecls, Opts.ASTDumpAll,
                          Opts.ASTDumpLookups, Opts.ASTDumpDeclTypes,
                          Opts.ASTDumpFormat);
      // In principle, we should call all the ASTConsumer methods the same way
      // the normal AST parsing process would, but there isn't an obvious way to
      // do that when using ASTUnit. Instead, we rely on the assumption
      // (apparently valid as of this writing) that the only ASTConsumer method
      // that has a nonempty implementation in ASTPrinter is
      // HandleTranslationUnit, and we just call HandleTranslationUnit manually.
      Dumper->HandleTranslationUnit(C);
      break;
    }
    default:
      llvm::errs() << "Warning: The requested ProgramAction is not implemented "
                      "by CheckMate and will be ignored.\n";
      break;
    }
  }
};


std::unique_ptr<_CheckMateInterface>
_CheckMateInterface::create(const struct _CheckMateOptions &CCopt,
                     const std::vector<std::string> &SourceFileList,
                     CompilationDatabase *CompDB) {
  // See clang/docs/checkedc/CheckMate/clang-tidy.md#_CheckMate-name-prefix
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::unique_ptr<_CheckMateInterface> _CheckMateInter(
      new _CheckMateInterface(CCopt, SourceFileList, CompDB));
  if (_CheckMateInter->ConstructionFailed) {
    return nullptr;
  }
  return _CheckMateInter;
}

//This is where all the drama begins
_CheckMateInterface::_CheckMateInterface(const struct _CheckMateOptions &CCopt,
                           const std::vector<std::string> &SourceFileList,
                           CompilationDatabase *CompDB) {

  _CheckMateOpts = CCopt;
  _CheckMateOpts.WarnRootCause |= _CheckMateOpts.WarnAllRootCause;

  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmPrinters();
  llvm::InitializeAllAsmParsers();

  ConstraintsBuilt = false;

  if (_CheckMateOpts.OutputPostfix != "-" && !_CheckMateOpts.OutputDir.empty()) {
    errs() << "CheckMate initialization error: Cannot use both -output-postfix and "
              "-output-dir\n";
    ConstructionFailed = true;
    return;
  }
  if (_CheckMateOpts.OutputPostfix == "-" && _CheckMateOpts.OutputDir.empty() &&
      SourceFileList.size() > 1) {
    errs() << "CheckMate initialization error: Cannot specify more than one input "
              "file when output is to stdout\n";
    ConstructionFailed = true;
    return;
  }

  std::string TmpPath;
  std::error_code EC;

  if (_CheckMateOpts.BaseDir.empty()) {
    _CheckMateOpts.BaseDir = ".";
  }

  // Get the canonical path of the base directory.
  TmpPath = _CheckMateOpts.BaseDir;
  EC = tryGetCanonicalFilePath(_CheckMateOpts.BaseDir, TmpPath);
  if (EC) {
    errs() << "CheckMate initialization error: Failed to canonicalize base directory "
           << "\"" << _CheckMateOpts.BaseDir << "\": " << EC.message() << "\n";
    ConstructionFailed = true;
    return;
  }
  _CheckMateOpts.BaseDir = TmpPath;

  if (!_CheckMateOpts.OutputDir.empty()) {
    // tryGetCanonicalFilePath will fail if the output dir doesn't exist yet, so
    // create it first.
    EC = llvm::sys::fs::create_directories(_CheckMateOpts.OutputDir);
    if (EC) {
      errs() << "CheckMate initialization error: Failed to create output directory \""
             << _CheckMateOpts.OutputDir << "\": " << EC.message() << "\n";
      ConstructionFailed = true;
      return;
    }
    TmpPath = _CheckMateOpts.OutputDir;
    EC = tryGetCanonicalFilePath(_CheckMateOpts.OutputDir, TmpPath);
    if (EC) {
      errs() << "CheckMate initialization error: Failed to canonicalize output "
             << "directory \"" << _CheckMateOpts.OutputDir << "\": " << EC.message()
             << "\n";
      ConstructionFailed = true;
      return;
    }
    _CheckMateOpts.OutputDir = TmpPath;
  }

  SourceFiles = SourceFileList;

  bool SawInputOutsideBaseDir = false;
  for (const auto &S : SourceFiles) {
    std::string AbsPath;
    EC = tryGetCanonicalFilePath(S, AbsPath);
    if (EC) {
      errs() << "CheckMate initialization error: Failed to canonicalize source file "
             << "path \"" << S << "\": " << EC.message() << "\n";
      ConstructionFailed = true;
      continue;
    }
    FilePaths.insert(AbsPath);
    if (!filePathStartsWith(AbsPath, _CheckMateOpts.BaseDir)) {
      errs()
          << "CheckMate initialization "
          << (_CheckMateOpts.OutputDir != "" || !_CheckMateOpts.AllowSourcesOutsideBaseDir
                  ? "error"
                  : "warning")
          << ": File \"" << AbsPath
          << "\" specified on the command line is outside the base directory\n";
      SawInputOutsideBaseDir = true;
    }
  }
  if (SawInputOutsideBaseDir) {
    errs() << "The base directory is currently \"" << _CheckMateOpts.BaseDir
           << "\" and can be changed with the -base-dir option.\n";
    if (_CheckMateOpts.OutputDir != "") {
      ConstructionFailed = true;
      errs() << "When using -output-dir, input files outside the base "
                "directory cannot be handled because there is no way to "
                "compute their output paths.\n";
    } else if (!_CheckMateOpts.AllowSourcesOutsideBaseDir) {
      ConstructionFailed = true;
      errs() << "You can use the -allow-sources-outside-base-dir option to "
                "temporarily downgrade this error to a warning.\n";
    }
  }

  CurrCompDB = CompDB;

}

_CheckMateInterface::~_CheckMateInterface() {
  assert(ConstructionFailed || DeterminedExitCode);
}

bool _CheckMateInterface::isSuccessfulSoFar() {
  if (HadNonDiagnosticError)
    return false;
  for (auto &TU : ASTs)
    if (TU->getDiagnostics().getClient()->getNumErrors() > 0)
      return false;
  return true;
}

int _CheckMateInterface::determineExitCode() {
  assert(!DeterminedExitCode);
  DeterminedExitCode = true;

  bool Success = isSuccessfulSoFar();

  bool SuccessAfterDiagnosticVerification = !HadNonDiagnosticError;
  for (auto &TU : ASTs)
    SuccessAfterDiagnosticVerification &=
        ((_CheckMateDiagnosticConsumer *)TU->getDiagnostics().getClient())
            ->finishCheckMateAnalysis();

  if (!Success && SuccessAfterDiagnosticVerification)
    // In this case, the `CheckMate` tool will typically just have printed a "Failure
    // occurred while X... Exiting" message. If we let that be the last word, it
    // could confuse the user.
    llvm::errs() << "(Note: Exiting successfully because the failure was due "
                    "solely to expected error diagnostics and diagnostic "
                    "verification succeeded.)\n";

  return (SuccessAfterDiagnosticVerification ? 0 : 1);
}

bool _CheckMateInterface::parseASTs() {

  std::lock_guard<std::mutex> Lock(InterfaceMutex);

  auto *Tool = new ClangTool(*CurrCompDB, SourceFiles);

  // load the ASTs
  _CheckMateASTBuilderAction Action(ASTs);
  int ToolExitStatus = Tool->run(&Action);
  HadNonDiagnosticError |= (ToolExitStatus != 0);

  GlobalProgramInfo.registerTranslationUnits(ASTs);

  return isSuccessfulSoFar();
}

bool _CheckMateInterface::addVariables() {

  std::lock_guard<std::mutex> Lock(InterfaceMutex);

  // Find multi-decls and assign names to unnamed inline TagDecls now so that
  // the assigned type names are available when we construct ConstraintVariables
  // for the multi-decl members in the "Add Variables" step below.
  for (auto &TU : ASTs)
    GlobalProgramInfo.TheMultiDeclsInfo.findUsedTagNames(TU->getASTContext());
  if (!isSuccessfulSoFar())
    return false;
  for (auto &TU : ASTs)
    GlobalProgramInfo.TheMultiDeclsInfo.findMultiDecls(TU->getASTContext());
  if (!isSuccessfulSoFar())
    return false;

  // 1. Add Variables.
  VariableAdderConsumer VA = VariableAdderConsumer(GlobalProgramInfo, nullptr);
  for (auto &TU : ASTs)
    VA.HandleTranslationUnit(TU->getASTContext());

  return isSuccessfulSoFar();
}

bool _CheckMateInterface::buildInitialConstraints() {

  std::lock_guard<std::mutex> Lock(InterfaceMutex);

  // 1. Gather constraints.
  ConstraintBuilderConsumer CB =
      ConstraintBuilderConsumer(GlobalProgramInfo, nullptr);
  for (auto &TU : ASTs)
    CB.HandleTranslationUnit(TU->getASTContext());
  if (!isSuccessfulSoFar())
    return false;

  ConstraintsBuilt = true;

  return isSuccessfulSoFar();
}

  bool _CheckMateInterface::writeAllConvertedFilesToDisk() {
  std::lock_guard<std::mutex> Lock(InterfaceMutex);

  // 3. Rewrite the input files.
  RewriteConsumer RC = RewriteConsumer(GlobalProgramInfo);
  for (auto &TU : ASTs)
    RC.HandleTranslationUnit(TU->getASTContext());
  return isSuccessfulSoFar();
}

bool _CheckMateInterface::PlaceC4Charges() {
  std::lock_guard<std::mutex> Lock(InterfaceMutex);
  //Fetch the tainted directory
  PlantC4 C4Planted(GlobalProgramInfo);
  return C4Planted.ConvertTaintedToVanilla();
}


