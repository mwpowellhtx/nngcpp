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

include (CheckFunctionExists)
include (CheckStructHasMember)
include (CheckSymbolExists)
include (CheckLibraryExists)
include (CheckCXXCompilerFlag)
# include (CheckCXXSourceCompiles)

macro (nngcpp_verify_cxx_compiler)

    check_cxx_compiler_flag (-std=c++11 CXX_COMPILER_HAS_11)

    if (NOT CXX_COMPILER_HAS_11)
        message (FATAL_ERROR "C++11 support is required from the '${CMAKE_CXX_COMPILER}' compiler.")
    else ()
        # # TODO: TBD: may need/want to append the flags right here and now...
        # set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        # TODO: TBD: alternately, this may also work.
        # Running with --std:c++11 for purposes of this wrapper.
        set (CMAKE_CXX_STANDARD 11)
    endif ()

endmacro ()

macro (nngcpp_set_system_definitions SYS)

    # message (FATAL_ERROR "The system is '${SYS}'")

    # TODO: TBD: I'm not sure there will be much in the way of cross-plat considerations in
    # the C++ code yet. However, otherwise, it is fairly straightforward wrapper code for the
    # most part. If necessary, we can revisit this later on, and as interest occurs.
    if (SYS MATCHES "Linux")
        # add_definitions (-DNNG_PLATFORM_POSIX)
        # add_definitions (-DNNG_PLATFORM_LINUX)
        # add_definitions (-DNNG_USE_EVENTFD)
        # set (NNG_PLATFORM_POSIX ON)

    elseif (SYS MATCHES "Darwin")
        # add_definitions (-DNNG_PLATFORM_POSIX)
        # add_definitions (-DNNG_PLATFORM_POSIX)
        # # macOS 10.12 and later have getentropy, but the older releases
        # # have ARC4_RANDOM, and that is sufficient to our needs.
        # add_definitions (-DNNG_USE_ARC4_RANDOM)

        # # macOS added some of CLOCK_MONOTONIC, but the implementation is
        # # broken and unreliable, so don't use it.
        # add_definitions (-DNNG_USE_CLOCKID=CLOCK_REALTIME)
        # set (NNG_PLATFORM_POSIX ON)

    elseif (SYS MATCHES "FreeBSD")
        # add_definitions (-DNNG_PLATFORM_POSIX)
        # add_definitions (-DNNG_PLATFORM_FREEBSD)
        # set (NNG_PLATFORM_POSIX ON)

    elseif (SYS MATCHES "NetBSD")
        # add_definitions (-DNNG_PLATFORM_POSIX)
        # add_definitions (-DNNG_PLATFORM_NETBSD)
        # set (NNG_PLATFORM_POSIX ON)

    elseif (SYS MATCHES "OpenBSD")
        # add_definitions (-DNNG_PLATFORM_POSIX)
        # add_definitions (-DNNG_PLATFORM_OPENBSD)
        # set (NNG_PLATFORM_POSIX ON)

    elseif (SYS MATCHES "SunOS")
        # add_definitions (-DNNG_PLATFORM_POSIX)
        # add_definitions (-DNNG_PLATFORM_SUNOS)

    elseif (SYS MATCHES "Windows")
        add_definitions (-D NNGCPP_PLATFORM_WINDOWS)
        add_definitions (-D _CRT_SECURE_NO_WARNINGS)
        add_definitions (-D _CRT_RAND_S)
        set (NNGCPP_PLATFORM_WINDOWS ON)

        # Target Windows Vista and later
        add_definitions (-D _WIN32_WINNT=0x0600)
        list (APPEND CMAKE_REQUIRED_DEFINITIONS -D _WIN32_WINNT=0x0600)

    else ()
        # message (AUTHOR_WARNING "WARNING: This platform may not be supported: ${SYS}")
        # message (AUTHOR_WARNING "${ISSUE_REPORT_MSG}")
        # # blithely hope for POSIX to work
        # add_definitions (-D PLATFORM_POSIX)

    endif ()

endmacro ()

macro (nngcpp_set_build_definition)

    if (NNGCPP_STATIC_LIB)
        add_definitions (-D NNGCPP_DECL=extern)
    endif ()

endmacro ()

macro (nngcpp_check_function SYM DEF)
    # So this one may not be quite as useful as check_symbol_exists: https://cmake.org/cmake/help/v3.9/module/CheckFunctionExists.html
    check_function_exists (${SYM} ${DEF})
    if (${DEF})
        add_definitions (-D ${DEF}=1)
    endif ()
endmacro ()

