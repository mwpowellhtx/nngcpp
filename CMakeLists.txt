#
#   Copyright (c) 2012 Martin Sustrik  All rights reserved.
#   Copyright (c) 2013 GoPivotal, Inc.  All rights reserved.
#   Copyright (c) 2015-2016 Jack R. Dunaway. All rights reserved.
#   Copyright 2016 Franklin "Snaipe" Mathieu <franklinmathieu@gmail.com>
#   Copyright 2017 Garrett D'Amore <garrett@damore.org>
#   Copyright (c) 2017 Michael W. Powell <mwpowellhtx@gmail.com> All rights reserved.
#
#   Permission is hereby granted, free of charge, to any person obtaining a copy
#   of this software and associated documentation files (the "Software"),
#   to deal in the Software without restriction, including without limitation
#   the rights to use, copy, modify, merge, publish, distribute, sublicense,
#   and/or sell copies of the Software, and to permit persons to whom
#   the Software is furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included
#   in all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
#   IN THE SOFTWARE.
#
cmake_minimum_required (VERSION 3.1)

project (cppnng CXX)

option (CMAKE_CONFIGURATION_TYPES "" Debug)
option (CMAKE_INSTALL_PREFIX "" install)

find_package (Git)

if (GIT_FOUND)
    message (STATUS "Git found: ${GIT_EXECUTABLE} ${GIT_VERSION_STRING}")
else ()
    message (FATAL_ERROR "Git required: not found.")
endif()

# Should be running this from the top-most repository directory. This
# should also go without saying, but may not be as obvious to all.

set (GIT_SUBMODULE_REPOS_DIR "${cppnng_SOURCE_DIR}/repos")
set (GIT_SUBMODULE_NNG_REPO_DIR "${GIT_SUBMODULE_REPOS_DIR}/nng")

set (CPPNNG_NNG_BUILD_DIR ${cppnng_SOURCE_DIR}/build/nng)

macro (cppnng_update_nng_git_submodule SUBMODULE_DIR)

    set (REPO_DIR "${SUBMODULE_DIR}")

    message (STATUS "Updating Git submodules ('${cppnng_SOURCE_DIR}') ...")

    # This would not evaluate correctly in the if (<expression/>), for whatever
    # reason. However, here I can capture the variable and test for it.

    message (STATUS "Checking for existing repository '${REPO_DIR}' ...")

    if (EXISTS "${REPO_DIR}/CMakeLists.txt")

        message (STATUS "Repository '${REPO_DIR}/CMakeLists.txt' already exists.")

    else ()

        message (STATUS "Running '\"${GIT_EXECUTABLE}\" submodule update --init --recursive' ...")

        execute_process (
            COMMAND "${GIT_EXECUTABLE}" submodule update --init --recursive
            WORKING_DIRECTORY "${cppnng_SOURCE_DIR}"
            RESULT_VARIABLE GIT_EC
            OUTPUT_VARIABLE GIT_OUTPUT
        )

        if (NOT GIT_EC EQUAL 0)
            message (STATUS "Failed updating Git submodules (${cppnng_SOURCE_DIR}).")
            message (FATAL_ERROR "${GIT_OUTPUT} (${GIT_EC})")
        endif ()

    endif ()

endmacro () # cppnng_update_nng_git_submodule

