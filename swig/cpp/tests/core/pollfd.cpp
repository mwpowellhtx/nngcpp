//
// Copyright (c) 2017 Michael W Powell <mwpowellhtx@gmail.com>
// Copyright 2017 Garrett D'Amore <garrett@damore.org>
// Copyright 2017 Capitar IT Group BV <info@capitar.com>
//
// This software is supplied under the terms of the MIT License, a
// copy of which should be located in the distribution where this
// file was obtained (LICENSE.txt).  A copy of the license may also be
// found online at https://opensource.org/licenses/MIT.
//

#include <catch.hpp>
#include <nngcpp.h>

#include <string>
#include <memory>
#include <thread>

#ifndef _WIN32

    #include <poll.h>
    #include <unistd.h>
    #define INVALID_SOCKET -1

#else // _WIN32

    #define poll WSAPoll

    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif // WIN32_LEAN_AND_MEAN

    #include <Windows.h>
    #include <WinSock2.h>

    #include <MSWSock.h>
    #include <WS2tcpip.h>

#endif // _WIN32

TEST_CASE("Poll FDs", "[pollfd]") {

    using namespace std;
    using namespace nng;
    using namespace nng::protocol;
    using _opt_ = option_names;

    std::unique_ptr<latest_pair_socket> s1;
    std::unique_ptr<latest_pair_socket> s2;

    SECTION("Given a connected Pair of Sockets") {

        // Same as ::nng_pair_open(...) == 0
        REQUIRE_NOTHROW(s1 = std::make_unique<latest_pair_socket>());
        REQUIRE_NOTHROW(s2 = std::make_unique<latest_pair_socket>());

        /* It is a known idiomatic thing that Windows Sockets are treated as 64-bit integers, sort of.
        Yet, cross platform, and with the options, it is more accurate to simply treat them as 32-bit
        integers. */

        const auto inv_sock = INVALID_SOCKET;
        const string addr = "inproc://yeahbaby";

        // Should be ~= ::nng_listen(s, addr, listenerp, 0) == 0
        REQUIRE_NOTHROW(s1->listen(addr));
        this_thread::sleep_for(50ms);

        REQUIRE_NOTHROW(s2->dial(addr));
        this_thread::sleep_for(50ms);

        SECTION("We can get a Receive File Descriptor") {
            int fd;
            //REQUIRE_NOTHROW(s1->get_option_int(_opt_::receive_file_descriptor, &fd));
            REQUIRE_NOTHROW(s1->get_option_int(_opt_::receive_file_descriptor, &fd));
            REQUIRE(fd != inv_sock);

            SECTION("And it is always the same File Descriptor") {
                int fd2;
                REQUIRE_NOTHROW(s1->get_option_int(_opt_::receive_file_descriptor, &fd2));
                REQUIRE(fd2 == fd);
            }

            SECTION("And they start non pollable") {
                pollfd pfd{ (SOCKET)fd, POLLIN, 0 };
                REQUIRE(::poll(&pfd, 1, 0) == 0);
                REQUIRE(pfd.revents == 0);
            }

            SECTION("But if we write they are pollable") {
                pollfd pfd{ (SOCKET)fd, POLLIN, 0 };
                REQUIRE_NOTHROW(s2->send("kick")); // TODO: TBD: send appears to be blocking
                REQUIRE(::poll(&pfd, 1, 1000) == 1);
                REQUIRE((pfd.revents&POLLIN));
            }
        }

        SECTION("We can get a Send File Descriptor") {
            int fd;
            REQUIRE_NOTHROW(s1->get_option_int(_opt_::send_file_descriptor, &fd));
            REQUIRE(fd != inv_sock);
            REQUIRE_NOTHROW(s1->send("oops"));
        }

        SECTION("Must have big enough size") {
            INFO("At least for now, we simply do not expose the general get/set options API through the C++ wrapper.");
        }

        SECTION("We cannot get a Send File Descriptor for Pull Socket") {

            std::unique_ptr<latest_pull_socket> s3;
            REQUIRE_NOTHROW(s3 = std::make_unique<latest_pull_socket>());

            int fd;
            // TODO: TBD: work on exception handling...
            REQUIRE_THROWS_AS(s3->get_option_int(_opt_::send_file_descriptor, &fd), nng_exception);

            SECTION("Destructor cleans up correctly") {
                REQUIRE(s3.release() == nullptr);
            }
        }

        SECTION("We cannot get a Receive File Descriptor for Push Socket") {

            std::unique_ptr<latest_push_socket> s3;
            REQUIRE_NOTHROW(s3 = std::make_unique<latest_push_socket>());

            int fd;
            // TODO: TBD: work on exception handling...
            REQUIRE_THROWS_AS(s3->get_option_int(_opt_::receive_file_descriptor, &fd), nng_exception);

            SECTION("Destructor cleans up correctly") {
                REQUIRE(s3.release() == nullptr);
            }
        }
    }

    SECTION("Destructors do not throw") {
        REQUIRE_NOTHROW(s1.release() == nullptr);
        REQUIRE_NOTHROW(s2.release() == nullptr);
    }
}
