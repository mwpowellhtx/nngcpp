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

// Which also includes Catch, as well as Chrono and Functional.
#include "../catch/catch_matchers_regex.hpp"
#include "../catch/catch_exception_translations.hpp"
#include "../catch/catch_nng_exception_matcher.hpp"
#include "../catch/catch_macros.hpp"
#include "../catch/timed_section_impl.hpp"
#include "../helpers/constants.h"

// TODO: TBD: update this module: follow the C API grouping more or less closely, adapting for C+ wrapperisms
// TODO: TBD: generally, update this taking into consideration updates; could "transaction" be useful now, in more broad sense? i.e. looking at constants::to_str for instance...

#include <cctype>
#include <memory>
#include <algorithm>

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

template<class It_ = std::string::const_iterator>
std::string trim_left(It_ cbegin, It_ cend) {
    std::string s;
    It_ it = cbegin;
    auto is_trimming = true;
    do
    {
        // Trim while we are trimming.
        if (is_trimming) {
            if (iswspace(*it)) { continue; }
            // Once we are no longer trimming, then accept everything else that we identity.
            is_trimming = false;
        }
        s = s + *it;
    } while (it++ != cend);
    return s;
}

template<class It_ = std::string::const_iterator>
std::string trim(It_ cbegin, It_ cend) {
    auto s = trim_left(cbegin, cend);
    return trim_right(s.cbegin(), s.cend());
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
    const std::string loopback_addr = "tcp://127.0.0.1:" + to_str(TEST_PORT);
    const std::string loopback_addr_regex_pattern = "tcp://[\\d]+\\.[\\d]+\\.[\\d]+\\.[\\d]+:[\\d]+";
    const std::string t1_addr = "inproc://t1";
    const std::string closed_addr = "inproc://closed";

    const size_t max_addr_length = NNG_MAXADDRLEN;

    const nng::messaging::buffer_vector_type empty_buf;
    const auto data_buf = to_buffer("data");
}

