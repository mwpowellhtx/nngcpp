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
#include "../catch/catch_tags.h"

#include "../helpers/basic_fixture.h"
#include "../helpers/constants.h"
#include "../helpers/protocol_boilerplate.hpp"

DEFINE_SOCKET_FIXTURE(v1, pair_socket_fixture, pair_socket)

// TODO: TBD: update this module: follow the C API grouping more or less closely, adapting for C+ wrapperisms
// TODO: TBD: generally, update this taking into consideration updates; could "transaction" be useful now, in more broad sense? i.e. looking at constants::to_str for instance...

#include <cctype>
#include <memory>
#include <algorithm>

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

    const nng::buffer_vector_type empty_buf = {};
    const auto data_buf = to_buffer("data");
}

// TODO: TBD: instead of "sock", this should perhaps go in "pair" ...

TEST_CASE("Verify protocol and peer are correct", Catch::Tags("pair"
    , "v1", "protocol", "peer", "internal", "nng", "cxx").c_str()) {

    using namespace std;
    using namespace nng;
    using namespace nng::protocol::v1;

    protocol_type actual;
    basic_fixture fixture;

    // Yes, we should still verify that Socket construction succeeds.
    unique_ptr<pair_socket_fixture> sp;

    SECTION("Given the created socket") {

        REQUIRE_NOTHROW(sp = make_unique<pair_socket_fixture>());
        REQUIRE(sp.get() != nullptr);

        SECTION("Requires protocol exposure") {
            REQUIRE_NOTHROW(sp->get_protocol());
        }

        SECTION("Requires peer exposure") {
            REQUIRE_NOTHROW(sp->get_peer());
        }

        SECTION("Verify protocol is correct") {
            actual = sp->get_protocol();
            REQUIRE(actual == proto_pair_v1);
            REQUIRE(actual == proto_pair);
        }

        SECTION("Verify peer is correct") {
            actual = sp->get_peer();
            REQUIRE(actual == proto_pair_v1);
            REQUIRE(actual == proto_pair);
        }
    }
}

