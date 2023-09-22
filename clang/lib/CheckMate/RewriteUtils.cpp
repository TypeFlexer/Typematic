//=--RewriteUtils.cpp---------------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// This class contains implementation of the functions and
// classes of RewriteUtils.h
//===----------------------------------------------------------------------===//

#include "clang/CheckMate/RewriteUtils.h"
#include "clang/CheckMate/CheckMateGlobalOptions.h"
#include "clang/CheckMate/DeclRewriter.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Tooling/Transformer/SourceCode.h"

using namespace llvm;
using namespace clang;

//RewrittenBody mkStringForPVDecl(MultiDeclMemberDecl *MMD, PVConstraint *PVC,
//                                ProgramInfo &Info) {
//  // Currently, it's cheap to keep recreating the ArrayBoundsRewriter. If that
//  // ceases to be true, we should pass it along as another argument.
////  ArrayBoundsRewriter ABRewriter{Info};
//
//  RewrittenDecl RD;
//  bool IsExternGlobalVar =
//      isa<VarDecl>(MMD) &&
//      cast<VarDecl>(MMD)->getFormalLinkage() == Linkage::ExternalLinkage;
//  if (_CheckMateOpts.ItypesForExtern && (isa<FieldDecl>(MMD) || IsExternGlobalVar) &&
//      // isSolutionChecked can return false here when splitting out an unchanged
//      // multi-decl member.
//      PVC->isSolutionChecked(Info.getConstraints().getVariables())) {
//    // Give record fields and global variables itypes when using
//    // -itypes-for-extern. Note that we haven't properly implemented itypes for
//    // structures and globals
//    // (https://github.com/correctcomputation/checkedc-clang/issues/744). This
//    // just rewrites to an itype instead of a fully checked type when a checked
//    // type could have been used. This does provide most of the rewriting
//    // infrastructure that would be required to support these itypes if
//    // constraint generation is updated to handle structure/global itypes.
//    RD = DeclRewriter::buildItypeDecl(PVC, cast<DeclaratorDecl>(MMD),
//                                      PVC->getName(), Info, ABRewriter, true,
//                                      true);
//  } else {
//    RD = DeclRewriter::buildCheckedDecl(PVC, cast<DeclaratorDecl>(MMD),
//                                        PVC->getName(), Info, ABRewriter,
//                                        true);
//  }
//  RD.Type = getStorageQualifierString(MMD) + RD.Type;
//
//  return RD;
//}

std::string mkStringForDeclWithUnchangedType(MultiDeclMemberDecl *MMD,
                                             ProgramInfo &Info) {
  ASTContext &Context = MMD->getASTContext();

  bool BaseTypeRenamed = Info.TheMultiDeclsInfo.wasBaseTypeRenamed(MMD);
  if (!BaseTypeRenamed) {
    // As far as we know, we can let Clang generate the declaration string.
    PrintingPolicy Policy = Context.getPrintingPolicy();
    Policy.SuppressInitializers = true;
    std::string DeclStr = "";
    raw_string_ostream DeclStream(DeclStr);
    MMD->print(DeclStream, Policy);
    assert("Original decl string empty." && !DeclStr.empty());
    return DeclStr;
  }

  // OK, we have to use mkString.
  QualType DType = getTypeOfMultiDeclMember(MMD);

  // If the type is not a pointer or array, then it should just equal the base
  // type except for top-level qualifiers, and it can't have itypes or bounds.
  llvm::Optional<std::string> BaseTypeNewNameOpt =
      Info.TheMultiDeclsInfo.getTypeStrOverride(DType.getTypePtr(), Context);
  assert(BaseTypeNewNameOpt &&
         "BaseTypeRenamed is true but we couldn't get the new name");
  std::string QualifierPrefix = DType.getQualifiers().getAsString();
  if (!QualifierPrefix.empty())
    QualifierPrefix += " ";
  return getStorageQualifierString(MMD) + QualifierPrefix +
         *BaseTypeNewNameOpt + " " + std::string(MMD->getName());
}

// Test to see if we can rewrite a given SourceRange.
// Note that R.getRangeSize will return -1 if SR is within
// a macro as well. This means that we can't re-write any
// text that occurs within a macro.
bool canRewrite(Rewriter &R, const CharSourceRange &SR) {
  return SR.isValid() && (R.getRangeSize(SR) != -1);
}