macro (nngcpp_check_symbol SYM HDR DEF)
    check_symbol_exists (${SYM} ${HDR} ${DEF})
    if (${DEF})
        add_definitions (-D ${DEF}=1)
    endif ()
endmacro ()

macro (nngcpp_check_library LIB SYM DEF)
    check_library_exists (${LIB} ${SYM} "" ${DEF})
    if (${DEF})
        add_definitions (-D ${DEF}=1)
        set (NNGCPP_REQUIRED_LIBS ${NNGCPP_REQUIRED_LIBS} ${LIB})
    endif ()
endmacro ()

macro (nngcpp_check_struct_member STR MEM HDR DEF)
    check_struct_has_member ("struct ${STR}" ${MEM} ${HDR} ${DEF})
    if (${DEF})
        add_definitions (-D ${DEF}=1)
    endif ()
endmacro ()

macro (nngcpp_verify_required_build_opts)

    # TODO: TBD: which should be copied from the nng_static.lib output discovered during the NNG configuration steps.
    set (NNGCPP_REQUIRED_LIBS ${NNGCPP_REQUIRED_LIBS} nng)

    if (WIN32)

        # Windows is a special snowflake.
        set (NNGCPP_REQUIRED_LIBS ${NNGCPP_REQUIRED_LIBS} ws2_32)
        set (NNGCPP_REQUIRED_LIBS ${NNGCPP_REQUIRED_LIBS} mswsock)
        set (NNGCPP_REQUIRED_LIBS ${NNGCPP_REQUIRED_LIBS} advapi32)

        # nngcpp_check_symbol (InitializeConditionVariable windows.h NNGCPP_HAVE_CONDVAR)
        # nngcpp_check_symbol (snprintf stdio${_H} NNGCPP_HAVE_SNPRINTF)

        # # TODO: TBD: debatable whether this is that required for purposes of supporting the C++ wrapper
        # if (NOT (NNGCPP_HAVE_CONDVAR AND NNGCPP_HAVE_SNPRINTF))
            # message (FATAL_ERROR
                # "Modern Windows API support is missing. "
                # "Versions of Windows prior to Vista are not supported.  "
                # "Further, the 32-bit MinGW environment is not supported. "
                # "Ensure you have at least Windows Vista or newer, and are "
                # "using either Visual Studio 2013 or newer or MinGW-W64."
            # )
        # endif ()

    else ()

        # # # TODO: TBD: fill in the blanks here as necessary, especially with open source contribution

        # # Unconditionally declare the following feature test macros.  These are
        # # needed for some platforms (glibc and SunOS/illumos) and are harmless
        # # on the others.
        # add_definitions (-D _GNU_SOURCE)
        # add_definitions (-D _REENTRANT)
        # add_definitions (-D _THREAD_SAFE)
        # add_definitions (-D _POSIX_PTHREAD_SEMANTICS)

        # # TODO: TBD: these were primarily NNG checks...
        # nngcpp_check_library (rt clock_gettime NNG_HAVE_CLOCK_GETTIME)
        # nngcpp_check_library (pthread sem_wait NNG_HAVE_SEMAPHORE_PTHREAD)
        # nngcpp_check_library (nsl gethostbyname NNG_HAVE_LIBNSL)
        # nngcpp_check_library (socket socket NNG_HAVE_LIBSOCKET)

        # # TODO: TBD: this may be the only check that really matters from the NNGCPP perspective.
        # nngcpp_check_library (nng nng_close NNGCPP_HAVE_LIBNNG)

        # nngcpp_check_symbol (AF_UNIX sys/socket.h NNG_HAVE_UNIX_SOCKETS)
        # nngcpp_check_symbol (backtrace_symbols_fd execinfo.h NNG_HAVE_BACKTRACE)
        # nngcpp_check_struct_member (msghdr msg_control sys/socket.h NNG_HAVE_MSG_CONTROL)

    endif ()

    # # TODO: TBD: was required for the NNG build. not sure yet for the NNCPP build...
    # nngcpp_check_symbol (strdup string.h NNG_HAVE_STRDUP)
    # nngcpp_check_symbol (strlcat string.h NNG_HAVE_STRLCAT)
    # nngcpp_check_symbol (strlcpy string.h NNG_HAVE_STRLCPY)
    # nngcpp_check_symbol (strnlen string.h NNG_HAVE_STRNLEN)

endmacro ()

nngcpp_verify_cxx_compiler ()

nngcpp_set_system_definitions ("${CMAKE_SYSTEM_NAME}")

nngcpp_set_build_definition ()

nngcpp_verify_required_build_opts ()

# Relative path from the top-level CMake working directory.
add_subdirectory (src)
# include (swig/cpp/src/CMakeLists.txt)
