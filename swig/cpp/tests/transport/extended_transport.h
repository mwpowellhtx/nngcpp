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

#ifndef NNGCPP_TESTS_EXTENDED_TRANSPORT_TESTS_H
#define NNGCPP_TESTS_EXTENDED_TRANSPORT_TESTS_H

#include <nngcpp.h>

#include "transport.h"

namespace constants {
    extern const std::string props;
    extern const nng::messaging::buffer_vector_type props_buf;
}

namespace nng {

    namespace messaging {

        // Parameters should be as fully vetted as possible prior to presentation to the extended tests function.
        template<typename Pipe_, typename Listener_, typename Dialer_>
        void test_local_addr_properties(Pipe_* const pp, Listener_* const lp
            , Dialer_* const dp, uint16_t expected_port) {}

        template<typename Pipe_, typename Listener_, typename Dialer_>
        void test_remote_addr_properties(Pipe_* const pp, Listener_* const lp
            , Dialer_* const dp, uint16_t expected_port) {}

        template<>
        extern void test_local_addr_properties<::nng_pipe, ::nng_listener, ::nng_dialer>(::nng_pipe* const pp
            , ::nng_listener* const lp, ::nng_dialer* const dp, uint16_t expected_port);

        template<>
        extern void test_remote_addr_properties<::nng_pipe, ::nng_listener, ::nng_dialer>(::nng_pipe* const pp
            , ::nng_listener* const lp, ::nng_dialer* const dp, uint16_t expected_port);

        template<>
        extern void test_local_addr_properties<message_pipe, listener, dialer>(message_pipe* const pp
            , listener* const lp, dialer* const dp, uint16_t expected_port);

        template<>
        extern void test_remote_addr_properties<message_pipe, listener, dialer>(message_pipe* const pp
            , listener* const lp, dialer* const dp, uint16_t expected_port);
    }
}

#endif // NNGCPP_TESTS_EXTENDED_TRANSPORT_TESTS_H