void rewriteSourceRange(Rewriter &R, const SourceRange &Range,
                        const std::string &NewText, bool ErrFail) {
  rewriteSourceRange(R, CharSourceRange::getTokenRange(Range), NewText,
                     ErrFail);
}

// Wrapper for Rewriter::ReplaceText(CharSourceRange, StringRef) that works
// around a bug that occurs when text has previously been inserted at the start
// location of the specified range.
//
// When Rewriter::ReplaceText(CharSourceRange, StringRef) computes the range of
// the rewrite buffer to replace, it sets the start location to be after the
// previous insertion (RewriteBuffer::ReplaceText calls getMappedOffset with
// AfterInserts = true) but sets the length to include the length of the
// previous insertion (it calls getRangeSize with the default RewriteOptions
// with IncludeInsertsAtBeginOfRange = true). This causes the range to extend
// beyond the intended end location by an amount equal to the length of the
// previous insertion. We avoid the problem by calling getRangeSize ourselves
// with IncludeInsertsAtBeginOfRange = false.
//
// TODO: File an upstream Clang bug report if appropriate. As of this writing
// (2021-11-24), we found some discussion of the problem
// (https://reviews.llvm.org/D107503) but not a real entry in the bug tracker.
static bool replaceTextWorkaround(Rewriter &R, const CharSourceRange &Range,
                                  const std::string &NewText) {
  Rewriter::RewriteOptions Opts;
  Opts.IncludeInsertsAtBeginOfRange = false;
  return R.ReplaceText(Range.getBegin(), R.getRangeSize(Range, Opts), NewText);
}

void rewriteSourceRange(Rewriter &R, const CharSourceRange &Range,
                        const std::string &NewText, bool ErrFail) {
  // Attempt to rewrite the source range. First use the source range directly
  // from the parameter.
  bool RewriteSuccess = false;
  if (canRewrite(R, Range))
    RewriteSuccess = !replaceTextWorkaround(R, Range, NewText);

  // If initial rewriting attempt failed (either because canRewrite returned
  // false or because ReplaceText failed (returning true), try rewriting again
  // with the source range expanded to be outside any macros used in the range.
  if (!RewriteSuccess) {
    CharSourceRange Expand = clang::Lexer::makeFileCharRange(
        Range, R.getSourceMgr(), R.getLangOpts());
    if (canRewrite(R, Expand))
      RewriteSuccess = !replaceTextWorkaround(R, Expand, NewText);
  }

  // Emit an error if we were unable to rewrite the source range. This is more
  // likely to be a bug in CheckMate than an issue with the input, but emitting a
  // diagnostic here with the intended rewriting is much more useful than
  // crashing with an assert fail.
  if (!RewriteSuccess) {
    clang::DiagnosticsEngine &DE = R.getSourceMgr().getDiagnostics();
    bool ReportError = ErrFail && !_CheckMateOpts.AllowRewriteFailures;
    reportCustomDiagnostic(
        DE,
        ReportError ? DiagnosticsEngine::Error : DiagnosticsEngine::Warning,
        "Unable to rewrite converted source range. Intended rewriting: "
        "\"%0\"",
        Range.getBegin())
        << R.getSourceMgr().getExpansionRange(Range) << NewText;
    if (ReportError) {
      reportCustomDiagnostic(
          DE, DiagnosticsEngine::Note,
          "you can use the -allow-rewrite-failures option to temporarily "
          "downgrade this error to a warning",
          // If we pass the location here, the macro call stack gets dumped
          // again, which looks silly.
          SourceLocation());
    }
  }
}

