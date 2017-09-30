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

project (nngcpp CXX)

include (GNUInstallDirs)

option (CMAKE_CONFIGURATION_TYPES "" Debug)
option (CMAKE_INSTALL_PREFIX "" install)

find_package (Threads REQUIRED)
find_package (Git REQUIRED)

if (GIT_FOUND)
    message (STATUS "Git found: ${GIT_EXECUTABLE} ${GIT_VERSION_STRING}")
else ()
    message (FATAL_ERROR "Git required: not found.")
endif()

set (_CPP .cpp)
set (_H .h)
set (_L .lib)
set (_O .obj)
set (_SO .dll)

# Should be running this from the top-most repository directory. This
# should also go without saying, but may not be as obvious to all.

set (NNGCPP_SWIG_DIR ${nngcpp_SOURCE_DIR}/swig)

# Building NNG is an internal external requirement, so to speak,
# that we want to occur at the time of configuration.
include (CMakeLists.nng.txt)

option (NNGCPP_ENABLE_DOC "Enable building documentation." ON)
option (NNGCPP_ENABLE_TESTS "Build and run tests" ON)
option (NNGCPP_ENABLE_XTRA_TOOLS "Build extra tools" OFF)
option (NNGCPP_ENABLE_NNGCAT "Enable building nngcat utility." ${NNG_TOOLS})
option (NNGCPP_ENABLE_COVERAGE "Enable coverage reporting." OFF)

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

# Relative path from the top-level CMake working directory.
add_subdirectory (swig/cpp)
# include (swig/cpp/CMakeLists.txt)

# # # TODO: TBD: include? or add? probably add is more appropriate...
# # Connects Source directory to top level CMake.
# add_subdirectory (src)

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

install (TARGETS LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install (TARGETS ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
