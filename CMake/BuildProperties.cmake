# BuildProperties.cmake
# Copyright (c) 2024 Saul D Beniquez
# License: MIT
#
# This module defines a function prevent_in_source_build() that prevents in-source builds
# and sets a policy for CMake version 3.24.0 and above.

function(prevent_in_source_build)
	# Prevent in-source builds
	if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
		message(FATAL_ERROR "Source and build directories cannot be the same.")
	endif()
endfunction()

function(disable_deprecated_features)
	# Use new timestamp behavior when extracting files archives
	if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
		cmake_policy(SET CMP0135 NEW)
	endif()
endfunction()

function(git_setup_submodules)
	find_package(Git QUIET)
	if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
		option(GIT_SUBMODULE "Check submodules during build" ON)
		if(GIT_SUBMODULE)
			message(STATUS "Git submodule update")
			execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
				WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
				RESULT_VARIABLE GIT_SUBMOD_RESULT)
			if(NOT GIT_SUBMOD_RESULT EQUAL "0")
				message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
			endif()
		endif()
	endif()
endfunction()

function(set_artifact_dir path)
    # Set local variable, not necessary to be parent scope since it's not used outside this function
    set(ARTIFACT_DIR "${path}")

    # Set project-specific artifact directory in parent scope
    set(${PROJECT_NAME}_ARTIFACT_DIR "${path}" PARENT_SCOPE)

    # Set output directories in parent scope using the provided path directly
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${path}/lib" PARENT_SCOPE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${path}/lib" PARENT_SCOPE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${path}/bin" PARENT_SCOPE)
endfunction()

function(disable_tests_if_subproject)
	if (NOT DEFINED PROJECT_NAME)
		option(BUILD_TESTING "Build and run unit tests" ON)
	else()
		option(BUILD_TESTING "Build and run unit tests" OFF)
	endif()
endfunction()

function(use_ccache)
option(USE_CCACHE
	[=[Use ccache compiler cache to speed up builds.
Enabled by default if ccache is found]=]
ON
)
# Search for the code caching compiler wrapper, ccache and enable it
# if found. This will speed up repeated builds.
if (USE_CCACHE)
	message(CHECK_START "Detecting cacche")

	find_program(CCACHE_PATH ccache)
	if(CCACHE_PATH)
			message(CHECK_PASS("found"))
			set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_PATH})
			set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE_PATH})
	endif()

	list(APPEND CMAKE_MESSAGE_INDENT " ")
		message(STATUS "(set -DUSE_CCACHE=Off to disable)")
	list(POP_BACK CMAKE_MESSAGE_INDENT)
endif()

endfunction()

function(detect_linkers)
	option(USE_MOLD "Use the mold/sold parallel linker for faster builds" OFF)
	if(USE_MOLD)
  		# Determine if the compiler is GCC or Clang
  		if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    		message(STATUS "Detected GCC/Clang, checking for mold/sold linker...")

    		# Check for mold linker on general systems and ld64.mold on macOS
    		if(APPLE)
      			find_program(MOLD_LINKER ld64.mold)
      			set(CMAKE_LINKER_TYPE SOLD)
  			else()
    			find_program(MOLD_LINKER mold)
      			set(CMAKE_LINKER_TYPE MOLD)
    		endif()

    		if(MOLD_LINKER)
      			message(STATUS "LINKER_TYPE set to ${CMAKE_LINKER_TYPE} for faster builds")
				list(APPEND CMAKE_MESSAGE_INDENT " ")
				message(STATUS "(set -DUSE_MOLD=OFF to disable)")
				list(POP_BACK CMAKE_MESSAGE_INDENT)
    		else()
      			message(STATUS "  -- No suitable mold linker found. Using default linker.")
    		endif()
  		else()
    		message(STATUS "Compiler is neither GCC nor Clang. Skipping mold linker check.")
  		endif()
	endif()
endfunction()

# vim: ts=4 sts=4 sw=4 noet foldmethod=indent :