static void emit(Rewriter &R, ASTContext &C, bool &StdoutModeEmittedMainFile) {
  if (_CheckMateOpts.Verbose)
    errs() << "Writing files out\n";

  bool StdoutMode = (_CheckMateOpts.OutputPostfix == "-" && _CheckMateOpts.OutputDir.empty());
  SourceManager &SM = C.getSourceManager();
  // Iterate over each modified rewrite buffer.
  for (auto Buffer = R.buffer_begin(); Buffer != R.buffer_end(); ++Buffer) {
    if (const FileEntry *FE = SM.getFileEntryForID(Buffer->first)) {
      assert(FE->isValid());
      // Used for diagnostics related to the file.
      SourceLocation BeginningOfFileSourceLoc =
          SM.translateFileLineCol(FE, 1, 1);

      DiagnosticsEngine &DE = C.getDiagnostics();
      DiagnosticsEngine::Level UnwritableChangeDiagnosticLevel =
        _CheckMateOpts.AllowUnwritableChanges ? DiagnosticsEngine::Warning
                                       : DiagnosticsEngine::Error;
      auto PrintExtraUnwritableChangeInfo = [&]() {
        // With -dump-unwritable-changes and not -allow-unwritable-changes, we
        // want the -allow-unwritable-changes note before the dump.
        if (!_CheckMateOpts.DumpUnwritableChanges) {
          reportCustomDiagnostic(
              DE, DiagnosticsEngine::Note,
              "use the -dump-unwritable-changes option to see the new version "
              "of the file",
              SourceLocation());
        }
        if (!_CheckMateOpts.AllowUnwritableChanges) {
          reportCustomDiagnostic(
              DE, DiagnosticsEngine::Note,
              "you can use the -allow-unwritable-changes option to temporarily "
              "downgrade this error to a warning",
              SourceLocation());
        }
        if (_CheckMateOpts.DumpUnwritableChanges) {
          errs() << "=== Beginning of new version of " << FE->getName()
                 << " ===\n";
          Buffer->second.write(errs());
          errs() << "=== End of new version of " << FE->getName() << " ===\n";
        }
      };

      // Check whether we are allowed to write this file.
      //
      // In our testing as of 2021-03-15, the file path returned by
      // FE->tryGetRealPathName() was canonical, but be safe against unusual
      // situations or possible future changes to Clang before we actually write
      // a file. We can't use FE->getName() because it seems it may be relative
      // to the `directory` field of the compilation database, which (now that
      // we no longer use `ClangTool::run`) is not guaranteed to match CheckMate's
      // working directory.
      std::string ToConv = FE->tryGetRealPathName().str();
      std::string FeAbsS = "";
      std::error_code EC = tryGetCanonicalFilePath(ToConv, FeAbsS);
      if (EC) {
        reportCustomDiagnostic(
            DE, UnwritableChangeDiagnosticLevel,
            "CheckMate internal error: not writing the new version of this file due "
            "to failure to re-canonicalize the file path provided by Clang",
            BeginningOfFileSourceLoc);
        reportCustomDiagnostic(
            DE, DiagnosticsEngine::Note,
            "file path from Clang was %0; error was: %1",
            SourceLocation())
            << ToConv << EC.message();
        PrintExtraUnwritableChangeInfo();
        continue;
      }
      if (FeAbsS != ToConv) {
        reportCustomDiagnostic(
            DE, UnwritableChangeDiagnosticLevel,
            "CheckMate internal error: not writing the new version of this file "
            "because the file path provided by Clang was not canonical",
            BeginningOfFileSourceLoc);
        reportCustomDiagnostic(
            DE, DiagnosticsEngine::Note,
            "file path from Clang was %0; re-canonicalized file path is %1",
            SourceLocation())
            << ToConv << FeAbsS;
        PrintExtraUnwritableChangeInfo();
        continue;
      }
      if (!canWrite(FeAbsS)) {
        reportCustomDiagnostic(DE, UnwritableChangeDiagnosticLevel,
                               "CheckMate internal error: CheckMate generated changes to "
                               "this file even though it is not allowed to "
                               "write to the file "
                               "(https://github.com/correctcomputation/"
                               "checkedc-clang/issues/387)",
                               BeginningOfFileSourceLoc);
        PrintExtraUnwritableChangeInfo();
        continue;
      }

      if (StdoutMode) {
        if (Buffer->first == SM.getMainFileID()) {
          // This is the new version of the main file. Print it to stdout,
          // except in the edge case where we have a compilation database with
          // multiple translation units with the same main file and we already
          // emitted a copy of the main file for a previous translation unit
          // (https://github.com/correctcomputation/checkedc-clang/issues/374#issuecomment-893612654).
          if (!StdoutModeEmittedMainFile) {
            Buffer->second.write(outs());
            StdoutModeEmittedMainFile = true;
          }
        } else {
          reportCustomDiagnostic(
              DE, UnwritableChangeDiagnosticLevel,
              "CheckMate generated changes to this file, which is under the base dir "
              "but is not the main file and thus cannot be written in stdout "
              "mode",
              BeginningOfFileSourceLoc);
          PrintExtraUnwritableChangeInfo();
        }
        continue;
      }

      // Produce a path/file name for the rewritten source file.
      std::string NFile;
      // We now know that we are using either OutputPostfix or OutputDir mode
      // because stdout mode is handled above. OutputPostfix defaults to "-"
      // when it's not provided, so any other value means that we should use
      // OutputPostfix. Otherwise, we must be in OutputDir mode.
      if (_CheckMateOpts.OutputPostfix != "-") {
        // That path should be the same as the old one, with a
        // suffix added between the file name and the extension.
        // For example \foo\bar\a.c should become \foo\bar\a.checked.c
        // if the OutputPostfix parameter is "checked" .
        std::string PfName = sys::path::filename(FE->getName()).str();
        std::string DirName = sys::path::parent_path(FE->getName()).str();
        std::string FileName = sys::path::remove_leading_dotslash(PfName).str();
        std::string Ext = sys::path::extension(FileName).str();
        std::string Stem = sys::path::stem(FileName).str();
        if(_CheckMateOpts.OutputPostfix == "rw")
          NFile = Stem + Ext;
        else
          NFile = Stem + "." + _CheckMateOpts.OutputPostfix + Ext;
        if (!DirName.empty())
          NFile = DirName + sys::path::get_separator().str() + NFile;
      } else {
        assert(!_CheckMateOpts.OutputDir.empty());
        // If this does not hold when OutputDir is set, it should have been a
        // fatal error in the _CheckMateInterface constructor.
        assert(filePathStartsWith(FeAbsS, _CheckMateOpts.BaseDir));
        // replace_path_prefix is not smart about separators, but this should be
        // OK because tryGetCanonicalFilePath should ensure that neither BaseDir
        // nor OutputDir has a trailing separator.
        SmallString<255> Tmp(FeAbsS);
        llvm::sys::path::replace_path_prefix(Tmp, _CheckMateOpts.BaseDir,
                                             _CheckMateOpts.OutputDir);
        NFile = std::string(Tmp.str());
        EC = llvm::sys::fs::create_directories(sys::path::parent_path(NFile));
        if (EC) {
          reportCustomDiagnostic(
              DE, DiagnosticsEngine::Error,
              "failed to create parent directory of output file \"%0\"",
              BeginningOfFileSourceLoc)
              << NFile;
          continue;
        }
      }

      raw_fd_ostream Out(NFile, EC, sys::fs::F_None);

      if (!EC) {
        if (_CheckMateOpts.Verbose)
          errs() << "writing out " << NFile << "\n";
        Buffer->second.write(Out);
      } else {
        reportCustomDiagnostic(DE, DiagnosticsEngine::Error,
                               "failed to write output file \"%0\"",
                               BeginningOfFileSourceLoc)
            << NFile;
        // This is awkward. What to do? Since we're iterating, we could have
        // created other files successfully. Do we go back and erase them? Is
        // that surprising? For now, let's just keep going.
      }
    }
  }

  if (StdoutMode && !StdoutModeEmittedMainFile) {
    // The main file is unchanged. Write out its original content.
    outs() << SM.getBufferOrFake(SM.getMainFileID()).getBuffer();
    StdoutModeEmittedMainFile = true;
  }
}

