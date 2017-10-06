//
// Copyright 2017 Garrett D'Amore <garrett@damore.org>
// Copyright 2017 Capitar IT Group BV <info@capitar.com>
//
// This software is supplied under the terms of the MIT License, a
// copy of which should be located in the distribution where this
// file was obtained (LICENSE.txt).  A copy of the license may also be
// found online at https://opensource.org/licenses/MIT.
//

// Which also includes Catch, as well as Chrono and Functional.
#include  "../tests/timed_section_impl.hpp"

#include <nngcpp.h>

#include <cctype>
#include <string>
#include <memory>
#include <thread>
#include <algorithm>

#define STRINGIFY(x)    #x

// TODO: TBD: these are a couple of interesting methods that might be interesting to include apart from the unit test itself.
template<class It_ = std::string::const_iterator>
std::string trim_right(It_ cbegin, It_ cend) {
    std::string s;
    It_ it = cend;
    auto is_trimming = true;
    do
    {
        // Trim while we are trimming.
        if (is_trimming) {
            if (iswspace(*it)) { continue; }
            // Once we are no longer trimming, then accept everything else that we identity.
            is_trimming = false;
        }
        s = *it + s;
    } while (it-- != cbegin);
    return s;
}

template <class At_, class Bt_>
bool starts_with(At_ abegin, At_ aend, Bt_ bbegin, Bt_ bend) {
    At_ acurrent;
    Bt_ bcurrent;
    for (acurrent = abegin, bcurrent = bbegin; !(acurrent == aend || bcurrent == bend)
        && *acurrent == *bcurrent; acurrent++, bcurrent++) {
    }
    // If we made it through the End of B, then the match is true.
    return bcurrent == bend;
}

// TODO: TBD: debatable how many of the constants should be included apart from the unit test itself...
namespace constants {
    const std::string url1_addr = "inproc://url1";
    const std::string url2_addr = "inproc://url2";
    const std::string bogus1_addr = "bogus://1";
    const std::string bogus2_addr = "bogus://2";
    const std::string no_addr = "inproc://no";
    const std::string asy_addr = "inproc://asy";
    const std::string here_addr = "inproc://here";
    const std::string loopback_addr = "tcp://127.0.0.1:" STRINGIFY(TEST_PORT);
    const std::string t1_addr = "inproc://t1";
    const std::string closed_addr = "inproc://closed";
    const size_t max_addr_length = NNG_MAXADDRLEN;
}

