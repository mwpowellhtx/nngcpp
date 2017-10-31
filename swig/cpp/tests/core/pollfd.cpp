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

#include <nngcpp.h>

#include "../catch/catch_exception_matcher_base.hpp"
#include "../catch/catch_exception_translations.hpp"
#include "../catch/catch_nng_exception_matcher.hpp"
#include "../catch/catch_macros.hpp"

#include <memory>

#ifndef _WIN32
#   include <poll.h>
#   include <unistd.h>
#   define INVALID_SOCKET -1
#else // _WIN32
#   define poll WSAPoll
#   ifndef WIN32_LEAN_AND_MEAN
#       define WIN32_LEAN_AND_MEAN
#   endif // WIN32_LEAN_AND_MEAN
#   include <Windows.h>
#   include <WinSock2.h>
#   include <MSWSock.h>
#   include <WS2tcpip.h>
#endif // _WIN32

namespace constants {
    const std::string addr = "inproc://yeahbaby";
}

TEST_CASE("Poll FDs", "[pollfd]") {

    using namespace std;
    using namespace nng;
    using namespace nng::protocol;
    using namespace nng::exceptions;
    using namespace constants;
    using namespace Catch::Matchers;
    using _opt_ = option_names;

    std::unique_ptr<latest_pair_socket> s1, s2;

    REQUIRE(s1 == nullptr);
    REQUIRE(s2 == nullptr);

    SECTION("Given a connected Pair of Sockets") {

        // Same as ::nng_pair_open(...) == 0
        REQUIRE_NOTHROW(s1 = make_unique<latest_pair_socket>());
        REQUIRE_NOTHROW(s2 = make_unique<latest_pair_socket>());

        /* It is a known idiomatic thing that Windows Sockets are treated as 64-bit integers, sort of.
        Yet, cross platform, and with the options, it is more accurate to simply treat them as 32-bit
        integers. */

        const auto inv_sock = INVALID_SOCKET;

        // Should be ~= ::nng_listen(s, addr, listenerp, 0) == 0
        REQUIRE_NOTHROW(s1->listen(addr));
        this_thread::sleep_for(50ms);

        REQUIRE_NOTHROW(s2->dial(addr));
        this_thread::sleep_for(50ms);

        SECTION("We can get a Receive File Descriptor") {
            int fd;

            REQUIRE_NOTHROW(fd = s1->GetOptions()->GetInt32(_opt_::recv_fd));
            REQUIRE(fd != inv_sock);

            SECTION("And it is always the same File Descriptor") {
                int fd2;
                REQUIRE_NOTHROW(fd2 = s1->GetOptions()->GetInt32(_opt_::recv_fd));
                REQUIRE(fd2 == fd);
            }

            SECTION("And they start non pollable") {
                pollfd pfd{ (SOCKET)fd , POLLIN, 0 };
                REQUIRE(::poll(&pfd, 1, 0) == 0);
                REQUIRE(pfd.revents == 0);
            }

            SECTION("But if we write they are pollable") {
                pollfd pfd{ (SOCKET)fd , POLLIN, 0 };
                const buffer_vector_type _kick_buf = { 'k','i','c','k','\0' };
                REQUIRE_NOTHROW(s2->Send(&_kick_buf, _kick_buf.size())); // TODO: TBD: was blocking prior to this...
                REQUIRE(::poll(&pfd, 1, 1000) == 1);
                REQUIRE((pfd.revents&POLLIN));
            }
        }

        SECTION("We can get a Send File Descriptor") {

            int fd;
            std::size_t sz;

            sz = sizeof(fd);
            REQUIRE_NOTHROW(fd = s1->GetOptions()->GetInt32(_opt_::send_fd));
            REQUIRE_NOTHROW(s1->GetOptions()->get(_opt_::send_fd, &fd, sz));
            REQUIRE(fd != inv_sock);

            const auto buf = messaging_utils::to_buffer("oops");
            sz = 4;

            REQUIRE_NOTHROW(s1->Send(&buf, sz));
        }

        SECTION("Must have big enough size") {

            int fd;
            std::size_t sz;

            sz = 1;
            REQUIRE_THROWS_AS_MATCHING(s1->GetOptions()->get(_opt_::recv_fd, &fd, sz), nng_exception, THROWS_NNG_EXCEPTION(ec_einval));

            sz = 128;
            REQUIRE_NOTHROW(s1->GetOptions()->get(_opt_::recv_fd, &fd, sz));
            REQUIRE(sz == sizeof(fd));
        }

        SECTION("We cannot get a Send File Descriptor for Pull Socket") {

            std::unique_ptr<latest_pull_socket> s3;
            REQUIRE_NOTHROW(s3 = make_unique<latest_pull_socket>());

            int fd;
            std::size_t sz;

            sz = sizeof(fd);
            // TODO: TBD: ditto working interim answer...
            REQUIRE_THROWS_AS_MATCHING(s3->GetOptions()->get(_opt_::send_fd, &fd, sz), nng_exception, THROWS_NNG_EXCEPTION(ec_enotsup));
            REQUIRE_THROWS_AS_MATCHING(s3->GetOptions()->GetInt32(_opt_::send_fd), nng_exception, THROWS_NNG_EXCEPTION(ec_enotsup));

            REQUIRE_NOTHROW(s3.reset());
            REQUIRE(s3 == nullptr);
        }

        SECTION("We cannot get a Receive File Descriptor for Push Socket") {

            std::unique_ptr<latest_push_socket> s3;
            REQUIRE_NOTHROW(s3 = make_unique<latest_push_socket>());

            int fd;
            std::size_t sz;

            sz = sizeof(fd);
            // TODO: TBD: this works as an interim measure, although the ResultBuilder needs a little help to better comprehend the result.
            REQUIRE_THROWS_AS_MATCHING(s3->GetOptions()->get(_opt_::recv_fd, &fd, sz), nng_exception, THROWS_NNG_EXCEPTION(ec_enotsup));
            REQUIRE_THROWS_AS_MATCHING(s3->GetOptions()->GetInt32(_opt_::recv_fd), nng_exception, THROWS_NNG_EXCEPTION(ec_enotsup));

            REQUIRE_NOTHROW(s3.reset());
            REQUIRE(s3 == nullptr);
        }
    }

    SECTION("Destructors do not throw") {
        REQUIRE_NOTHROW(s1.reset());
        REQUIRE_NOTHROW(s2.reset());
        REQUIRE(s1 == nullptr);
        REQUIRE(s2 == nullptr);
    }
}