// Rewrites types that inside other expressions. This includes cast expression
// and compound literal expressions.
class TypeExprRewriter : public clang::RecursiveASTVisitor<TypeExprRewriter> {
public:
  explicit TypeExprRewriter(ASTContext *C, ProgramInfo &I, Rewriter &R)
      : Context(C), Info(I), Writer(R) {}

  bool VisitCompoundLiteralExpr(CompoundLiteralExpr *CLE) {
    SourceRange TypeSrcRange(
        CLE->getBeginLoc().getLocWithOffset(1),
        CLE->getTypeSourceInfo()->getTypeLoc().getEndLoc());
    rewriteType(CLE, TypeSrcRange);
    return true;
  }

  bool VisitCStyleCastExpr(CStyleCastExpr *ECE) {
    SourceRange TypeSrcRange(
        ECE->getBeginLoc().getLocWithOffset(1),
        ECE->getTypeInfoAsWritten()->getTypeLoc().getEndLoc());
    rewriteType(ECE, TypeSrcRange);
    return true;
  }

private:
  ASTContext *Context;
  ProgramInfo &Info;
  Rewriter &Writer;

  void rewriteType(Expr *E, SourceRange &Range) {
  }
};

// Adds type parameters to calls to alloc functions.
// The basic assumption this makes is that an alloc function will be surrounded
// by a cast expression giving its type when used as a type other than void*.
SourceRange BodyReplacement::getSourceRange(SourceManager &SM) const {
  return getDeclSourceRangeWithAnnotations(getDecl(),
                                           /*IncludeInitializer=*/false);
}