TEST_CASE("Socket Operations", "[socket][operations][ngg][cxx]") {

    using namespace std;
    using namespace std::chrono;
    using namespace nng;
    using namespace nng::protocol;
    using namespace nng::exceptions;
    using namespace constants;
    using namespace Catch;
    using namespace Catch::Matchers;
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

        //// TODO: TBD: the same section in the C code was removed, but I wonder if we call Close and expect the same behavior?
        //// TODO: TBD: see: https://github.com/nanomsg/nng/issues/136 and follow up comments
        //// TODO: TBD: Call Close instead of Shutdown and expect the same behavior?
        //SECTION("And we can shut it down") {

        //    REQUIRE_NOTHROW(s1->Shutdown());
        //    // TODO: TBD: this will work for now as a rough cut Exception match...
        //    REQUIRE_THROWS_AS_MATCHING(s1->Shutdown(), nng_exception, THROWS_NNG_EXCEPTION(ec_eclosed));

        //    SECTION("It cannot receive") {

        //        size_type sz = 0;
        //        buffer_vector_type buf;

        //        // TODO: TBD: was using flag_alloc
        //        // TODO: TBD: this will work for now as a rough cut Exception match...
        //        REQUIRE_THROWS_AS_MATCHING(s1->TryReceive(&buf, sz), nng_exception, THROWS_NNG_EXCEPTION(ec_eclosed));
        //    }

        //    SECTION("It cannot send") {

        //        // TODO: TBD: this will work for now as a rough cut Exception match...
        //        REQUIRE_THROWS_AS_MATCHING(s1->Send(empty_buf), nng_exception, THROWS_NNG_EXCEPTION(ec_eclosed));
        //    }

        //    SECTION("Cannot create Endpoints based on Socket") {

        //        SECTION("Session cannot create Dialer given Socket and Address") {

        //            // Do not keep track of these pointers.
        //            // TODO: TBD: this will work for now as a rough cut Exception match...
        //            REQUIRE_THROWS_AS_MATCHING(_session_.create_dialer_ep(*s1, closed_addr), nng_exception, THROWS_NNG_EXCEPTION(ec_eclosed));
        //        }

        //        SECTION("Session cannot create Listener given Socket and Address") {

        //            // Do not keep track of these pointers.
        //            // TODO: TBD: this will work for now as a rough cut Exception match...
        //            REQUIRE_THROWS_AS_MATCHING(_session_.create_listener_ep(*s1, closed_addr), nng_exception, THROWS_NNG_EXCEPTION(ec_eclosed));
        //        }

        //        SECTION("Cannot Dial Socket and receive Dialer back") {

        //            auto dp = _session_.create_dialer_ep();
        //            REQUIRE(dp != nullptr);
        //            // TODO: TBD: this will work for now as a rough cut Exception match...
        //            REQUIRE_THROWS_AS_MATCHING(s1->Dial(closed_addr, dp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_eclosed));
        //            REQUIRE_NOTHROW(_session_.remove_dialer_ep(dp.get()));
        //        }

        //        SECTION("Cannot Listen Socket and receive Listener back") {

        //            auto lp = _session_.create_listener_ep();
        //            REQUIRE(lp != nullptr);
        //            // TODO: TBD: this will work for now as a rough cut Exception match...
        //            REQUIRE_THROWS_AS_MATCHING(s1->Listen(closed_addr, lp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_eclosed));
        //            REQUIRE_NOTHROW(_session_.remove_listener_ep(lp.get()));
        //        }
        //    }
        //}

        SECTION("Receive with no Pipes times out correctly") {

            const auto timeout = 100ms;

            RUN_TIMED_SECTION_MILLISECONDS(timeout, [&]() {

                REQUIRE_NOTHROW(s1->GetOptions()->SetDuration(O::recv_timeout_duration, timeout));

                unique_ptr<binary_message> bmp;

                REQUIRE_NOTHROW(bmp = make_unique<binary_message>((::nng_msg*)nullptr));
                REQUIRE_THROWS_AS_MATCHING(s1->TryReceive(bmp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_etimedout));
                REQUIRE(bmp->HasOne() == false);
            });
        }

        SECTION("Receive non-blocking with no Pipes gives " STRINGIFY(ec_eagain)) {

            buffer_vector_type buf;
            size_type sz = 0;
            // TODO: TBD: this will work for now as a rough cut Exception match...
            REQUIRE_THROWS_AS_MATCHING(s1->TryReceive(&buf, sz, flag_nonblock), nng_exception, THROWS_NNG_EXCEPTION(ec_eagain));
            REQUIRE_THAT(buf, Equals(empty_buf));
        }

        SECTION("Send with no Pipes times out correctly") {

            // TODO: TBD: this one is failing: seems to be returning sooner than expected.
            const auto timeout = 100ms;

            RUN_TIMED_SECTION_MILLISECONDS(timeout, [&]() {

                REQUIRE_NOTHROW(s1->GetOptions()->SetDuration(O::send_timeout_duration, timeout));
                // TODO: TBD: this will work for now as a rough cut Exception match...
                REQUIRE_THROWS_AS_MATCHING(s1->Send(empty_buf), nng_exception, THROWS_NNG_EXCEPTION(ec_etimedout));

            });
        }

        SECTION("We can set and get options") {

            const auto timeout = 123ms;
            const auto v = static_cast<int64_t>(0);
            size_type  sz;

            REQUIRE_NOTHROW(s1->GetOptions()->SetDuration(O::send_timeout_duration, timeout));

            SECTION("Read-only options handled properly") {

                // TODO: TBD: this will work for now as a rough cut Exception match...
                REQUIRE_THROWS_AS_MATCHING(s1->GetOptions()->SetInt32(O::recv_fd, 0), nng_exception, THROWS_NNG_EXCEPTION(ec_ereadonly));
                REQUIRE_THROWS_AS_MATCHING(s1->GetOptions()->SetInt32(O::send_fd, 0), nng_exception, THROWS_NNG_EXCEPTION(ec_ereadonly));
                REQUIRE_THROWS_AS_MATCHING(s1->GetOptions()->SetString(O::local_addr, "a"), nng_exception, THROWS_NNG_EXCEPTION(ec_ereadonly));
            }

            SECTION("Url option works") {

                shared_ptr<listener> lp;
                shared_ptr<dialer> dp;

                REQUIRE_NOTHROW(lp = _session_.create_listener_ep(*s1, url1_addr));
                REQUIRE_NOTHROW(dp = _session_.create_dialer_ep(*s1, url2_addr));

                string url;

                url.resize(max_addr_length);
                REQUIRE_NOTHROW(url = lp->GetOptions()->GetText(O::url));
                REQUIRE_THAT(url, Equals(url1_addr, CaseSensitive::Yes));

                // TODO: TBD: this will work for now as a rough cut Exception match...
                REQUIRE_THROWS_AS_MATCHING(lp->GetOptions()->SetString(O::url, url), nng_exception, THROWS_NNG_EXCEPTION(ec_ereadonly));

                (url = "").resize(max_addr_length);
                REQUIRE_NOTHROW(url = dp->GetOptions()->GetText(O::url));
                // Check length as a smoke test, followed by string equality.
                REQUIRE(url.length() == url2_addr.length());
                REQUIRE_THAT(url, Equals(url2_addr, CaseSensitive::Yes));

                // TODO: TBD: this will work for now as a rough cut Exception match...
                REQUIRE_THROWS_AS_MATCHING(dp->GetOptions()->SetString(O::url, url), nng_exception, THROWS_NNG_EXCEPTION(ec_ereadonly));
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
                REQUIRE_THROWS_AS_MATCHING(s1->Dial(no_addr), nng_exception, THROWS_NNG_EXCEPTION(ec_econnrefused));
            }

            SECTION("Dialing async does not get refused") {

                REQUIRE_NOTHROW(s1->Dial(asy_addr, flag_nonblock));

                SECTION("And connects late") {

                    shared_ptr<latest_pair_socket> s2;

                    REQUIRE_NOTHROW(s2 = _session_.create_pair_socket());

                    REQUIRE_NOTHROW(s2->Listen(asy_addr));

                    // TODO: TBD: had to increase this from 100ms; and then, I'm not sure that was enough, or there might actually be something blocking
                    this_thread::sleep_for(1000ms);

                    REQUIRE_NOTHROW(s1->Send(data_buf, data_buf.size()));

                    /* This is a departure from the original unit testing in which allocation was expected,
                    i.e. was being called with flags = flag_alloc. In this case, we fully expect the client
                    to do the allocation and handling of its own buffers. */

                    sz = 4;
                    buffer_vector_type buf;
                    REQUIRE_NOTHROW(s2->TryReceive(&buf, sz));
                    REQUIRE(buf.size() == sz);
                    REQUIRE_THAT(buf, Equals(data_buf));

                    REQUIRE_NOTHROW(_session_.remove_pair_socket(s2.get()));
                }

                SECTION("Listening works") {

                    auto lp = _session_.create_listener_ep();

                    REQUIRE_NOTHROW(s1->Listen(here_addr, lp.get()));

                    SECTION("Second Listen fails with " STRINGIFY(ec_eaddrinuse)) {

                        // TODO: TBD: this will work for now as a rough cut Exception match...
                        REQUIRE_THROWS_AS_MATCHING(s1->Listen(here_addr), nng_exception, THROWS_NNG_EXCEPTION(ec_eaddrinuse));
                    }

                    SECTION("We cannot try to Start a Listener again") {

                        // TODO: TBD: this will work for now as a rough cut Exception match...
                        REQUIRE_THROWS_AS_MATCHING(lp->Start(), nng_exception, THROWS_NNG_EXCEPTION(ec_estate));
                    }

                    SECTION("We can connect to it") {

                        shared_ptr<latest_pair_socket> s2;

                        REQUIRE_NOTHROW(s2 = _session_.create_pair_socket());

                        REQUIRE_NOTHROW(s2->Dial(here_addr));

                        REQUIRE_NOTHROW(_session_.remove_pair_socket(s2.get()));
                    }
                }

                SECTION("Dialer creation ok") {

                    shared_ptr<dialer> dp;

                    REQUIRE_NOTHROW(dp = _session_.create_dialer_ep(*s1, loopback_addr));

                    SECTION("Options work") {

                        size_t actual;
                        const size_t value = 4321, expected = value;
                        REQUIRE_NOTHROW(dp->GetOptions()->SetSize(O::max_recv_sz, value));
                        REQUIRE_NOTHROW(actual = dp->GetOptions()->GetSize(O::max_recv_sz));
                        REQUIRE(actual == expected);
                    }

                    SECTION("Socket options not supported for Dialer") {

                        // Not appropriate for dialer.
                        // TODO: TBD: this will work for now as a rough cut Exception match...
                        REQUIRE_THROWS_AS_MATCHING(dp->GetOptions()->SetInt32(O::raw, 1), nng_exception, THROWS_NNG_EXCEPTION(ec_enotsup));
                        REQUIRE_THROWS_AS_MATCHING(dp->GetOptions()->SetMilliseconds(O::min_reconnect_duration, 1), nng_exception, THROWS_NNG_EXCEPTION(ec_enotsup));
                    }

                    SECTION("Bad size checks") {

                        // TODO: TBD: this will work for now as a rough cut Exception match...
                        REQUIRE_THROWS_AS_MATCHING(dp->GetOptions()->set(O::max_recv_sz, "a", 1), nng_exception, THROWS_NNG_EXCEPTION(ec_einval));
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
                        REQUIRE_NOTHROW(lp->GetOptions()->SetSize(O::max_recv_sz, value));
                        REQUIRE_NOTHROW(actual = lp->GetOptions()->GetSize(O::max_recv_sz));
                        REQUIRE(actual == expected);
                    }

                    SECTION("Socket options not supported for Listener") {

                        // Not appropriate for dialer.
                        // TODO: TBD: this will work for now as a rough cut Exception match...
                        REQUIRE_THROWS_AS_MATCHING(lp->GetOptions()->SetInt32(O::raw, 1), nng_exception, THROWS_NNG_EXCEPTION(ec_enotsup));
                        REQUIRE_THROWS_AS_MATCHING(lp->GetOptions()->SetMilliseconds(O::min_reconnect_duration, 1), nng_exception, THROWS_NNG_EXCEPTION(ec_enotsup));
                    }

                    SECTION("Bad size checks") {

                        // TODO: TBD: this will work for now as a rough cut Exception match...
                        REQUIRE_THROWS_AS_MATCHING(lp->GetOptions()->set(O::max_recv_sz, "a", 1), nng_exception, THROWS_NNG_EXCEPTION(ec_einval));
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
                    REQUIRE_THROWS_AS_MATCHING(l.GetOptions()->SetSize(O::max_recv_sz, 0), nng_exception, THROWS_NNG_EXCEPTION(ec_enoent));

                    int value;
                    size_type sz = 1;

                    // TODO: TBD: this will work for now as a rough cut Exception match...
                    REQUIRE_THROWS_AS_MATCHING(l.GetOptions()->get(O::raw, (void*)&value, sz), nng_exception, THROWS_NNG_EXCEPTION(ec_enoent));
                    REQUIRE_THROWS_AS_MATCHING(l.GetOptions()->GetSize(O::max_recv_sz), nng_exception, THROWS_NNG_EXCEPTION(ec_enoent));
                    REQUIRE_THROWS_AS_MATCHING(l.GetOptions()->GetInt32(O::raw), nng_exception, THROWS_NNG_EXCEPTION(ec_enoent));

                    REQUIRE_THROWS_AS_MATCHING(l.GetOptions()->GetDuration(O::linger_duration), nng_exception, THROWS_NNG_EXCEPTION(ec_enoent));
                }

                SECTION("Cannot access absent Dialer Endpoint options") {

                    // Setup an "absent" Dialer Endpoint.
                    dialer d;

                    // TODO: TBD: a lot of gets here, but only one set?
                    // TODO: TBD: this will work for now as a rough cut Exception match...
                    REQUIRE_THROWS_AS_MATCHING(d.GetOptions()->SetSize(O::max_recv_sz, 0), nng_exception, THROWS_NNG_EXCEPTION(ec_enoent));

                    int value;
                    size_t sz = 1;

                    REQUIRE_THROWS_AS_MATCHING(d.GetOptions()->get(O::raw, (void*)&value, sz), nng_exception, THROWS_NNG_EXCEPTION(ec_enoent));
                    REQUIRE_THROWS_AS_MATCHING(d.GetOptions()->GetSize(O::max_recv_sz), nng_exception, THROWS_NNG_EXCEPTION(ec_enoent));
                    REQUIRE_THROWS_AS_MATCHING(d.GetOptions()->GetInt32(O::raw), nng_exception, THROWS_NNG_EXCEPTION(ec_enoent));

                    REQUIRE_THROWS_AS_MATCHING(d.GetOptions()->GetDuration(O::linger_duration), nng_exception, THROWS_NNG_EXCEPTION(ec_enoent));
                }

                SECTION("We can send and receive messages") {

                    int length;
                    const auto timeout = 3000ms;

                    shared_ptr<latest_pair_socket> s2;

                    REQUIRE_NOTHROW(s2 = _session_.create_pair_socket());

                    REQUIRE_NOTHROW(s1->GetOptions()->SetInt32(O::recv_buf, 1));
                    REQUIRE_NOTHROW(length = s1->GetOptions()->GetInt32(O::recv_buf));
                    REQUIRE(length == 1);

                    REQUIRE_NOTHROW(s1->GetOptions()->SetInt32(O::send_buf, 1));
                    REQUIRE_NOTHROW(s2->GetOptions()->SetInt32(O::send_buf, 1));

                    REQUIRE_NOTHROW(s1->GetOptions()->SetDuration(O::send_timeout_duration, timeout));
                    REQUIRE_NOTHROW(s1->GetOptions()->SetDuration(O::recv_timeout_duration, timeout));
                    REQUIRE_NOTHROW(s2->GetOptions()->SetDuration(O::send_timeout_duration, timeout));
                    REQUIRE_NOTHROW(s2->GetOptions()->SetDuration(O::recv_timeout_duration, timeout));

                    REQUIRE_NOTHROW(s1->Listen(t1_addr));
                    REQUIRE_NOTHROW(s2->Dial(t1_addr));

                    size_t sz = 4;
                    const auto expected = data_buf;

                    REQUIRE_NOTHROW(s1->Send(data_buf, sz));

                    buffer_vector_type actual;
                    actual.resize(sz);

                    REQUIRE_NOTHROW(s2->TryReceive(&actual, sz));

                    //REQUIRE(actual.size() == sz);
                    REQUIRE_THAT(actual, Equals(expected));

                    REQUIRE_NOTHROW(_session_.remove_pair_socket(s2.get()));
                }
            }
        }
    }
}
