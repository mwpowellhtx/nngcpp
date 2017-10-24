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

#include "transport.h"

#include "../catch/catch_exception_matcher_base.hpp"
#include "../catch/catch_exception_translations.hpp"
#include "../catch/catch_tags.h"
#include "../helpers/constants.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <core/aio.h>
#include <core/transport.h>

    extern ::nni_tran* nni_tran_find(const char *);

#ifdef __cplusplus
}
#endif // __cplusplus

void init(const std::string& addr) {
    WARN("Initializing for address: '" + addr + "'");
    REQUIRE(::nni_tran_find(addr.c_str()));
}

namespace constants {

    const std::vector<std::string> prefix_tags = { "ipc" };

    // TODO: TBD: I wonder if this ultimately refers to actually /tmp/nng_ipc_test_ in the file path?
    // TODO: TBD: that would be on Linux, I'm assuming; what about on Windows? is the implementation "there" yet?
    //const std::string test_addr_base = "ipc:///tmp/nng_ipc_test_";
    const std::string test_addr_base = "ipc:///tmp/nng_ipc_test_";

    const char port_delim = '\0';
}

TEST_CASE("Simple C style program to open reqrep IPC and send", Catch::Tags(
    "ipc", "req", "rep", "send", "recv", "msg", "simple", "c").c_str()) {

    using namespace std;
    using namespace nng;

    ::nng_dialer d;
    ::nng_listener l;
    ::nng_socket s1, s2;

    unique_ptr<binary_message> bmp;

    const std::string addr = "ipc:///tmp/somewhere";

    REQUIRE(::nng_pair_open(&s1) == 0);
    REQUIRE(s1);

    REQUIRE(::nng_pair_open(&s2) == 0);
    REQUIRE(s2);

    // TODO: TBD: listeners and dialers are "created", but never "closed"? what happens when they are closed in front of parent sockets?
    REQUIRE(::nng_listener_create(&l, s1, addr.c_str()) == 0);
    REQUIRE(l);

    REQUIRE(::nng_dialer_create(&d, s2, addr.c_str()) == 0);
    REQUIRE(d);

    REQUIRE(::nng_listener_start(l, 0) == 0);
    REQUIRE(::nng_dialer_start(d, 0) == 0);

    // Using a hybrid approach to testing since I have confidence that messages are well supported.
    REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
    REQUIRE_NOTHROW(*bmp << "hello");

    REQUIRE(::nng_sendmsg(s1, bmp->get_msgp(), 0) == 0);
    // On Sent is signaled during Socket::Send operations upon transfer of message ownership to NNG API.
    REQUIRE_NOTHROW(bmp->on_sent());

    ::nng_msg* msgp;
    REQUIRE(::nng_recvmsg(s2, &msgp, 0) == 0);
    REQUIRE(msgp);
    REQUIRE_NOTHROW(bmp->set_msgp(msgp));

    REQUIRE(::nng_close(s1) == 0);
    REQUIRE(::nng_close(s2) == 0);

    REQUIRE_NOTHROW(bmp.reset());
}