TEST_CASE("Socket Operations", "[sock]") {

    using namespace std;
    using namespace std::chrono;
    using namespace nng;
    using namespace nng::protocol;
    using namespace constants;
    using _opt_ = option_names;

    session _session_;

    SECTION("We are able to open a Pair Socket") {

        shared_ptr<latest_pair_socket> s1;

        REQUIRE_NOTHROW(s1 = _session_.create_pair_socket());
        REQUIRE(s1.get());

        SECTION("And we can shut it down") {
            REQUIRE_NOTHROW(s1->shutdown());
            // TODO: TBD: with the breadth of unit testing here, I think it might be time to seriously consider how best to perform error handling
            REQUIRE_THROWS_AS(s1->shutdown(), nng_exception);

            SECTION("It cannot receive") {
                FAIL();
                string r;
                socket::receive_size_type sz = 0;
                // TODO: TBD: was using flag_alloc
                REQUIRE_THROWS_AS(s1->try_receive(r, sz), nng_exception);
            }

            SECTION("It cannot send") {
                FAIL();
                REQUIRE_THROWS_AS(s1->send(""), nng_exception);
            }

            SECTION("Cannot create Endpoints based on Socket") {
                FAIL();
                shared_ptr<dialer> dp;
                shared_ptr<listener> lp;

                SECTION("Session cannot create Dialer given Socket and Address") {
                    FAIL();
                    REQUIRE_THROWS_AS(dp = _session_.create_dialer_ep(*s1, closed_addr), nng_exception);
                }

                SECTION("Session cannot create Listener given Socket and Address") {
                    FAIL();
                    REQUIRE_THROWS_AS(lp = _session_.create_listener_ep(*s1, closed_addr), nng_exception);
                }

                SECTION("Cannot Dial Socket and receive Dialer back") {
                    FAIL();
                    REQUIRE_THROWS_AS(s1->dial(closed_addr, dp.get()), nng_exception);
                }

                SECTION("Cannot Listen Socket and receive Listener back") {
                    FAIL();
                    REQUIRE_THROWS_AS(s1->listen(closed_addr, lp.get()), nng_exception);
                }
            }
        }

        SECTION("Its protocol and peer are still Pair") {
            REQUIRE(s1->get_protocol() == proto_pair);
            REQUIRE(s1->get_peer() == proto_pair);
        }

        SECTION("Receive with no Pipes times out correctly") {

            const auto timeout = 100ms;

            RUN_TIMED_SECTION_MILLISECONDS(timeout, [&]() {

                REQUIRE_NOTHROW(s1->set_option_usec(_opt_::receive_timeout_usec
                    , duration_cast<microseconds>(timeout).count()));

                string r;
                socket::receive_size_type sz = 1;
                REQUIRE_THROWS_AS(s1->try_receive(r, sz), nng_exception);
                REQUIRE(!r.length());
            });
        }

        SECTION("Receive non-blocking with no Pipes gives ec_again") {
            string r;
            socket::receive_size_type sz = 1;
            // TODO: TBD: ec_again ...
            REQUIRE_THROWS_AS(s1->try_receive(r, sz, to_int(flag_nonblock)), nng_exception);
            REQUIRE(!r.length());
        }

        SECTION("Send with no Pipes times out correctly") {

            const auto timeout = 100ms;

            RUN_TIMED_SECTION_MILLISECONDS(timeout, [&]() {

                REQUIRE_THROWS_AS(s1->set_option_usec(_opt_::send_timeout_usec,
                    duration_cast<microseconds>(timeout).count()), nng_exception);

                string s;
                REQUIRE_THROWS_AS(s1->send(s), nng_exception);
            });
        }

        SECTION("We can set and get options") {

            const auto timeout = 1234us;
            const auto v = static_cast<int64_t>(0);
            size_t  sz;

            REQUIRE_NOTHROW(s1->set_option_usec(_opt_::send_timeout_usec, timeout.count()));

            SECTION("Read-only options handled properly") {

                // TODO: TBD: should return: ec_ereadonly
                REQUIRE_THROWS_AS(s1->set_option_int(_opt_::receive_file_descriptor, 0), nng_exception);
                REQUIRE_THROWS_AS(s1->set_option_int(_opt_::send_file_descriptor, 0), nng_exception);
                REQUIRE_THROWS_AS(s1->set_option(_opt_::local_address, "a"), nng_exception);
            }

            SECTION("Url option works") {

                shared_ptr<listener> lp;
                shared_ptr<dialer> dp;

                REQUIRE_NOTHROW(lp = _session_.create_listener_ep(*s1, url1_addr));
                REQUIRE_NOTHROW(dp = _session_.create_dialer_ep(*s1, url2_addr));

                string url;

                url.resize(max_addr_length);
                REQUIRE_NOTHROW(lp->get_option(_opt_::url, url));
                // Check length as a smoke test, followed by string equality.
                REQUIRE(url.length() == url1_addr.length());
                REQUIRE(url == url1_addr);

                // TODO: TBD: ec_ereadonly ...
                REQUIRE_THROWS_AS(lp->set_option(_opt_::url, url), nng_exception);

                (url = "").resize(max_addr_length);
                REQUIRE_NOTHROW(dp->get_option(_opt_::url, url));
                // Check length as a smoke test, followed by string equality.
                REQUIRE(url.length() == url2_addr.length());
                REQUIRE(url == url2_addr);

                // TODO: TBD: ec_ereadonly ...
                REQUIRE_THROWS_AS(dp->set_option(_opt_::url, url), nng_exception);
            }

            SECTION("Bogus Urls not supported") {

                SECTION("Dialing fails properly") {

                    // TODO: TBD: ec_enotsup ...
                    REQUIRE_THROWS_AS(_session_.create_dialer_ep(*s1, bogus1_addr), nng_exception);

                    SECTION("Listening fails properly") {

                        // TODO: TBD: ec_enotsup ...
                        REQUIRE_THROWS_AS(_session_.create_listener_ep(*s1, bogus2_addr), nng_exception);
                    }
                }
            }

            SECTION("Dialing synch can get refused") {

                // TODO: TBD: ec_econnrefused ...
                REQUIRE_THROWS_AS(_session_.create_dialer_ep(*s1, no_addr), nng_exception);
            }

            SECTION("Dialing async does not get refused") {

                REQUIRE_NOTHROW(s1->dial(asy_addr, to_int(flag_nonblock)));

                SECTION("And connects late") {

                    shared_ptr<latest_pair_socket> s2;

                    REQUIRE_NOTHROW(s2 = _session_.create_pair_socket());

                    REQUIRE_NOTHROW(s2->listen(asy_addr));

                    // TODO: TBD: had to increase this from 100ms; and then, I'm not sure that was enough, or there might actually be something blocking
                    this_thread::sleep_for(1000ms);

                    const string data = "abc";
                    string buffer;

                    REQUIRE_NOTHROW(s1->send(data, 4));

                    /* This is a departure from the original unit testing in which allocation was expected,
                    i.e. was being called with flags = flag_alloc. In this case, we fully expect the client
                    to do the allocation and handling of its own buffers. */

                    sz = 4;
                    REQUIRE_NOTHROW(s2->try_receive(buffer, sz));
                    REQUIRE(sz == 4);
                    REQUIRE(buffer == data);

                    REQUIRE_NOTHROW(_session_.remove_pair_socket(s2.get()));
                }

                SECTION("Listening works") {

                    shared_ptr<listener> lp;

                    REQUIRE_NOTHROW(lp = _session_.create_listener_ep(*s1, here_addr));
                    // TODO: TBD: test for "has 'id'" ?
                    REQUIRE(s1.get());

                    SECTION("Second Listener fails with Address In Use") {
                        // TODO: TBD: ec_eaddrinuse ...
                        REQUIRE_THROWS_AS(_session_.create_listener_ep(*s1, here_addr), nng_exception);
                    }

                    SECTION("We cannot try to Start a Listener again") {
                        // TODO: TBD: ec_estate ...
                        REQUIRE_THROWS_AS(lp->start(), nng_exception);
                    }

                    SECTION("We can connect to it") {

                        shared_ptr<latest_pair_socket> s2;

                        REQUIRE_NOTHROW(s2 = _session_.create_pair_socket());

                        REQUIRE_NOTHROW(s2->dial(here_addr));

                        REQUIRE_NOTHROW(_session_.remove_pair_socket(s2.get()));
                    }
                }

                SECTION("Dialer creation ok") {

                    shared_ptr<dialer> dp;

                    REQUIRE_NOTHROW(dp = _session_.create_dialer_ep(*s1, loopback_addr));

                    SECTION("Options work") {
                        size_t actual;
                        const size_t value = 4321, expected = value;
                        REQUIRE_NOTHROW(dp->set_option_size(_opt_::max_receive_size, value));
                        REQUIRE_NOTHROW(dp->get_option_size(_opt_::max_receive_size, &actual));
                        REQUIRE(actual == expected);
                    }

                    SECTION("Socket options not supported for Dialer") {
                        // Not appropriate for dialer.
                        // TODO: TBD: ec_enotsup ...
                        REQUIRE_THROWS_AS(dp->set_option_int(_opt_::raw, 1), nng_exception);
                        // TODO: TBD: ec_enotsup ...
                        REQUIRE_THROWS_AS(dp->set_option_usec(_opt_::min_reconnect_time_usec, 1), nng_exception);
                    }

                    SECTION("Bad size checks") {
                        const string a = "a";
                        // TODO: TBD: ec_einval ...
                        REQUIRE_THROWS_AS(dp->set_option(_opt_::max_reconnect_time_usec, (void*)&a[0], 1), nng_exception);
                    }
                    //// TODO: TBD: the NNG testing tested for confusion between dialer and listener ID, API, etc; however, that really isn't possible under the C++ paradigm. it is a "use case" that has been designed out of the picture.
                    //SECTION("Cannot listen") {
                    //    FAIL();
                    //    // TODO: TBD: was nng_listener_start(dp, 0) ...
                    //    // TODO: TBD: however, this API has been better designed via the C++ paradigm
                    //    // TODO: TBD: ec_enotsup ...
                    //    REQUIRE_THROWS_AS(dp->start(), nng_exception);
                    //});
                }

                SECTION("Listener creation ok") {

                    shared_ptr<listener> lp;

                    REQUIRE_NOTHROW(lp = _session_.create_listener_ep(*s1, loopback_addr));

                    SECTION("Options work") {
                        size_t actual;
                        const size_t value = 4321, expected = value;
                        REQUIRE_NOTHROW(lp->set_option_size(_opt_::max_receive_size, value));
                        REQUIRE_NOTHROW(lp->get_option_size(_opt_::max_receive_size, &actual));
                        REQUIRE(actual == expected);
                    }

                    SECTION("Socket options not supported for Listener") {
                        // Not appropriate for dialer.
                        // TODO: TBD: ex_enotsup ...
                        REQUIRE_THROWS_AS(lp->set_option_int(_opt_::raw, 1), nng_exception);
                        // TODO: TBD: ex_enotsup ...
                        REQUIRE_THROWS_AS(lp->set_option_usec(_opt_::min_reconnect_time_usec, 1), nng_exception);
                    }

                    SECTION("Bad size checks") {
                        // TODO: TBD: ec_einval ...
                        REQUIRE_THROWS_AS(lp->set_option(_opt_::max_reconnect_time_usec, "a", 1), nng_exception);
                    }

                    // TODO: ditto dialer unit testing above: dialer cannot listen; listener cannot dial, this is a design decision not a test decision.
                    //SECTION("Cannot dial") {
                    //    FAIL();
                    //    So(nng_dialer_start(ep, 0) == NNG_ENOTSUP);
                    //});
                }

                SECTION("Cannot access absent Listener Endpoint options") {

                    // Setup an "absent" Listener Endpoint.
                    listener l;

                    // TODO: TBD: a lot of gets here, but only one set?
                    // TODO: TBD: ec_enoent ...
                    REQUIRE_THROWS_AS(l.set_option_size(_opt_::max_receive_size, 0), nng_exception);

                    int value;
                    size_t sz = 1;

                    // TODO: TBD: ec_enoent ...
                    REQUIRE_THROWS_AS(l.get_option(_opt_::raw, (void*)&value, &sz), nng_exception);
                    // TODO: TBD: ec_enoent ...
                    REQUIRE_THROWS_AS(l.get_option_size(_opt_::max_receive_size, &sz), nng_exception);
                    // TODO: TBD: ec_enoent ...
                    REQUIRE_THROWS_AS(l.get_option_int(_opt_::raw, &value), nng_exception);

                    uint64_t timeout;

                    // TODO: TBD: ec_enoent ...
                    REQUIRE_THROWS_AS(l.get_option_usec(_opt_::linger, &timeout), nng_exception);
                }

                SECTION("Cannot access absent Dialer Endpoint options") {

                    // Setup an "absent" Dialer Endpoint.
                    dialer d;

                    // TODO: TBD: a lot of gets here, but only one set?
                    // TODO: TBD: ec_enoent ...
                    REQUIRE_THROWS_AS(d.set_option_size(_opt_::max_receive_size, 0), nng_exception);

                    int value;
                    size_t sz = 1;

                    // TODO: TBD: ec_enoent ...
                    REQUIRE_THROWS_AS(d.get_option(_opt_::raw, (void*)&value, &sz), nng_exception);
                    // TODO: TBD: ec_enoent ...
                    REQUIRE_THROWS_AS(d.get_option_size(_opt_::max_receive_size, &sz), nng_exception);
                    // TODO: TBD: ec_enoent ...
                    REQUIRE_THROWS_AS(d.get_option_int(_opt_::raw, &value), nng_exception);

                    uint64_t timeout;

                    // TODO: TBD: ec_enoent ...
                    REQUIRE_THROWS_AS(d.get_option_usec(_opt_::linger, &timeout), nng_exception);
                }

                SECTION("We can send and receive messages") {

                    int length;
                    const auto timeout = 3000ms;

                    shared_ptr<latest_pair_socket> s2;

                    REQUIRE_NOTHROW(s2 = _session_.create_pair_socket());

                    REQUIRE_NOTHROW(s1->set_option_int(_opt_::receive_buffer, 1));
                    REQUIRE_NOTHROW(s1->get_option_int(_opt_::receive_buffer, &length));
                    REQUIRE(length == 1);

                    REQUIRE_NOTHROW(s1->set_option_int(_opt_::send_buffer, 1));
                    REQUIRE_NOTHROW(s2->set_option_int(_opt_::send_buffer, 1));

                    REQUIRE_NOTHROW(s1->set_option_usec(_opt_::send_timeout_usec, timeout.count()));
                    REQUIRE_NOTHROW(s1->set_option_usec(_opt_::receive_timeout_usec, timeout.count()));
                    REQUIRE_NOTHROW(s2->set_option_usec(_opt_::send_timeout_usec, timeout.count()));
                    REQUIRE_NOTHROW(s2->set_option_usec(_opt_::receive_timeout_usec, timeout.count()));

                    REQUIRE_NOTHROW(s1->listen(t1_addr));
                    REQUIRE_NOTHROW(s2->dial(t1_addr));

                    size_t sz = 4;
                    const string sent = "abc", expected = sent;

                    REQUIRE_NOTHROW(s1->send(sent, sz));

                    string actual;
                    actual.resize(sz);

                    REQUIRE_NOTHROW(s2->try_receive(actual, sz));
                    // TODO: TBD: I'm not sure that this is quite right here... or that it should even be that necessary...
                    actual = trim_right(actual.cbegin(), actual.cend());
                    REQUIRE(actual.size() == sz);
                    REQUIRE(actual == expected);

                    REQUIRE_NOTHROW(_session_.remove_pair_socket(s2.get()));
                }
            }
        }
    }
}