TEST_CASE("Socket Operations", "[socket][operations][ngg][cxx]") {

    using namespace std;
    using namespace std::chrono;
    using namespace nng;
    using namespace nng::messaging;
    using namespace nng::protocol;
    using namespace Catch;
    using namespace Catch::Matchers;
    using namespace constants;
    using O = option_names;

    // TODO: TBD: re-work this in better alignment with whatever message-based API was being tested.

    //// TODO: TBD: this is useful when troubleshooting hard crashes.
    // cout << __FUNCSIG__ << " (" << __FILE__ << ":" << __LINE__ << ")" << endl;

    session _session_;

    CHECK_THAT(loopback_addr, MatchesRegEx(loopback_addr_regex_pattern));

    SECTION("We are able to open a Pair Socket") {

        shared_ptr<latest_pair_socket> s1;

        REQUIRE_NOTHROW(s1 = _session_.create_pair_socket());
        REQUIRE(s1.get());

        SECTION("And we can shut it down") {

            REQUIRE_NOTHROW(s1->shutdown());
            // TODO: TBD: this will work for now as a rough cut Exception match...
            REQUIRE_THROWS_AS_MATCHING(s1->shutdown(), nng_exception, THROWS_NNG_EXCEPTION(ec_eclosed));

            SECTION("It cannot receive") {

                size_type sz = 0;
                buffer_vector_type buf;

                // TODO: TBD: was using flag_alloc
                // TODO: TBD: this will work for now as a rough cut Exception match...
                REQUIRE_THROWS_AS_MATCHING(s1->try_receive(&buf, sz), nng_exception, THROWS_NNG_EXCEPTION(ec_eclosed));
            }

            SECTION("It cannot send") {

                // TODO: TBD: this will work for now as a rough cut Exception match...
                REQUIRE_THROWS_AS_MATCHING(s1->send(&empty_buf), nng_exception, THROWS_NNG_EXCEPTION(ec_eclosed));
            }

            SECTION("Cannot create Endpoints based on Socket") {

                SECTION("Session cannot create Dialer given Socket and Address") {

                    // Do not keep track of these pointers.
                    // TODO: TBD: this will work for now as a rough cut Exception match...
                    REQUIRE_THROWS_AS_MATCHING(_session_.create_dialer_ep(*s1, closed_addr), nng_exception, THROWS_NNG_EXCEPTION(ec_eclosed));
                }

                SECTION("Session cannot create Listener given Socket and Address") {

                    // Do not keep track of these pointers.
                    // TODO: TBD: this will work for now as a rough cut Exception match...
                    REQUIRE_THROWS_AS_MATCHING(_session_.create_listener_ep(*s1, closed_addr), nng_exception, THROWS_NNG_EXCEPTION(ec_eclosed));
                }

                SECTION("Cannot Dial Socket and receive Dialer back") {

                    auto dp = _session_.create_dialer_ep();
                    REQUIRE(dp != nullptr);
                    // TODO: TBD: this will work for now as a rough cut Exception match...
                    REQUIRE_THROWS_AS_MATCHING(s1->dial(closed_addr, dp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_eclosed));
                    REQUIRE_NOTHROW(_session_.remove_dialer_ep(dp.get()));
                }

                SECTION("Cannot Listen Socket and receive Listener back") {

                    auto lp = _session_.create_listener_ep();
                    REQUIRE(lp != nullptr);
                    // TODO: TBD: this will work for now as a rough cut Exception match...
                    REQUIRE_THROWS_AS_MATCHING(s1->listen(closed_addr, lp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_eclosed));
                    REQUIRE_NOTHROW(_session_.remove_listener_ep(lp.get()));
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

                REQUIRE_NOTHROW(s1->set_option(O::receive_timeout_duration, timeout));

                unique_ptr<binary_message> bmp;

                REQUIRE_NOTHROW(bmp = make_unique<binary_message>((::nng_msg*)nullptr));
                REQUIRE_THROWS_AS_MATCHING(s1->try_receive(bmp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_etimedout));
                REQUIRE(bmp->has_one() == false);
            });
        }

        SECTION("Receive non-blocking with no Pipes gives " STRINGIFY(ec_eagain)) {

            buffer_vector_type buf;
            size_type sz = 0;
            // TODO: TBD: this will work for now as a rough cut Exception match...
            REQUIRE_THROWS_AS_MATCHING(s1->try_receive(&buf, sz, flag_nonblock), nng_exception, THROWS_NNG_EXCEPTION(ec_eagain));
            REQUIRE_THAT(buf, Equals(empty_buf));
        }

        SECTION("Send with no Pipes times out correctly") {

            // TODO: TBD: this one is failing: seems to be returning sooner than expected.
            const auto timeout = 100ms;

            RUN_TIMED_SECTION_MILLISECONDS(timeout, [&]() {

                REQUIRE_NOTHROW(s1->set_option(O::send_timeout_duration, timeout));
                // TODO: TBD: this will work for now as a rough cut Exception match...
                REQUIRE_THROWS_AS_MATCHING(s1->send(&empty_buf), nng_exception, THROWS_NNG_EXCEPTION(ec_etimedout));

            });
        }

        SECTION("We can set and get options") {

            const auto timeout = 123ms;
            const auto v = static_cast<int64_t>(0);
            size_type  sz;

            REQUIRE_NOTHROW(s1->set_option(O::send_timeout_duration, timeout));

            SECTION("Read-only options handled properly") {

                // TODO: TBD: this will work for now as a rough cut Exception match...
                REQUIRE_THROWS_AS_MATCHING(s1->set_option_int(O::receive_file_descriptor, 0), nng_exception, THROWS_NNG_EXCEPTION(ec_ereadonly));
                REQUIRE_THROWS_AS_MATCHING(s1->set_option_int(O::send_file_descriptor, 0), nng_exception, THROWS_NNG_EXCEPTION(ec_ereadonly));
                REQUIRE_THROWS_AS_MATCHING(s1->set_option(O::local_address, "a"), nng_exception, THROWS_NNG_EXCEPTION(ec_ereadonly));
            }

            SECTION("Url option works") {

                shared_ptr<listener> lp;
                shared_ptr<dialer> dp;

                REQUIRE_NOTHROW(lp = _session_.create_listener_ep(*s1, url1_addr));
                REQUIRE_NOTHROW(dp = _session_.create_dialer_ep(*s1, url2_addr));

                string url;

                url.resize(max_addr_length);
                REQUIRE_NOTHROW(lp->get_option(O::url, url));
                // Check length as a smoke test, followed by string equality.
                REQUIRE(url.length() == url1_addr.length());
                REQUIRE_THAT(url, Equals(url1_addr, CaseSensitive::Yes));

                // TODO: TBD: this will work for now as a rough cut Exception match...
                REQUIRE_THROWS_AS_MATCHING(lp->set_option(O::url, url), nng_exception, THROWS_NNG_EXCEPTION(ec_ereadonly));

                (url = "").resize(max_addr_length);
                REQUIRE_NOTHROW(dp->get_option(O::url, url));
                // Check length as a smoke test, followed by string equality.
                REQUIRE(url.length() == url2_addr.length());
                REQUIRE_THAT(url, Equals(url2_addr, CaseSensitive::Yes));

                // TODO: TBD: this will work for now as a rough cut Exception match...
                REQUIRE_THROWS_AS_MATCHING(dp->set_option(O::url, url), nng_exception, THROWS_NNG_EXCEPTION(ec_ereadonly));
            }

            SECTION("Bogus Urls not supported") {

                SECTION("Dialing fails properly") {

                    // TODO: TBD: this will work for now as a rough cut Exception match...
                    REQUIRE_THROWS_AS_MATCHING(_session_.create_dialer_ep(*s1, bogus1_addr), nng_exception, THROWS_NNG_EXCEPTION(ec_enotsup));

                    SECTION("Listening fails properly") {

                        // TODO: TBD: this will work for now as a rough cut Exception match...
                        REQUIRE_THROWS_AS_MATCHING(_session_.create_listener_ep(*s1, bogus2_addr), nng_exception, THROWS_NNG_EXCEPTION(ec_enotsup));
                    }
                }
            }

            SECTION("Dialing sync can get refused") {

                // TODO: TBD: this will work for now as a rough cut Exception match...
                REQUIRE_THROWS_AS_MATCHING(s1->dial(no_addr), nng_exception, THROWS_NNG_EXCEPTION(ec_econnrefused));
            }

            SECTION("Dialing async does not get refused") {

                REQUIRE_NOTHROW(s1->dial(asy_addr, flag_nonblock));

                SECTION("And connects late") {

                    shared_ptr<latest_pair_socket> s2;

                    REQUIRE_NOTHROW(s2 = _session_.create_pair_socket());

                    REQUIRE_NOTHROW(s2->listen(asy_addr));

                    // TODO: TBD: had to increase this from 100ms; and then, I'm not sure that was enough, or there might actually be something blocking
                    this_thread::sleep_for(1000ms);

                    REQUIRE_NOTHROW(s1->send(&data_buf, data_buf.size()));

                    /* This is a departure from the original unit testing in which allocation was expected,
                    i.e. was being called with flags = flag_alloc. In this case, we fully expect the client
                    to do the allocation and handling of its own buffers. */

                    sz = 4;
                    buffer_vector_type buf;
                    REQUIRE_NOTHROW(s2->try_receive(&buf, sz));
                    REQUIRE(buf.size() == sz);
                    REQUIRE_THAT(buf, Equals(data_buf));

                    REQUIRE_NOTHROW(_session_.remove_pair_socket(s2.get()));
                }

                SECTION("Listening works") {

                    auto lp = _session_.create_listener_ep();

                    REQUIRE_NOTHROW(s1->listen(here_addr, lp.get()));

                    SECTION("Second Listen fails with " STRINGIFY(ec_eaddrinuse)) {

                        // TODO: TBD: this will work for now as a rough cut Exception match...
                        REQUIRE_THROWS_AS_MATCHING(s1->listen(here_addr), nng_exception, THROWS_NNG_EXCEPTION(ec_eaddrinuse));
                    }

                    SECTION("We cannot try to Start a Listener again") {

                        // TODO: TBD: this will work for now as a rough cut Exception match...
                        REQUIRE_THROWS_AS_MATCHING(lp->start(), nng_exception, THROWS_NNG_EXCEPTION(ec_estate));
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
                        REQUIRE_NOTHROW(dp->set_option_size(O::max_receive_size, value));
                        REQUIRE_NOTHROW(dp->get_option_size(O::max_receive_size, &actual));
                        REQUIRE(actual == expected);
                    }

                    SECTION("Socket options not supported for Dialer") {

                        // Not appropriate for dialer.
                        // TODO: TBD: this will work for now as a rough cut Exception match...
                        REQUIRE_THROWS_AS_MATCHING(dp->set_option_int(O::raw, 1), nng_exception, THROWS_NNG_EXCEPTION(ec_enotsup));
                        REQUIRE_THROWS_AS_MATCHING(dp->set_option_ms(O::min_reconnect_time_duration, 1), nng_exception, THROWS_NNG_EXCEPTION(ec_enotsup));
                    }

                    SECTION("Bad size checks") {

                        // TODO: TBD: this will work for now as a rough cut Exception match...
                        REQUIRE_THROWS_AS_MATCHING(dp->set_option(O::max_receive_size, "a", 1), nng_exception, THROWS_NNG_EXCEPTION(ec_einval));
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
                        REQUIRE_NOTHROW(lp->set_option_size(O::max_receive_size, value));
                        REQUIRE_NOTHROW(lp->get_option_size(O::max_receive_size, &actual));
                        REQUIRE(actual == expected);
                    }

                    SECTION("Socket options not supported for Listener") {

                        // Not appropriate for dialer.
                        // TODO: TBD: this will work for now as a rough cut Exception match...
                        REQUIRE_THROWS_AS_MATCHING(lp->set_option_int(O::raw, 1), nng_exception, THROWS_NNG_EXCEPTION(ec_enotsup));
                        REQUIRE_THROWS_AS_MATCHING(lp->set_option_ms(O::min_reconnect_time_duration, 1), nng_exception, THROWS_NNG_EXCEPTION(ec_enotsup));
                    }

                    SECTION("Bad size checks") {

                        // TODO: TBD: this will work for now as a rough cut Exception match...
                        REQUIRE_THROWS_AS_MATCHING(lp->set_option(O::max_receive_size, "a", 1), nng_exception, THROWS_NNG_EXCEPTION(ec_einval));
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
                    // TODO: TBD: this will work for now as a rough cut Exception match...
                    REQUIRE_THROWS_AS_MATCHING(l.set_option_size(O::max_receive_size, 0), nng_exception, THROWS_NNG_EXCEPTION(ec_enoent));

                    int value;
                    size_type sz = 1;

                    // TODO: TBD: this will work for now as a rough cut Exception match...
                    REQUIRE_THROWS_AS_MATCHING(l.get_option(O::raw, (void*)&value, &sz), nng_exception, THROWS_NNG_EXCEPTION(ec_enoent));
                    REQUIRE_THROWS_AS_MATCHING(l.get_option_size(O::max_receive_size, &sz), nng_exception, THROWS_NNG_EXCEPTION(ec_enoent));
                    REQUIRE_THROWS_AS_MATCHING(l.get_option_int(O::raw, &value), nng_exception, THROWS_NNG_EXCEPTION(ec_enoent));

                    duration_type timeout;

                    REQUIRE_THROWS_AS_MATCHING(l.get_option(O::linger_duration, &timeout), nng_exception, THROWS_NNG_EXCEPTION(ec_enoent));
                }

                SECTION("Cannot access absent Dialer Endpoint options") {

                    // Setup an "absent" Dialer Endpoint.
                    dialer d;

                    // TODO: TBD: a lot of gets here, but only one set?
                    // TODO: TBD: this will work for now as a rough cut Exception match...
                    REQUIRE_THROWS_AS_MATCHING(d.set_option_size(O::max_receive_size, 0), nng_exception, THROWS_NNG_EXCEPTION(ec_enoent));

                    int value;
                    size_t sz = 1;

                    REQUIRE_THROWS_AS_MATCHING(d.get_option(O::raw, (void*)&value, &sz), nng_exception, THROWS_NNG_EXCEPTION(ec_enoent));
                    REQUIRE_THROWS_AS_MATCHING(d.get_option_size(O::max_receive_size, &sz), nng_exception, THROWS_NNG_EXCEPTION(ec_enoent));
                    REQUIRE_THROWS_AS_MATCHING(d.get_option_int(O::raw, &value), nng_exception, THROWS_NNG_EXCEPTION(ec_enoent));

                    duration_type timeout;

                    REQUIRE_THROWS_AS_MATCHING(d.get_option(O::linger_duration, &timeout), nng_exception, THROWS_NNG_EXCEPTION(ec_enoent));
                }

                SECTION("We can send and receive messages") {

                    int length;
                    const auto timeout = 3000ms;

                    shared_ptr<latest_pair_socket> s2;

                    REQUIRE_NOTHROW(s2 = _session_.create_pair_socket());

                    REQUIRE_NOTHROW(s1->set_option_int(O::receive_buffer, 1));
                    REQUIRE_NOTHROW(s1->get_option_int(O::receive_buffer, &length));
                    REQUIRE(length == 1);

                    REQUIRE_NOTHROW(s1->set_option_int(O::send_buffer, 1));
                    REQUIRE_NOTHROW(s2->set_option_int(O::send_buffer, 1));

                    REQUIRE_NOTHROW(s1->set_option(O::send_timeout_duration, timeout));
                    REQUIRE_NOTHROW(s1->set_option(O::receive_timeout_duration, timeout));
                    REQUIRE_NOTHROW(s2->set_option(O::send_timeout_duration, timeout));
                    REQUIRE_NOTHROW(s2->set_option(O::receive_timeout_duration, timeout));

                    REQUIRE_NOTHROW(s1->listen(t1_addr));
                    REQUIRE_NOTHROW(s2->dial(t1_addr));

                    size_t sz = 4;
                    const auto expected = data_buf;

                    REQUIRE_NOTHROW(s1->send(&data_buf, sz));

                    buffer_vector_type actual;
                    actual.resize(sz);

                    REQUIRE_NOTHROW(s2->try_receive(&actual, sz));

                    //REQUIRE(actual.size() == sz);
                    REQUIRE_THAT(actual, Equals(expected));

                    REQUIRE_NOTHROW(_session_.remove_pair_socket(s2.get()));
                }
            }
        }
    }
}
