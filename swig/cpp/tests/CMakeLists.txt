#
#   Copyright (c) 2012 Martin Sustrik  All rights reserved.
#   Copyright (c) 2013 GoPivotal, Inc.  All rights reserved.
#   Copyright (c) 2015-2016 Jack R. Dunaway. All rights reserved.
#   Copyright 2016 Franklin "Snaipe" Mathieu <franklinmathieu@gmail.com>
#   Copyright 2017 Garrett D'Amore <garrett@damore.org>
#   Copyright 2017 Capitar IT Group BV <info@capitar.com>
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

#  Build unit tests.

if (NNGCPP_ENABLE_TESTS)

    # This is required in order to locate the NNG resources.
    link_directories (${NNG_INSTALL_PREFIX}/lib)

    if (THREADS_HAVE_PTHREAD_ARG)
        add_definitions (-pthread)
    endif()

    set (TEST_PORT 12100)

    macro (nngcpp_add_test TEST_FILENAME TIMEOUT)
        # list (APPEND all_tests ${NAME})
        set (TEST_PATH ${TEST_FILENAME}.cpp)
        get_filename_component (TEST_NAME ${TEST_FILENAME} NAME_WE)
        add_executable (${TEST_NAME} main.cpp ${TEST_PATH})
        # Treat NNG and NNGCPP as DEPENDENCIES rather than link libraries.
        add_dependencies (${TEST_NAME} nng)
        add_dependencies (${TEST_NAME} ${NNGCPP_PROJECT_NAME_STATIC})
        target_include_directories (${TEST_NAME} PUBLIC $<BUILD_INTERFACE:${CATCH_GIT_REPO_DIR}/include>)
        # TODO: TBD: this one we may want to just include, always, as a default solution-wide include directory
        target_include_directories (${TEST_NAME} PUBLIC $<BUILD_INTERFACE:${NNG_INSTALL_PREFIX}/include>)
        # So it does not appear to be enough to Add the Dependency; we must ALSO configure Target Link Libraries.
        target_link_libraries (${TEST_NAME} ${NNGCPP_REQUIRED_LIBS} nng_static ${NNGCPP_PROJECT_NAME_STATIC})
        # TODO: TBD: c++14? or c++11?
        target_compile_definitions (${TEST_NAME} PUBLIC -D NNG_STATIC_LIB -D NNGCPP_STATIC_LIB -D TEST_PORT=${TEST_PORT})
        if (CMAKE_THREAD_LIBS_INIT)
            target_link_libraries (${TEST_NAME} "${CMAKE_THREAD_LIBS_INIT}")
        endif ()
        # -v? -p?
        # Should TEST_PORT just be a compiler definition?
        # add_test (NAME ${TEST_NAME} COMMAND ${TEST_NAME} -v -p TEST_PORT=${TEST_PORT})
        add_test (NAME ${TEST_NAME} COMMAND ${TEST_NAME} -v -p)
        set_tests_properties (${TEST_NAME} PROPERTIES TIMEOUT ${TIMEOUT})
        math (EXPR TEST_PORT "${TEST_PORT}+20")
        message (STATUS "Unit test '${TEST_FILENAME}' configured.")
    endmacro ()

    macro (add_nng_compat_test NAME TIMEOUT)
        # list (APPEND all_tests ${NAME})
        # add_executable (${NAME} ${NAME}.c)
        # target_link_libraries (${NAME} ${PROJECT_NAME}_static)
        # target_link_libraries (${NAME} ${NNG_REQUIRED_LIBRARIES})
        # target_compile_definitions(${NAME} PUBLIC -DNNG_STATIC_LIB)
        # if (CMAKE_THREAD_LIBS_INIT)
            # target_link_libraries (${NAME} "${CMAKE_THREAD_LIBS_INIT}")
        # endif ()

        # add_test (NAME ${NAME} COMMAND ${NAME} ${TEST_PORT})
        # set_tests_properties (${NAME} PROPERTIES TIMEOUT ${TIMEOUT})
        # math (EXPR TEST_PORT "${TEST_PORT}+10")
    endmacro ()

    macro (add_nng_cpp_test NAME TIMEOUT)
        # if (NOT NNG_ENABLE_COVERAGE)
            # enable_language (CXX)
            # list (APPEND all_tests ${NAME})
            # add_executable (${NAME} ${NAME}.cc)
            # target_link_libraries (${NAME} ${PROJECT_NAME}_static)
            # target_link_libraries (${NAME} ${NNG_REQUIRED_LIBRARIES})
            # target_compile_definitions(${NAME} PUBLIC -DNNG_STATIC_LIB)
            # if (CMAKE_THREAD_LIBS_INIT)
                # target_link_libraries (${NAME} "${CMAKE_THREAD_LIBS_INIT}")
            # endif ()

            # add_test (NAME ${NAME} COMMAND ${NAME} ${TEST_PORT})
            # set_tests_properties (${NAME} PROPERTIES TIMEOUT ${TIMEOUT})
            # math (EXPR TEST_PORT "${TEST_PORT}+10")
        # endif ()
    endmacro ()

else ()
    macro (cppnng_add_test NAME TIMEOUT)
    endmacro (cppnng_add_test)
    macro (add_nng_compat_test NAME TIMEOUT)
    endmacro (add_nng_compat_test)
    macro (add_nng_cpp_test NAME TIMEOUT)
    endmacro (add_nng_cpp_test)
endif ()

nngcpp_add_test (core/errors 2)
nngcpp_add_test(core/pollfd 5)

# nngcpp_add_test(bus 5)
# nngcpp_add_test(event 5)
# nngcpp_add_test(idhash 5)
# nngcpp_add_test(inproc 5)
# nngcpp_add_test(ipc 5)
# nngcpp_add_test(list 5)
# nngcpp_add_test(platform 5)
# nngcpp_add_test(reqrep 5)
# nngcpp_add_test(pipeline 5)
# nngcpp_add_test(pubsub 5)
# nngcpp_add_test(reconnect 5)
# nngcpp_add_test(resolv 10)
# nngcpp_add_test(sock 5)
# nngcpp_add_test(survey 5)
# nngcpp_add_test(synch 5)
# nngcpp_add_test(transport 5)
# nngcpp_add_test(tcp 5)
# nngcpp_add_test(tcp6 5)
# nngcpp_add_test(scalability 20)
# nngcpp_add_test(message 5)
# nngcpp_add_test(device 5)
# nngcpp_add_test(pair1 5)
# nngcpp_add_test(udp 5)

# if (NNG_HAVE_ZEROTIER)
    # nngcpp_add_test(zt 60)
# endif()

# # compatbility tests
# add_nng_compat_test(compat_block 5)
# add_nng_compat_test(compat_bug777 5)
# add_nng_compat_test(compat_bus 5)
# add_nng_compat_test(compat_cmsg 5)
# add_nng_compat_test(compat_msg 5)
# add_nng_compat_test(compat_iovec 5)
# add_nng_compat_test(compat_device 5)
# add_nng_compat_test(compat_pair 5)
# add_nng_compat_test(compat_pipeline 5)
# add_nng_compat_test(compat_reqrep 5)
# add_nng_compat_test(compat_survey 5)
# add_nng_compat_test(compat_reqttl 5)
# add_nng_compat_test(compat_shutdown 5)
