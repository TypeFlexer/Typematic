set(LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  Option
  Support
  )

if (MSVC)
  set_source_files_properties(ConstraintBuilder.cpp PROPERTIES COMPILE_FLAGS /bigobj)
  set_source_files_properties(DeclRewriter.cpp PROPERTIES COMPILE_FLAGS /bigobj)
  set_source_files_properties(RewriteUtils.cpp PROPERTIES COMPILE_FLAGS /bigobj)
endif()

add_clang_library(clangCheckMate
  CheckMate.cpp
  CheckMateStats.cpp
  ConstraintBuilder.cpp
  ConstraintResolver.cpp
  DeclRewriter.cpp
  MappingVisitor.cpp
  MultiDecls.cpp
  PersistentSourceLoc.cpp
  ProgramInfo.cpp
  ProgramVar.cpp
  RewriteUtils.cpp
  PlantC4.cpp
  StructInit.cpp
  Utils.cpp
  ${five_c_source}
  LINK_LIBS
  clangAST
  clangAnalysis
  clangBasic
  clangDriver
  clangFrontend
  clangRewriteFrontend
  clangStaticAnalyzerFrontend
  clangTooling
  clangTransformer
)
