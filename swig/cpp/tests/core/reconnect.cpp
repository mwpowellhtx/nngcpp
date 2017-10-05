//
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

#define APPENDSTR(m, s) nng_msg_append(m, s, strlen(s))
#define CHECKSTR(m, s)                   \
	So(nng_msg_len(m) == strlen(s)); \
	So(memcmp(nng_msg_body(m), s, strlen(s)) == 0)

namespace nng {
    struct messages {
    private:
        messages() {}

    public:

        static const std::string hello;
        static const std::string again;
    };
}

const std::string nng::messages::hello = "hello";
const std::string nng::messages::again = "again";

TEST_CASE("Reconnect works", "[reconnect]") {

    using namespace std;
    using namespace std::chrono;
    using namespace nng;
    using namespace nng::protocol;
    using _opt_ = option_names;
    using msgs = messages;

    // Which "at-exit" destructor handles cleaning up the NNG resources: i.e. ::nng_fini.
    session _session_;

    const string addr = "inproc://test";

    SECTION("We can create a pipeline") {

        std::shared_ptr<latest_push_socket> push;
        std::shared_ptr<latest_pull_socket> pull;

        REQUIRE_NOTHROW(push = _session_.create_push_socket());
        REQUIRE_NOTHROW(pull = _session_.create_pull_socket());

        duration<uint64_t, micro> reconnect_time = 10000us;

        // TODO: TBD: count() might be the way to go here, but I cannot say for sure.
        // http://en.cppreference.com/w/cpp/chrono/duration
        REQUIRE_NOTHROW(pull->set_option_usec(_opt_::min_reconnect_time_usec, reconnect_time.count()));
        REQUIRE_NOTHROW(pull->set_option_usec(_opt_::max_reconnect_time_usec, reconnect_time.count()));


        SECTION("Dialing before listening works") {

            REQUIRE_NOTHROW(push->dial(addr, to_int(flag_nonblock)));
            this_thread::sleep_for(100ms);

            REQUIRE_NOTHROW(pull->listen(addr));

            /* 'Frame' in this case is loosely speaking. We do not care about exposing the NNG msg structure,
            per se. Rather, we should simply be able to "frame" buffers or strings, accordingly. */
            SECTION("We can send a frame") {
                this_thread::sleep_for(100ms);
                const auto& hello = msgs::hello;
                REQUIRE_NOTHROW(push->send(hello, hello.length()));
                string r;
                REQUIRE_NOTHROW(pull->try_receive(r, hello.length()));
                REQUIRE(r == hello);
            }
        }

        SECTION("Reconnection works") {

            REQUIRE_NOTHROW(push->dial(addr, to_int(flag_nonblock)));

            {
                auto lp = _session_.create_listener();
                REQUIRE(lp.get());

                REQUIRE_NOTHROW(pull->listen(addr, lp.get()));
                this_thread::sleep_for(100ms);

                _session_.remove_listener(lp.get());
                lp.reset();
            }

            REQUIRE_NOTHROW(pull->listen(addr));

            // TODO: TBD: This was heavily nng_pipe based from reconnect.c
            // TODO: TBD: we may provide comprehension of nng_pipe from a nngcpp POV, but I'm not sure the complexity of nng_msg how that is different from a simple send/receive?
            SECTION("They still exchange frames") {
                SECTION("Even after pipe close") {
                }
            }

            // TODO: TBD: have a look at listeners, dialers, which are endpoints: they reportedly carry "pipe" details with them;
            // TODO: TBD: not to be confused with the classical "pipes" in Windows or Linux vocabulary; better name would be "channel", perhaps.
            // TODO: TBD: but for confusion with mangos...
#if 0
            Convey("Reconnection works", {
                nng_listener l;
            So(nng_dial(push, addr, NULL, NNG_FLAG_NONBLOCK) == 0);
            So(nng_listen(pull, addr, &l, 0) == 0);
            nng_usleep(100000);
            nng_listener_close(l);
            So(nng_listen(pull, addr, NULL, 0) == 0);

            Convey("They still exchange frames",{
                nng_msg *msg;
            nng_pipe p1;

            nng_usleep(100000);
            So(nng_msg_alloc(&msg, 0) == 0);
            APPENDSTR(msg, "hello");
            So(nng_sendmsg(push, msg, 0) == 0);
            msg = NULL;
            So(nng_recvmsg(pull, &msg, 0) == 0);
            So(msg != NULL);
            CHECKSTR(msg, "hello");
            p1 = nng_msg_get_pipe(msg);
            nng_msg_free(msg);

            Convey("Even after pipe close",{
                nng_pipe p2;

            nng_pipe_close(p1);
            nng_usleep(100000);
            So(nng_msg_alloc(&msg, 0) == 0);
            APPENDSTR(msg, "again");
            So(nng_sendmsg(push, msg, 0) == 0);
            msg = NULL;
            So(nng_recvmsg(pull, &msg, 0) == 0);
            So(msg != NULL);
            CHECKSTR(msg, "again");
            p2 = nng_msg_get_pipe(msg);
            nng_msg_free(msg);
            So(p2 != p1);
            });
            });
            });
#endif //0

        }
    }
}
