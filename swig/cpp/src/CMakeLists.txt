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

include_directories (AFTER SYSTEM "${NNG_INSTALL_PREFIX}/include")

# It is recommended to list ALL of the source files like so; as compared/contrasted with, say,
# file (GLOB) or even file (GLOB_RECURSE). This is because CMake fails to see new or updates
# files that are GLOB'ed unless detectable changes also occur to the CMake files themselves.
set (NNGCPP_CPP_SRCS
    nngcpp.h
    nngcpp_integration.h
    core/enums.h
    core/enums.cpp
    core/core.h
    core/address.cpp
    core/address.h
    core/device.cpp
    core/device.h
    core/dialer.cpp
    core/dialer.h
    core/endpoint.cpp
    core/endpoint.h
    core/listener.cpp
    core/listener.h
    core/messenger.cpp
    core/messenger.h
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
    core/exceptions.hpp
    core/exceptions/nng_exception.cpp
    core/exceptions/nng_exception.h
    core/exceptions/not_implemented.cpp
    core/exceptions/not_implemented.h
    core/exceptions/invalid_operation.cpp
    core/exceptions/invalid_operation.h
    messaging/binary_message.cpp
    messaging/binary_message.h
    messaging/binary_message_body.cpp
    messaging/binary_message_body.h
    messaging/binary_message_header.cpp
    messaging/binary_message_header.h
    messaging/message_base.cpp
    messaging/message_base.h
    messaging/message_pipe.cpp
    messaging/message_pipe.h
    messaging/messaging_utils.cpp
    messaging/messaging_utils.h
    messaging/messaging_api_base.hpp
    messaging/messaging_gymnastics.cpp
    messaging/messaging_gymnastics.h
    protocol/bus/bus.cpp
    protocol/bus/bus.h
    protocol/pair/pair_v0.cpp
    protocol/pair/pair_v0.h
    protocol/pair/pair_v1.cpp
    protocol/pair/pair_v1.h
    protocol/pipeline/pull.cpp
    protocol/pipeline/pull.h
    protocol/pipeline/push.cpp
    protocol/pipeline/push.h
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
)