macro (cppnng_checkout_nng_git_revision REVISION SUBMODULE_DIR)

    set (REVISION_PROVIDED_NOT_PROVIDED REVISION STREQUAL "")
    set (GIT_CHECKOUT_REVISION $"{REVISION}")

    # Check out a specific REVISION when required.
    if (REVISION_PROVIDED_NOT_PROVIDED)
        set (GIT_CHECKOUT_REVISION "HEAD")
        message (STATUS "Git Checkout revision set to '${GIT_CHECKOUT_REVISION}'.")
    endif ()

    # # TODO: TBD: is there a better way, a way (PERIOD), to query for the current branch/revision?
    # message (STATUS "Running ""${GIT_EXECUTABLE}"" branch | sed -n '/\* /s///p' ...")

    # execute_process (
        # COMMAND "\"${GIT_EXECUTABLE}\" branch | sed -n '/\* /s///p'"
        # WORKING_DIRECTORY "${SUBMODULE_DIR}"
        # RESULT_VARIABLE GIT_EC
        # OUTPUT_VARIABLE GIT_OUTPUT
    # )

    # # message (STATUS "Git branch is '${GIT_OUTPUT}'.")

    message(STATUS "Running '\"${GIT_EXECUTABLE}\" checkout -b ${GIT_CHECKOUT_REVISION} origin/${GIT_CHECKOUT_REVISION} --force' ...")

    # The Working Directory is arcane knowledge; we know this by organization of the repository.
    execute_process (
        COMMAND "${GIT_EXECUTABLE}" checkout -b ${GIT_CHECKOUT_REVISION} origin/${GIT_CHECKOUT_REVISION} --force
        WORKING_DIRECTORY "${SUBMODULE_DIR}"
        RESULT_VARIABLE GIT_EC
        OUTPUT_VARIABLE GIT_OUTPUT
    )

    # Expecting EC 0 in a perfect world; EC 128 may be acceptable, especially if already at that REVISION.
    if (NOT (GIT_EC EQUAL 0 OR GIT_EC EQUAL 128))
        message (STATUS "Failed checking out revision '${GIT_CHECKOUT_REVISION}' at '${SUBMODULE_DIR}'.")
        message (FATAL_ERROR "${GIT_OUTPUT} (${GIT_EC})")
    endif ()

endmacro () # cppnng_checkout_nng_git_revision

macro (cppnng_nng_add_cmake_generator GEN)
    set (CPPNNG_NNG_CMAKE_OPTIONS "${CPPNNG_NNG_CMAKE_OPTIONS} -G \"${GEN}\"")
endmacro ()

macro (cppnng_nng_add_cmake_var OPT TYPE VALUE)
    set (CPPNNG_NNG_CMAKE_OPTIONS "${CPPNNG_NNG_CMAKE_OPTIONS} -D ${OPT}:${TYPE}=${VALUE}")
endmacro ()

macro (cppnng_nng_add_custom_opt OPT VALUE)
    set (CPPNNG_NNG_CMAKE_OPTIONS "${CPPNNG_NNG_CMAKE_OPTIONS} ${OPT} ${VALUE}")
endmacro ()

