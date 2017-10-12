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

# Analog to the Visual Studio Solution paradigm.
project (NngCppAndSwig CXX)

include (ExternalProject)
# TODO: TBD: Processor Count was included for purposes of supporting a Boost build, but since then I've gone a different direction
include (ProcessorCount)
include (GNUInstallDirs)

option (CMAKE_CONFIGURATION_TYPES "" Debug)
option (CMAKE_INSTALL_PREFIX "" install)

option (NNGCPP_ENABLE_DOC "Enable building documentation." ON)
option (NNGCPP_ENABLE_TESTS "Build and run tests" ON)
option (NNGCPP_ENABLE_XTRA_TOOLS "Build extra tools" OFF)
option (NNGCPP_ENABLE_NNGCAT "Enable building nngcat utility." ${NNG_TOOLS})
option (NNGCPP_ENABLE_COVERAGE "Enable coverage reporting." OFF)

find_package (Threads REQUIRED)

macro (verify_git_exists)

    find_package (Git REQUIRED)

    if (GIT_FOUND)
        message (STATUS "Git found: ${GIT_EXECUTABLE} ${GIT_VERSION_STRING}")
    else ()
        message (FATAL_ERROR "Git required: not found.")
    endif()

endmacro ()

verify_git_exists()

# Should be running this from the top-most repository directory. This
# should also go without saying, but may not be as obvious to all.

set (NNGCPP_SWIG_DIR ${NngCppAndSwig_SOURCE_DIR}/swig)
set (NNG_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/nngbuild)
set (NNG_GIT_REPO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/repos/nng)
set (CATCH_GIT_REPO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/repos/catch)
# TODO: TBD: for now adding a second CATCH2_GIT_REPO_DIR definition.
set (CATCH2_GIT_REPO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/repos/catch2)
set (QUICKCXXLIP_GIT_REPO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/repos/quickcpplib)

if (OFF)

    # Building NNG is an internal external requirement, so to speak,
    # that we want to occur at the time of configuration.
    include (CMakeLists.nng.txt)

else ()

    set (NNG_GIT_TAG HEAD)

    ExternalProject_Add (nng
        GIT_REPOSITORY git@github.com:nanomsg/nng.git
        GIT_TAG ${NNG_GIT_TAG}
        GIT_PROGRESS ON
        SOURCE_DIR ${NNG_GIT_REPO_DIR}
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:STRING=${NNG_INSTALL_PREFIX} -DNNG_ENABLE_COVERAGE:BOOL=OFF -DNNG_ENABLE_DOC:BOOL=OFF -DNNG_ENABLE_NNGCAT:BOOL=OFF -DNNG_ENABLE_TESTS:BOOL=OFF -DNNG_ENABLE_TOOLS:BOOL=OFF -DNNG_ENABLE_ZEROTIER:BOOL=OFF
        LOG_DOWNLOAD ON
    )

    message (STATUS "NNG '${NNG_GIT_TAG}' build configured.")

endif ()

macro (nngcpp_test_dependencies_cxx_catch TESTS_ENABLED)

    if (${TESTS_ENABLED})

        set (CATCH_GIT_TAG v1.10.0)
        # TODO: TBD: something about the format of the tag name is causing issues.
        # set (CATCH_GIT_TAG v2.0.0-develop.4)
        # # TODO: TBD: furthermore, something about the hash isn't right either.
        # set (CATCH_GIT_TAG bd46f66754f11b5644913f6eb8a1900971f3e0ad)
        # # TODO: TBD: both appear to be a CMake issue.
        # # Talk about 'bleeding edge'... I do not like being on the HEAD, I would prefer to be checking out against a "stable" tag.
        # set (CATCH_GIT_TAG HEAD)

        ExternalProject_Add (download_catch
            GIT_REPOSITORY git@github.com:philsquared/Catch.git
            GIT_TAG ${CATCH_GIT_TAG}
            GIT_PROGRESS ON
            SOURCE_DIR ${CATCH_GIT_REPO_DIR}
            LOG_DOWNLOAD ON
        )

        message (STATUS "Catch '${CATCH_GIT_TAG}' download configured.")

    endif ()

endmacro ()

macro (nngcpp_test_dependencies_cxx_quickcxxlib TESTS_ENABLED)

    # Running with CXX naming aligned with CMake conventions.
    if (${TESTS_ENABLED})

        set (QUICKCXXLIB_GIT_TAG HEAD)

        ExternalProject_Add (download_quickcpplib
            GIT_REPOSITORY git@github.com:ned14/quickcpplib.git
            GIT_TAG ${QUICKCXXLIB_GIT_TAG}
            GIT_PROGRESS ON
            SOURCE_DIR ${QUICKCXXLIP_GIT_REPO_DIR}
            CMAKE_ARGS -DPROJECT_IS_DEPENDENCY:bool=ON -Dquickcpplib_DISABLE_DOC_GEN:bool=ON
            LOG_DOWNLOAD ON
            CONFIGURE_COMMAND cmake -E echo "Skipping configure step."
            BUILD_COMMAND cmake -E echo "Skipping build step."
            INSTALL_COMMAND cmake -E echo "Skipping install step."
            TEST_COMMAND cmake -E echo "Skipping test step."
        )

        message (STATUS "QuickCppLib '${QUICKCXXLIB_GIT_TAG}' download configured.")

    endif ()

endmacro ()

nngcpp_test_dependencies_cxx_catch (${NNGCPP_ENABLE_TESTS})
nngcpp_test_dependencies_cxx_quickcxxlib (${NNGCPP_ENABLE_TESTS})

