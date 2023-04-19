# This script adds the checkcbox C tests to the LLVM tests infrastructure,
# provided that the checkcbox C repo exists as a subdirectory.

# LLVM uses the CMake meta-build system to generate the build system.
# The build system includes targets for running tests.
#
# The script adds a new target for running checkcbox C tests.
# To do that, it sets up  the information needed by the LIT tool.

if(CHECKCBOX_IN_TREE)
  add_subdirectory(checkcbox/include)

  set(CHECKCBOX_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
  set(CHECKCBOX_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})


  if (CMAKE_CFG_INTDIR STREQUAL ".")
    set(LLVM_BUILD_MODE ".")
  else ()
    set(LLVM_BUILD_MODE "%(build_mode)s")
  endif ()

  string(REPLACE ${CMAKE_CFG_INTDIR} ${LLVM_BUILD_MODE} CLANG_TOOLS_DIR ${LLVM_RUNTIME_OUTPUT_INTDIR})

  configure_lit_site_cfg(
    ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg.py
    MAIN_CONFIG
    ${CMAKE_CURRENT_SOURCE_DIR}/lit.cfg.py
    )

  list(APPEND CHECKCBOX_TEST_DEPS
    clang clang-resource-headers checkcbox-headers
    )

  if(CLANG_ENABLE_STATIC_ANALYZER)
    list(APPEND CLANG_TEST_DEPS
      clang-check
      )
  endif()

  set(CHECKCBOX_TEST_PARAMS
    checkcbox_site_config=${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg.py
    )

if( NOT CLANG_BUILT_STANDALONE )
  list(APPEND CHECKCBOX_TEST_DEPS
    llvm-config
    FileCheck count not
    opt
    )
endif()

  add_custom_target(checkcbox-test-depends DEPENDS ${CHECKCBOX_TEST_DEPS})

  add_lit_testsuite(check-checkcbox "Running the checkcbox C regression tests"
    ${CMAKE_CURRENT_BINARY_DIR}
    #LIT ${LLVM_LIT}
    PARAMS ${CHECKCBOX_TEST_PARAMS}
    DEPENDS ${CHECKCBOX_TEST_DEPS}
    ARGS ${CHECKCBOX_TEST_EXTRA_ARGS}
    )

  set_target_properties(check-checkcbox PROPERTIES FOLDER "checkcbox C tests")

# Add a lit target to run the checkcbox tests for ARM.
if (CHECKCBOX_ARM_SYSROOT AND CHECKCBOX_ARM_RUNUNDER)
  set(CHECKCBOX_TEST_EXTRA_ARGS "-DCHECKCBOX_TARGET=ARM")

  add_lit_testsuite(check-checkcbox-arm "Running the CheckCbox regression tests for ARM"
    ${CMAKE_CURRENT_BINARY_DIR}
    #LIT ${LLVM_LIT}
    PARAMS ${CHECKCBOX_TEST_PARAMS}
    DEPENDS ${CHECKCBOX_TEST_DEPS}
    ARGS ${CHECKCBOX_TEST_EXTRA_ARGS}
    )

  set_target_properties(check-checkcbox-arm PROPERTIES FOLDER "checkcbox C tests for ARM")
endif()

endif()
