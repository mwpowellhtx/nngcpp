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
#include <thread>
#include <functional>

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

        // TODO: TBD: listen here? or just prior to entering the thread?
        static void server_callback(std::shared_ptr<protocol::latest_rep_socket> sp) {

            using namespace constants;

            //// TODO: TBD: Catch v1 is apparently known thread-unsafe; will be interested to know if/when v2 will be ready for prime time
            //REQUIRE_NOTHROW(sp->listen(atscale_addr));
            //// TODO: TBD: in the meanwhile, it may be interested to consider alternatives...
            //// https://github.com/ned14/quickcpplib/blob/master/include/boost/test/unit_test.hpp
            //// https://ned14.github.io/quickcpplib/hierarchy.html
            sp->listen(atscale_addr);
        }

        void init() {

            using _opt_ = option_names;

            INFO("Setting up server reply socket.");

            REQUIRE_NOTHROW(_repsp = _sessionp->create_rep_socket());

            CHECK(_repsp.get() != nullptr);

            REQUIRE_NOTHROW(_repsp->set_option_int(_opt_::receive_buffer, 256));
            REQUIRE_NOTHROW(_repsp->set_option_int(_opt_::send_buffer, 256));

            using namespace std::placeholders;

            _server_threadp = std::make_unique<std::thread>(
                std::bind(&server_callback, _1), _repsp);
        }

    public:

        server_simulation(session* const sessionp)
            : _sessionp(sessionp), _repsp(), _server_threadp() {
            init();
        }

        ~server_simulation() {
            _server_threadp->join();
            _sessionp->remove_rep_socket(_repsp.get());
        }
    };
}

//static int nclients = 200;

nng_socket   rep;
void *       server;

void
serve(void *arg)
{
	nng_msg *msg;

	for (;;) {
		msg = NULL;
		if ((nng_recvmsg(rep, &msg, 0) != 0) ||
		    (nng_sendmsg(rep, msg, 0) != 0)) {
			break;
		}
	}
	if (msg != NULL) {
		nng_msg_free(msg);
	}
	nng_close(rep);
}

void
stop(void)
{
	nng_closeall();
	nng_thread_destroy(server);
	//nng_fini();
}

int
openclients(nng_socket *clients, int num)
{
	int      rv;
	int      i;
	uint64_t t;
	for (i = 0; i < num; i++) {
		t = 100000; // 100ms
		nng_socket c;
		if (((rv = nng_req_open(&c)) != 0) ||
		    ((rv = nng_setopt_usec(c, NNG_OPT_RECVTIMEO, t)) != 0) ||
		    ((rv = nng_setopt_usec(c, NNG_OPT_SENDTIMEO, t)) != 0) ||
		    ((rv = nng_dial(c, addr, NULL, 0)) != 0)) {
			return (rv);
		}
		clients[i] = c;
	}
	return (0);
}

int
transact(nng_socket *clients, int num)
{
	nng_msg *msg;
	int      rv;
	int      i;

	for (i = 0; i < num; i++) {

		if (((rv = nng_msg_alloc(&msg, 0)) != 0) ||
		    ((rv = nng_sendmsg(clients[i], msg, 0)) != 0) ||
		    ((rv = nng_recvmsg(clients[i], &msg, 0)) != 0)) {
			// We may leak a message, but this is an
			// error case anyway.
			break;
		}
		nng_msg_free(msg);
		msg = NULL;
	}
	return (rv);
}

TEST_CASE("Test that messaging scales", "[scalability]") {

    using namespace std;
    using namespace nng;
    using namespace nng::protocol;

    session _session_;
    server_simulation _sim_(&_session_);

    nng_socket *clients;
	int *       results;

	atexit(stop);

	clients = calloc(nclients, sizeof(nng_socket));
	results = calloc(nclients, sizeof(int));

    //// TODO: TBD: all this is pretty much taken care of on the front end of the Server Simulation during its ctor.
	//if (/*(nng_rep_open(&rep) != 0) ||*/
	//    /*(nng_setopt_int(rep, NNG_OPT_RECVBUF, 256) != 0) ||*/
	//    /*(nng_setopt_int(rep, NNG_OPT_SENDBUF, 256) != 0) ||*/
	//    /*(nng_listen(rep, addr, NULL, 0) != 0) ||*/
	//    (nng_thread_create(&server, serve, NULL) != 0)) {
	//	fprintf(stderr, "Unable to set up server!\n");
	//	exit(1);
	//}

	Test("Scalability", {
		Convey("We can handle many many clients", {
			int i;
			So(openclients(clients, nclients) == 0);
			So(transact(clients, nclients) == 0);
			for (i = 0; i < nclients; i++) {
				So(nng_close(clients[i]) == 0);
			}
		});
	});

	free(clients);
	free(results);
});
