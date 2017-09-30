#
#   Copyright (c) 2012-2013 Martin Sustrik  All rights reserved.
#   Copyright (c) 2013 GoPivotal, Inc.  All rights reserved.
#   Copyright (c) 2015-2016 Jack R. Dunaway. All rights reserved.
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

# TODO: TBD: should also include relavent dirs for Windows and across platforms
include_directories (AFTER SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/swig/cpp/src")

set (NNGCPP_CPP_SRCS
    nngcpp.h

    core/core.h

    core/device.cpp
    core/device.h
    core/dialer.cpp
    core/dialer.h
    core/endpoint.cpp
    core/endpoint.h
    core/listener.cpp
    core/listener.h
    core/nng_exception.cpp
    core/nng_exception.h
    core/options.cpp
    core/options.h
    core/receiver.cpp
    core/receiver.h
    core/sender.cpp
    core/sender.h
    core/session.cpp
    core/session.h
    core/socket.cpp
    core/socket.h

    protocol/bus/bus.cpp
    protocol/bus/bus.h

    protocol/pair/pair_v0.cpp
    protocol/pair/pair_v0.h
    protocol/pair/pair_v1.cpp
    protocol/pair/pair_v1.h

    protocol/pipeline/pull.cpp
    protocol/pipeline/pull.h
    protocol/pipeline/push.cpp
    protocol/pipeline/push.c

    protocol/pubsub/pub.cpp
    protocol/pubsub/pub.h
    protocol/pubsub/sub.cpp
    protocol/pubsub/sub.h

    protocol/reqrep/rep.cpp
    protocol/reqrep/rep.h
    protocol/reqrep/req.cpp
    protocol/reqrep/req.h

    protocol/survey/respond.cpp
    protocol/survey/respond.h
    protocol/survey/survey.cpp
    protocol/survey/survey.h

    # The Transports are an internal concern done within the NNG code itself, FOR NOW.
)

# Additionally, anything cross-platform is something tucked away in NNG itself.
# NNG_PLATFORM_POSIX
# NNG_PLATFORM_WINDOWS

# Provide same folder structure in IDE as on disk.
foreach (f ${NNGCPP_CPP_SRCS})

    # message (FATAL_ERROR "File '${f} in path '${CMAKE_CURRENT_SOURCE_DIR}' ...")

    # Get the path of the file relative to source directory.
    if (IS_ABSOLUTE ${f})
        file (RELATIVE_PATH f ${CMAKE_CURRENT_SOURCE_DIR} ${f})
    endif ()

    # TODO: TBD: could (should?) potentially separate these into sources and headers, not unlike a "typical" VC project...
    set (NNGCPP_CPP_SRC_GROUP "${f}")
    set (f "${CMAKE_CURRENT_SOURCE_DIR}/${f}")

    # Remove the filename part.
    string (REGEX REPLACE "(.*)(/[^/]*)$" "\\1" NNGCPP_CPP_SRC_GROUP ${NNGCPP_CPP_SRC_GROUP})

    # CMake source_group expects \\, not /.
    string (REPLACE / \\ NNGCPP_CPP_SRC_GROUP ${NNGCPP_CPP_SRC_GROUP})
    source_group ("${NNGCPP_CPP_SRC_GROUP}" FILES ${f})

endforeach ()


set (NNCPP_PROJECT_NAME_STATIC ${PROJECT_NAME}.lib)
# TODO: TBD: Not .dll across all platforms: may be .so on Linux, for instance.
set (NNCPP_PROJECT_NAME_SHARED ${PROJECT_NAME}.dll)

set (NNCPP_PROJECT_STATIC_SOURCE_DIR ${PROJECT_SOURCE_DIR}/static)
set (NNCPP_PROJECT_STATIC_BINARY_DIR ${PROJECT_BINARY_DIR}/static)
set (NNCPP_PROJECT_SHARED_SOURCE_DIR ${PROJECT_SOURCE_DIR}/shared)
set (NNCPP_PROJECT_SHARED_BINARY_DIR ${PROJECT_BINARY_DIR}/shared)

set (NNCPP_INSTALL_STATIC_LIBDIR ${CMAKE_INSTALL_LIBDIR}/static)
set (NNCPP_INSTALL_STATIC_BINDIR ${CMAKE_INSTALL_BINDIR}/static)
set (NNCPP_INSTALL_SHARED_LIBDIR ${CMAKE_INSTALL_LIBDIR}/shared)
set (NNCPP_INSTALL_SHARED_BINDIR ${CMAKE_INSTALL_BINDIR}/shared)


# Configure the STATIC and then the SHARED libraries.
add_library (${NNCPP_PROJECT_NAME_STATIC} STATIC ${NNGCPP_CPP_SRCS})
target_compile_definitions (${NNCPP_PROJECT_NAME_STATIC} PUBLIC -D NNG_STATIC_LIB)

add_library (${NNCPP_PROJECT_NAME_SHARED} SHARED ${NNGCPP_CPP_SRCS})
# This is where we experience a bit of demarcation from the underlying NNG. We want to
# be consumers of the NNG static library in this instance, and not the shared library.
target_compile_definitions (${NNCPP_PROJECT_NAME_SHARED} PRIVATE -D NNG_STATIC_LIB)


# Set library outputs same as top-level project binary outputs
set_target_properties (${NNCPP_PROJECT_NAME_STATIC} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${NNCPP_PROJECT_STATIC_BINARY_DIR})
set_target_properties (${NNCPP_PROJECT_NAME_STATIC} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${NNCPP_PROJECT_STATIC_BINARY_DIR})
set_target_properties (${NNCPP_PROJECT_NAME_STATIC} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${NNCPP_PROJECT_STATIC_BINARY_DIR})
# set_target_properties (${NNCPP_PROJECT_NAME_STATIC} PROPERTIES PDB_OUTPUT_DIRECTORY ${NNCPP_PROJECT_STATIC_BINARY_DIR})
# set_target_properties (${NNCPP_PROJECT_NAME_STATIC} PROPERTIES COMPILE_PDB_OUTPUT_DIRECTORY ${NNCPP_PROJECT_STATIC_SOURCE_DIR})

set_target_properties (${NNCPP_PROJECT_NAME_SHARED} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${NNCPP_PROJECT_SHARED_BINARY_DIR})
set_target_properties (${NNCPP_PROJECT_NAME_SHARED} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${NNCPP_PROJECT_SHARED_BINARY_DIR})
set_target_properties (${NNCPP_PROJECT_NAME_SHARED} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${NNCPP_PROJECT_SHARED_BINARY_DIR})
# set_target_properties (${NNCPP_PROJECT_NAME_SHARED} PROPERTIES PDB_OUTPUT_DIRECTORY ${NNCPP_PROJECT_SHARED_BINARY_DIR})
# set_target_properties (${NNCPP_PROJECT_NAME_SHARED} PROPERTIES COMPILE_PDB_OUTPUT_DIRECTORY ${NNCPP_PROJECT_SHARED_SOURCE_DIR})

# message (FATAL_ERROR "Target link libs for '${PROJECT_NAME}' including '${NNGCPP_REQUIRED_LIBS}' ...")

target_link_libraries (${NNCPP_PROJECT_NAME_SHARED} ${NNGCPP_REQUIRED_LIBS})

if (THREADS_HAVE_PTHREAD_ARG)
    add_definitions (-pthread)
endif ()

if (CMAKE_THREAD_LIBS_INIT)
    target_link_libraries (${NNCPP_PROJECT_NAME_SHARED} "${CMAKE_THREAD_LIBS_INIT}")
endif ()

# pkg-config file
if (NNGCPP_REQUIRED_LIBS)
    foreach (lib ${NNGCPP_REQUIRED_LIBS})
        # # from original; what was this trying to do ????
        # set (NNG_REQUIRED_LFLAGS "${NNG_REQUIRED_LFLAGS} -l${lib}")
        set (NNGCPP_REQUIRED_LFLAGS "${NNGCPP_REQUIRED_LFLAGS} ${lib}.lib")
    endforeach()
endif()

# # # TODO: TBD: so this is truly disabled-disabled in the NNG source.
# # configure_file (pkgconfig.in ${PROJECT_NAME}.pc @ONLY)
# # install (
# #     FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
# #     DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

# message (FATAL_ERROR
    # "Static library dir: '${NNCPP_INSTALL_STATIC_LIBDIR}'"
    # "Static binaries dir: '${NNCPP_INSTALL_STATIC_BINDIR}'"
    # "Shared library dir: '${NNCPP_INSTALL_SHARED_LIBDIR}'"
    # "Shared binaries dir: '${NNCPP_INSTALL_SHARED_BINDIR}'"
# )

# # ????
# install (TARGETS ${NNCPP_PROJECT_NAME_STATIC}
    # ARCHIVE DESTINATION ${NNCPP_INSTALL_STATIC_LIBDIR}
    # LIBRARY DESTINATION ${NNCPP_INSTALL_STATIC_LIBDIR}
    # RUNTIME DESTINATION ${NNCPP_INSTALL_STATIC_BINDIR}
# )

# # ????
# install (TARGETS ${NNCPP_PROJECT_NAME_SHARED}
    # ARCHIVE DESTINATION ${NNCPP_INSTALL_SHARED_LIBDIR}
    # LIBRARY DESTINATION ${NNCPP_INSTALL_SHARED_LIBDIR}
    # RUNTIME DESTINATION ${NNCPP_INSTALL_SHARED_BINDIR}
# )

install (
    TARGETS ${NNCPP_PROJECT_NAME_SHARED} ${NNCPP_PROJECT_NAME_STATIC}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# # TODO: TBD: not here but at the top level then?
# install (TARGETS LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
# install (TARGETS ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

# # TODO: TBD: GLOB may not be the best choice here...
# file (GLOB_RECURSE NNCPP_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
# install (FILES ${NNCPP_HEADER_FILES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# if (NNCPP_ENABLE_NNGCAT)
#     install (TARGETS nngcat RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
# endif()
