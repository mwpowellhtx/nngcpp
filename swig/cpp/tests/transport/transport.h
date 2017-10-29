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

#ifndef NNGCPP_TESTS_TRANSPORT_TESTS_H
#define NNGCPP_TESTS_TRANSPORT_TESTS_H

#include <protocol/protocol.h>

#include "../catch/catch_nng_exception_matcher.hpp"
#include "../catch/catch_exception_translations.hpp"
#include "../catch/catch_macros.hpp"
#include "../helpers/basic_fixture.h"

#include <vector>
#include <string>
#include <memory>
#include <functional>

namespace constants {
    extern const std::string test_addr_base;
    extern const char port_delim;
    extern const std::vector<std::string> prefix_tags;
}

extern void init(const std::string& addr);

namespace nng {

    struct address_calculator {

        address_calculator(char port_delim = '\0');

        virtual ~address_calculator();

        std::string GetIPv4Addr(const std::string& base_addr, int delta = 0);
        std::string get_next_addr(const std::string& base_addr, int delta = 1);
        std::string get_prev_addr(const std::string& base_addr, int delta = -1);

    private:

        static int port;

        std::string _port_delim;

        uint16_t GetPort(int delta);

    public:

        static uint16_t get_current_port();
    };

    template<typename Req_, typename Rep_>
    struct transport_fixture_redeux_base : public basic_fixture {

        typedef Req_ req_type;
        typedef Rep_ rep_type;

        req_type _req;
        rep_type _rep;

    protected:

        transport_fixture_redeux_base() : _req(), _rep() {
        }

        transport_fixture_redeux_base(req_type req, rep_type rep) : _req(req), _rep(rep) {
        }
    };

    struct c_style_transport_fixture_redeux
        : public transport_fixture_redeux_base<::nng_socket, ::nng_socket> {

        c_style_transport_fixture_redeux();

        virtual ~c_style_transport_fixture_redeux();
    };

    struct transport_fixture_redeux : public transport_fixture_redeux_base<
        std::unique_ptr<protocol::latest_req_socket>
        , std::unique_ptr<protocol::latest_rep_socket>> {

        transport_fixture_redeux();

        virtual ~transport_fixture_redeux();
    };
}

#endif // NNGCPP_TESTS_TRANSPORT_TESTS_H