SourceRange FunctionBodyReplacement::getSourceRange(SourceManager &SM) const {
  SourceLocation Begin = RewriteGeneric ? getDeclBegin(SM) :
                      (RewriteReturn ? getReturnBegin(SM) : getParamBegin(SM));
  SourceLocation End = RewriteParams ? getDeclEnd(SM) : getReturnEnd(SM);
  // Begin can be equal to End if the SourceRange only contains one token.
  assert("Invalid FunctionBodyReplacement SourceRange!" &&
         (Begin == End || SM.isBeforeInTranslationUnit(Begin, End)));
  return SourceRange(Begin, End);
}

SourceLocation FunctionBodyReplacement::getDeclBegin(SourceManager &SM) const {
  SourceLocation Begin = Decl->getBeginLoc();
  return Begin;
}

SourceLocation FunctionBodyReplacement::getReturnBegin(SourceManager &SM) const {
  // TODO: more accuracy
  // This code gets the point after a modifier like "static"
  // But currently, that leads to multiple "static"s
  //  SourceRange ReturnSource = Decl->getReturnTypeSourceRange();
  //  if (ReturnSource.isValid())
  //    return ReturnSource.getBegin();

  // Invalid return means we're in a macro or typedef, so just get the
  // starting point. We may overwrite a _For_any(..), but those only
  // exist in partially converted code, so we're relying on the user
  // to have it correct anyway.

  return getDeclBegin(SM);
}

SourceLocation FunctionBodyReplacement::getParamBegin(SourceManager &SM) const {
  FunctionTypeLoc FTypeLoc = getFunctionTypeLoc(Decl);
  // If we can't get a FunctionTypeLoc instance, then we'll guess that the
  // l-paren is the token following the function name. This can clobber some
  // comments and formatting.
  if (FTypeLoc.isNull())
    return Lexer::getLocForEndOfToken(Decl->getLocation(), 0, SM,
                                      Decl->getLangOpts());
  return FTypeLoc.getLParenLoc();
}

SourceLocation FunctionBodyReplacement::getReturnEnd(SourceManager &SM) const {
  return Decl->getReturnTypeSourceRange().getEnd();
}

