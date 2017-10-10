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

#include "../catch/catch_exception_translations.hpp"

#include <nngcpp.h>

#include <string>
#include <thread>
#include <functional>
#include <algorithm>
#include <chrono>
#include <ostream>
#include <atomic>

#include "../catch/catch_macros.hpp"

namespace constants {
    const std::string atscale_addr = "inproc:///atscale";
}

namespace nng {
    struct server_simulation {
    private:

        session* const _sessionp;

        std::shared_ptr<protocol::latest_rep_socket> _repsp;

        std::unique_ptr<std::thread> _server_threadp;

        std::atomic<bool> _stop_server;

        // TODO: TBD: listen here? or just prior to entering the thread?
        void server_callback(std::shared_ptr<protocol::latest_rep_socket> sp) {

            using namespace std;
            using namespace protocol;

            //INFO("Server: started...");

            while (!_stop_server) {

                string r;
                latest_rep_socket::receive_size_type sz = 128;

                //INFO("Server: receiving...");

                REQUIRE_NOTHROW(sp->try_receive(r, sz));

                //INFO("Server: received '" + r + "'");

                const auto s = r;

                //INFO("Server: sending '" + s + "'");

                REQUIRE_NOTHROW(sp->send(s));

                //INFO("Server: sent.");
            }

            //INFO("Server: stopped.");
        }

        void init() {

            _stop_server = false;

            using _opt_ = option_names;

            //INFO("Setting up server reply socket.");

            REQUIRE_NOTHROW(_repsp = _sessionp->create_rep_socket());

            CHECK(_repsp.get() != nullptr);

            REQUIRE_NOTHROW(_repsp->set_option_int(_opt_::receive_buffer, 256));
            REQUIRE_NOTHROW(_repsp->set_option_int(_opt_::send_buffer, 256));

            using namespace std::placeholders;
            using namespace constants;

            //// TODO: TBD: Catch v1 is apparently known thread-unsafe; will be interested to know if/when v2 will be ready for prime time
            //REQUIRE_NOTHROW(sp->listen(atscale_addr));
            //// TODO: TBD: in the meanwhile, it may be interested to consider alternatives...
            //// https://github.com/ned14/quickcpplib/blob/master/include/boost/test/unit_test.hpp
            //// https://ned14.github.io/quickcpplib/hierarchy.html
            _repsp->listen(atscale_addr);

            _server_threadp = std::make_unique<std::thread>(std::bind(
                &server_simulation::server_callback, this, _1), _repsp);
        }

    public:

        server_simulation(session* const sessionp)
            : _sessionp(sessionp), _repsp(), _server_threadp() {
            init();
        }

        ~server_simulation() {
            // TODO: requires to signal the thread?
            //INFO("Server: stopping...");
            _stop_server = true;
            //INFO("Server: thread rejoining...");
            _server_threadp->join();
            //INFO("Server: thread rejoined.");
            _sessionp->remove_rep_socket(_repsp.get());
        }
    };
}

#define SOCKET_COUNT 200

typedef std::vector<std::shared_ptr<nng::protocol::latest_req_socket>> req_socket_vector;

req_socket_vector create_and_dial_clients(nng::session& session, int count = SOCKET_COUNT) {

    using namespace std;
    using namespace nng;
    using namespace nng::protocol;
    using namespace constants;
    using _opt_ = option_names;

    static const auto timeout = 100ms;

    req_socket_vector sockets;

    while (count--) {

        shared_ptr<latest_req_socket> sp;

        REQUIRE_NOTHROW(sp = session.create_req_socket());
        REQUIRE(sp.get() != nullptr);

        REQUIRE_NOTHROW(sp->set_option_usec(_opt_::receive_timeout_usec, timeout.count()));
        REQUIRE_NOTHROW(sp->set_option_usec(_opt_::send_timeout_usec, timeout.count()));

        REQUIRE_NOTHROW(sp->dial(atscale_addr));

        sockets.push_back(sp);
    }

    return sockets;
}

bool transact_clients(req_socket_vector& sockets) {

    using namespace std;
    using namespace nng;
    using namespace nng::protocol;

    auto value = static_cast<int>(1);

    const auto get_next_message = [&]() {
        ostringstream os;
        os << ++value;
        return string(os.str());
    };

    for_each(sockets.begin(), sockets.end(), [&](shared_ptr<latest_req_socket> sp) {

        // TODO: TBD: um, okay... this seems like it should be a stronger error then: i.e. REQUIRE_NOTHROW, instead of simply a "break"
        // We may leak a message, but this is an error case anyway.
        const auto s = get_next_message(), expected = s;

        //INFO("Client: sending '" + s + "'");

        REQUIRE_NOTHROW(sp->send(s));

        //INFO("Client: sent '" + s + "'");

        string r;
        auto sz = s.size();

        //INFO("Client: receiving...");

        REQUIRE_NOTHROW(sp->try_receive(r, sz));

        //INFO("Client: received '" + r + "'");
    });

    return true;
}

TEST_CASE("Test that messaging scales", "[scalability]") {

    using namespace std;
    using namespace nng;
    using namespace nng::protocol;

    session _session_;
    server_simulation _sim_(&_session_);

    // Because of the nature of SECTION, I'm not sure this is quite the right model for this test.
    //INFO("We can handle many, many clients.");

    {
        auto sockets = create_and_dial_clients(_session_);

        REQUIRE(sockets.size() == SOCKET_COUNT);

        transact_clients(sockets);

        for (auto& sp : sockets) {

            _session_.remove_req_socket(sp.get());

            remove_if(sockets.begin(), sockets.end(), [&sp](req_socket_vector::value_type& x) {
                return x.get() == sp.get();
            });
        }
    }
}