macro (process_source_groups)

    message (STATUS "Processing source groups for project '${PROJECT_NAME}' ...")
    foreach (f IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${f}")
            file (RELATIVE_PATH f_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${f}")
        else ()
            set (f_rel "${f}")
        endif ()
        get_filename_component (f_path "${f_rel}" PATH)
        # TODO: TBD: this one may get somewhat platform specific.
        string (REPLACE / \\ f_path_msvc "${f_path}")
        message (STATUS "Placing file '${f}' in source group [${f_path_msvc}] ...")
        source_group ("${f_path_msvc}" FILES "${f}")
    endforeach ()
    message (STATUS "'${PROJECT_NAME}' source groups processing complete.")

endmacro ()

process_source_groups (${NNGCPP_CPP_SRCS})

# Additionally, anything cross-platform is something tucked away in NNG itself.
# NNG_PLATFORM_POSIX
# NNG_PLATFORM_WINDOWS

# Technically, PROJECT_NAME is analogous to "Solution Name", which we do not want to confuse with the actual "PROJECTS".
set (NNGCPP_PROJECT_NAME_BASE nngcpp)

set (NNGCPP_PROJECT_NAME_STATIC ${NNGCPP_PROJECT_NAME_BASE}.lib CACHE INTERNAL "")
# TODO: TBD: Not .dll across all platforms: may be .so on Linux, for instance.
set (NNGCPP_PROJECT_NAME_SHARED ${NNGCPP_PROJECT_NAME_BASE}.dll)

set (NNGCPP_PROJECT_STATIC_SOURCE_DIR ${PROJECT_SOURCE_DIR}/static)
set (NNGCPP_PROJECT_STATIC_BINARY_DIR ${PROJECT_BINARY_DIR}/static)
set (NNGCPP_PROJECT_SHARED_SOURCE_DIR ${PROJECT_SOURCE_DIR}/shared)
set (NNGCPP_PROJECT_SHARED_BINARY_DIR ${PROJECT_BINARY_DIR}/shared)

set (NNGCPP_INSTALL_STATIC_LIBDIR ${CMAKE_INSTALL_LIBDIR}/static)
set (NNGCPP_INSTALL_STATIC_BINDIR ${CMAKE_INSTALL_BINDIR}/static)
set (NNGCPP_INSTALL_SHARED_LIBDIR ${CMAKE_INSTALL_LIBDIR}/shared)
set (NNGCPP_INSTALL_SHARED_BINDIR ${CMAKE_INSTALL_BINDIR}/shared)


# Configure the STATIC and then the SHARED libraries.
add_library (${NNGCPP_PROJECT_NAME_STATIC} STATIC ${NNGCPP_CPP_SRCS})
# TODO: TBD: /Zc:wchar_t- is for Visual Studio CL.EXE, I think.
target_compile_definitions (${NNGCPP_PROJECT_NAME_STATIC} PUBLIC -D NNG_STATIC_LIB -D NNGCPP_STATIC_LIB)

add_library (${NNGCPP_PROJECT_NAME_SHARED} SHARED ${NNGCPP_CPP_SRCS})
# This is where we experience a bit of demarcation from the underlying NNG. We want to
# be consumers of the NNG static library in this instance, and not the shared library.
target_compile_definitions (${NNGCPP_PROJECT_NAME_SHARED} PRIVATE -D NNG_STATIC_LIB -D NNGCPP_SHARED_LIB)


# Set library outputs same as top-level project binary outputs
set_target_properties (${NNGCPP_PROJECT_NAME_STATIC} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${NNGCPP_PROJECT_STATIC_BINARY_DIR})
set_target_properties (${NNGCPP_PROJECT_NAME_STATIC} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${NNGCPP_PROJECT_STATIC_BINARY_DIR})
set_target_properties (${NNGCPP_PROJECT_NAME_STATIC} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${NNGCPP_PROJECT_STATIC_BINARY_DIR})
# set_target_properties (${NNGCPP_PROJECT_NAME_STATIC} PROPERTIES PDB_OUTPUT_DIRECTORY ${NNGCPP_PROJECT_STATIC_BINARY_DIR})
# set_target_properties (${NNGCPP_PROJECT_NAME_STATIC} PROPERTIES COMPILE_PDB_OUTPUT_DIRECTORY ${NNGCPP_PROJECT_STATIC_SOURCE_DIR})

set_target_properties (${NNGCPP_PROJECT_NAME_SHARED} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${NNGCPP_PROJECT_SHARED_BINARY_DIR})
set_target_properties (${NNGCPP_PROJECT_NAME_SHARED} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${NNGCPP_PROJECT_SHARED_BINARY_DIR})
set_target_properties (${NNGCPP_PROJECT_NAME_SHARED} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${NNGCPP_PROJECT_SHARED_BINARY_DIR})
# set_target_properties (${NNGCPP_PROJECT_NAME_SHARED} PROPERTIES PDB_OUTPUT_DIRECTORY ${NNGCPP_PROJECT_SHARED_BINARY_DIR})
# set_target_properties (${NNGCPP_PROJECT_NAME_SHARED} PROPERTIES COMPILE_PDB_OUTPUT_DIRECTORY ${NNGCPP_PROJECT_SHARED_SOURCE_DIR})

# message (FATAL_ERROR "Target link libs for '${PROJECT_NAME}' including '${NNGCPP_REQUIRED_LIBS}' ...")

add_dependencies (${NNGCPP_PROJECT_NAME_SHARED} nng)
add_dependencies (${NNGCPP_PROJECT_NAME_STATIC} nng)

target_link_libraries (${NNGCPP_PROJECT_NAME_SHARED} ${NNGCPP_REQUIRED_LIBS})

if (THREADS_HAVE_PTHREAD_ARG)
    add_definitions (-pthread)
endif ()

if (CMAKE_THREAD_LIBS_INIT)
    target_link_libraries (${NNGCPP_PROJECT_NAME_SHARED} "${CMAKE_THREAD_LIBS_INIT}")
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
    # "Static library dir: '${NNGCPP_INSTALL_STATIC_LIBDIR}'"
    # "Static binaries dir: '${NNGCPP_INSTALL_STATIC_BINDIR}'"
    # "Shared library dir: '${NNGCPP_INSTALL_SHARED_LIBDIR}'"
    # "Shared binaries dir: '${NNGCPP_INSTALL_SHARED_BINDIR}'"
# )

# # ????
# install (TARGETS ${NNGCPP_PROJECT_NAME_STATIC}
    # ARCHIVE DESTINATION ${NNGCPP_INSTALL_STATIC_LIBDIR}
    # LIBRARY DESTINATION ${NNGCPP_INSTALL_STATIC_LIBDIR}
    # RUNTIME DESTINATION ${NNGCPP_INSTALL_STATIC_BINDIR}
# )

# # ????
# install (TARGETS ${NNGCPP_PROJECT_NAME_SHARED}
    # ARCHIVE DESTINATION ${NNGCPP_INSTALL_SHARED_LIBDIR}
    # LIBRARY DESTINATION ${NNGCPP_INSTALL_SHARED_LIBDIR}
    # RUNTIME DESTINATION ${NNGCPP_INSTALL_SHARED_BINDIR}
# )

# Yes, in fact we want install () to be as close to the specific source as possible.
install (
    TARGETS ${NNGCPP_PROJECT_NAME_SHARED} ${NNGCPP_PROJECT_NAME_STATIC}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# if (NNGCPP_ENABLE_NNGCAT)
#     install (TARGETS nngcat RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
# endif()