SourceLocation FunctionBodyReplacement::getDeclEnd(SourceManager &SM) const {
  SourceLocation End;
  if (isKAndRFunctionDecl(Decl)) {
    // For K&R style function declaration, use the beginning of the function
    // body as the end of the declaration. K&R declarations must have a body.
    End = locationPrecedingChar(Decl->getBody()->getBeginLoc(), SM, ';');
  } else {
    FunctionTypeLoc FTypeLoc = getFunctionTypeLoc(Decl);
    if (FTypeLoc.isNull()) {
      // Without a FunctionTypeLocation, we have to approximate the end of the
      // declaration as the location of the first r-paren before the start of
      // the function body. This is messed up by comments and ifdef blocks
      // containing r-paren, but works correctly most of the time.
      End = getFunctionDeclRParen(Decl, SM);
    } else if (Decl->getReturnType()->isFunctionPointerType()) {
      // If a function returns a function pointer type, the parameter list for
      // the returned function type comes after the top-level functions
      // parameter list. Of course, this FunctionTypeLoc can also be null, so we
      // have another fall back to the r-paren approximation.
      FunctionTypeLoc T = getFunctionTypeLoc(FTypeLoc.getReturnLoc());
      if (!T.isNull())
        End = T.getRParenLoc();
      else
        End = getFunctionDeclRParen(Decl, SM);
    } else {
      End = FTypeLoc.getRParenLoc();
    }
  }

  // If there's a bounds or interop type expression, this will come after the
  // right paren of the function declaration parameter list.
  SourceLocation AnnotationsEnd = getCheckedCAnnotationsEnd(Decl);
  if (AnnotationsEnd.isValid() &&
      (!End.isValid() || SM.isBeforeInTranslationUnit(End, AnnotationsEnd)))
    End = AnnotationsEnd;

  // SourceLocations are weird and turn up invalid for reasons I don't
  // understand. Fallback to extracting r paren location from source
  // character buffer.
  if (!End.isValid())
    End = getFunctionDeclRParen(Decl, SM);

  return End;
}

//std::string ArrayBoundsRewriter::getBoundsString(const PVConstraint *PV,
//                                                 Decl *D, bool Isitype,
//                                                 bool OmitLowerBound) {
//  auto &ABInfo = Info.getABoundsInfo();
//
//  // Try to find a bounds key for the constraint variable. If we can't,
//  // ValidBKey is set to false, indicating that DK has not been initialized.
//  BoundsKey DK;
//  bool ValidBKey = true;
//  if (PV->hasBoundsKey())
//    DK = PV->getBoundsKey();
//  else if (!ABInfo.tryGetVariable(D, DK))
//    ValidBKey = false;
//
//  std::string BString = "";
//  // For itype we do not want to add a second ":".
//  std::string Pfix = Isitype ? " " : " : ";
//
//  if (ValidBKey && !PV->hasSomeSizedArr()) {
//    ABounds *ArrB = ABInfo.getBounds(DK);
//    // If we have pointer arithmetic and cannot add range bounds, then do not
//    // emit any bounds string.
//    if (ArrB != nullptr && ABInfo.hasLowerBound(DK)) {
//      if (OmitLowerBound)
//        BString = ArrB->mkStringWithoutLowerBound(&ABInfo, D);
//      else
//        BString = ArrB->mkString(&ABInfo, D, DK);
//      if (!BString.empty())
//        BString = Pfix + BString;
//    }
//  }
//  if (BString.empty() && PV->srcHasBounds()) {
//    BString = Pfix + PV->getBoundsStr();
//  }
//  return BString;
//}

//bool ArrayBoundsRewriter::hasNewBoundsString(const PVConstraint *PV, Decl *D,
//                                             bool Isitype) {
//  std::string BStr = getBoundsString(PV, D, Isitype);
//  // There is a bounds string but has nothing declared?
//  return !BStr.empty() && !PV->srcHasBounds();
//}

std::set<PersistentSourceLoc> RewriteConsumer::EmittedDiagnostics;
void RewriteConsumer::emitRootCauseDiagnostics(ASTContext &Context) {
  clang::DiagnosticsEngine &DE = Context.getDiagnostics();
  unsigned ID = DE.getCustomDiagID(
      DiagnosticsEngine::Warning, "Root cause for %0 unchecked pointer%s0: %1");
  SourceManager &SM = Context.getSourceManager();


}

void RewriteConsumer::HandleTranslationUnit(ASTContext &Context) {
  Info.enterCompilationUnit(Context);

  if (_CheckMateOpts.WarnRootCause)
    emitRootCauseDiagnostics(Context);

  // Rewrite Variable declarations
  Rewriter R(Context.getSourceManager(), Context.getLangOpts());
  DeclRewriter::rewriteBody(Context, Info, R);

  // Take care of some other rewriting tasks
  std::set<llvm::FoldingSetNodeID> Seen;
  TranslationUnitDecl *TUD = Context.getTranslationUnitDecl();
  for (const auto &D : TUD->decls()) {

  }

  // Output files.
  emit(R, Context, StdoutModeEmittedMainFile);
  Info.exitCompilationUnit();

  return;
}