# if (NNG_ENABLE_COVERAGE)
#     # NB: This only works for GCC and Clang 3.0 and newer.  If your stuff
#     # is older than that, you will need to find something newer.  For
#     # correct reporting, we always turn off all optimizations.
#     if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
#         set (CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
#         set (CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
#     elseif (CMAKE_C_COMPILER_ID STREQUAL "Clang")
#         set (CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
#         set (CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
#     elseif (CMAKE_COMPILER_ID STREQUAL "AppleClang")
#         set (CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
#         set (CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
#     else()
#         message(FATAL_ERROR "Unable to enable coverage for your compiler.")
#     endif()
# endif()

# Connect the CMake chain to the next directory of interest.
add_subdirectory (swig)

# Build the tools

# # TODO: TBD: tools directory should be committed to src/tools/, most likely.
# if (NNGCPP_ENABLE_NNGCAT)
#     add_executable (nanocat tools/nngcat.c tools/options.c)
#     target_link_libraries (nanocat ${PROJECT_NAME})
# endif ()

# if (NNGCPP_ENABLE_DOC)
#     find_program (ASCIIDOCTOR_EXE asciidoctor)
#     if (NOT ASCIIDOCTOR_EXE)
#         message (WARNING "Could not find asciidoctor: skipping docs")
#         set (NNGCPP_ENABLE_DOC OFF)
#     else ()
#         message (STATUS "Using asciidoctor at ${ASCIIDOCTOR_EXE}")
#     endif ()
# endif ()

# # # TODO: TBD: not sure we will use ASCIIDOC for this; may look into doxygen
# # Build the documenation
# if (NNG_ENABLE_DOC)

    # set (NNG_DOCDIR ${CMAKE_CURRENT_SOURCE_DIR}/doc)
    # set (NNG_STYLESHEET ${NNG_DOCDIR}/stylesheet.css)
    # set (NNG_TITLE ${PROJECT_NAME} ${NNG_PACKAGE_VERSION})
    # set (NNG_A2M ${ASCIIDOCTOR_EXE} -b manpage -amanmanual='${NNG_TITLE}')
    # set (NNG_A2H ${ASCIIDOCTOR_EXE} -d manpage -b html5 -a stylesheeet=${NNG_STYLESHEET} -aversion-label=${PROJECT_NAME} -arevnumber=${NNG_PACKAGE_VERSION})

    # macro (add_libnng_man NAME SECT)
        # add_custom_command (
            # OUTPUT ${NAME}.${SECT}
            # COMMAND ${NNG_A2M} -o ${NAME}.${SECT} ${NNG_DOCDIR}/${NAME}.adoc
            # MAIN_DEPENDENCY ${NNG_DOCDIR}/${NAME}.adoc
        # )

        # add_custom_command (
            # OUTPUT ${NAME}.html
            # COMMAND ${NNG_A2H} -o ${NAME}.html ${NNG_DOCDIR}/${NAME}.adoc
            # DEPENDS ${NNG_STYLESHEET}
            # MAIN_DEPENDENCY ${NNG_DOCDIR}/${NAME}.adoc
        # )

        # set (NNG_MANS ${NNG_MANS} ${NAME}.${SECT})
        # set (NNG_HTMLS ${NNG_HTMLS} ${NAME}.html)

        # install (
            # FILES ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.html
            # DESTINATION ${CMAKE_INSTALL_DOCDIR}
        # )
        # install (
            # FILES ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.${SECT}
            # DESTINATION ${CMAKE_INSTALL_MANDIR}/man${SECT}
        # )

    # endmacro (add_libnng_man)

    # if (NNG_ENABLE_NNGCAT)
        # add_libnng_man (nngcat 1)
    # endif ()

    # #add_libnng_man (nn_errno 3)
    # #add_libnng_man (nn_strerror 3)
    # #add_libnng_man (nn_symbol 3)
    # #add_libnng_man (nn_symbol_info 3)
    # #add_libnng_man (nn_allocmsg 3)
    # #add_libnng_man (nn_reallocmsg 3)
    # #add_libnng_man (nn_freemsg 3)
    # #add_libnng_man (nn_socket 3)
    # #add_libnng_man (nn_close 3)
    # #add_libnng_man (nn_get_statistic 3)
    # #add_libnng_man (nn_getsockopt 3)
    # #add_libnng_man (nn_setsockopt 3)
    # #add_libnng_man (nn_bind 3)
    # #add_libnng_man (nn_connect 3)
    # #add_libnng_man (nn_shutdown 3)
    # #add_libnng_man (nn_send 3)
    # #add_libnng_man (nn_recv 3)
    # #add_libnng_man (nn_sendmsg 3)
    # #add_libnng_man (nn_recvmsg 3)
    # #add_libnng_man (nn_device 3)
    # #add_libnng_man (nn_cmsg 3)
    # #add_libnng_man (nn_poll 3)
    # #add_libnng_man (nn_term 3)

    # #add_libnng_man (nanomsg 7)
    # #add_libnng_man (nn_pair 7)
    # #add_libnng_man (nn_reqrep 7)
    # #add_libnng_man (nn_pubsub 7)
    # #add_libnng_man (nn_survey 7)
    # #add_libnng_man (nn_pipeline 7)
    # #add_libnng_man (nn_bus 7)
    # #add_libnng_man (nn_inproc 7)
    # #add_libnng_man (nn_ipc 7)
    # #add_libnng_man (nn_tcp 7)
    # #add_libnng_man (nn_ws 7)
    # #add_libnng_man (nn_env 7)

    # add_custom_target (man ALL DEPENDS ${NNG_MANS})
    # add_custom_target (html ALL DEPENDS ${NNG_HTMLS})

# endif ()

# # TODO: TBD: no, the install should happen as close to the "source" project(s) as possible
# install (TARGETS LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
# install (TARGETS ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