macro (cppnng_configure_nng NNG_SRC_DIR NNG_BUILD_DIR)

    # Clear out the options.
    set (CPPNNG_NNG_CMAKE_OPTIONS "")

    cppnng_nng_add_cmake_generator ("${CMAKE_GENERATOR}")

    # Align the CMake CONFIGURATION types. Whatever we want to build for CPPNNG, we also want to build from NNG.
    # Also, apparently, no need to do any gymnastic delimter replacement, either, it seems.
    cppnng_nng_add_cmake_var ("CMAKE_CONFIGURATION_TYPES" STRING "\"${CMAKE_CONFIGURATION_TYPES}\"")

    cppnng_nng_add_cmake_var ("CMAKE_INSTALL_PREFIX" STRING "\"${NNG_BUILD_DIR}\"")
    cppnng_nng_add_cmake_var ("NNG_ENABLE_COVERAGE" BOOL OFF)
    cppnng_nng_add_cmake_var ("NNG_ENABLE_DOC" BOOL OFF)
    cppnng_nng_add_cmake_var ("NNG_ENABLE_NNGCAT" BOOL OFF)
    cppnng_nng_add_cmake_var ("NNG_ENABLE_TESTS" BOOL OFF)
    cppnng_nng_add_cmake_var ("NNG_ENABLE_TOOLS" BOOL OFF)
    # TODO: TBD: at the moment we do not care about ZeroTier, but this could change moving forward.
    cppnng_nng_add_cmake_var ("NNG_ENABLE_ZEROTIER" BOOL OFF)

    # Make sure the BUILD directory exists.
    if (NOT EXISTS "${NNG_BUILD_DIR}")
        message (STATUS "Creating directory '${NNG_BUILD_DIR}' ...")
        make_directory ("${NNG_BUILD_DIR}")
    else ()
        message (STATUS "Directory '${NNG_BUILD_DIR}' already exists.")
    endif ()

    message (STATUS "Invoking CMake: \"${CMAKE_COMMAND}\" ${CPPNNG_NNG_CMAKE_OPTIONS} \"${NNG_SRC_DIR}\"")

    execute_process (
        COMMAND "${CMAKE_COMMAND}" ${CPPNNG_NNG_CMAKE_OPTIONS} "${NNG_SRC_DIR}"
        WORKING_DIRECTORY ${NNG_BUILD_DIR}
        RESULT_VARIABLE CPPNN_NNG_CONFIG_EC
        OUTPUT_VARIABLE CPPNN_NNG_CONFIG_MSG
    )

    # So I did try to setup a macro for one-stop shopping exit code handling, but this
    # was problematic at best, for the usually string versus variable gymnastics issues.
    if (CPPNN_NNG_CONFIG_EC EQUAL 0)
        if (CPPNN_NNG_CONFIG_MSG STREQUAL "")
            set (CPPNN_NNG_CONFIG_MSG "CMake succeeded.")
        endif()
        message (STATUS "${CPPNN_NNG_CONFIG_MSG} (${CPPNN_NNG_CONFIG_EC})")
    else ()
        if (CPPNN_NNG_CONFIG_MSG STREQUAL "")
            set (CPPNN_NNG_CONFIG_MSG "An error occurred.")
        endif ()
        message (FATAL_ERROR "${CPPNN_NNG_CONFIG_MSG} (${CPPNN_NNG_CONFIG_EC})")
    endif ()
    
endmacro ()

macro (cppnng_build_nng NNG_BUILD_DIR)

    message (STATUS "Invoking CMake: \"${CMAKE_COMMAND}\" --build .")

    execute_process (
        COMMAND "${CMAKE_COMMAND}" --build .
        WORKING_DIRECTORY ${NNG_BUILD_DIR}
        RESULT_VARIABLE CPPNN_NNG_BUILD_EC
        OUTPUT_VARIABLE CPPNN_NNG_BUILD_MSG
    )

    # Ditto string versus variable gymnastics issues.
    if (CPPNN_NNG_BUILD_EC EQUAL 0)
        if (CPPNN_NNG_BUILD_MSG STREQUAL "")
            set (CPPNN_NNG_BUILD_MSG "CMake succeeded.")
        endif()
        message (STATUS "${CPPNN_NNG_BUILD_MSG} (${CPPNN_NNG_BUILD_EC})")
    else ()
        if (CPPNN_NNG_BUILD_MSG STREQUAL "")
            set (CPPNN_NNG_BUILD_MSG "An error occurred.")
        endif ()
        message (FATAL_ERROR "${CPPNN_NNG_BUILD_MSG} (${CPPNN_NNG_BUILD_EC})")
    endif ()

endmacro ()

# We could try to jump through the hoops of the Swiss army knife ExternalProject_Add
# and related machinations, however, this seems like the best possible option IMO.

cppnng_update_nng_git_submodule ("${GIT_SUBMODULE_NNG_REPO_DIR}")

cppnng_update_nng_git_submodule ("${NNG_REVISION}" "${GIT_SUBMODULE_NNG_REPO_DIR}")

cppnng_configure_nng ("${GIT_SUBMODULE_NNG_REPO_DIR}" "${CPPNNG_NNG_BUILD_DIR}")

cppnng_build_nng ("${CPPNNG_NNG_BUILD_DIR}")
