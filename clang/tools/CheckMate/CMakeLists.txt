set(LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  Option
  Support
  )

add_clang_tool(CheckMate
  CheckMateStandalone.cpp
  )

target_link_libraries(CheckMate
  PRIVATE
  clangCheckMate
  clangAST
  clangBasic
  clangDriver
  clangFrontend
  clangRewriteFrontend
  clangStaticAnalyzerFrontend
  clangTooling
  )

install(TARGETS CheckMate
  RUNTIME DESTINATION bin)
