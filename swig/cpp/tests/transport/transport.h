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

#ifndef NNGCPP_TESTS_TRANSPORT_FIXTURE_H
#define NNGCPP_TESTS_TRANSPORT_FIXTURE_H

#include <protocol/protocol.h>

#include "../catch/catch_nng_exception_matcher.hpp"
#include "../catch/catch_exception_translations.hpp"
#include "../catch/catch_macros.hpp"
#include "../helpers/basic_fixture.h"

#include <string>
#include <memory>
#include <functional>

namespace nng {

    struct address_calculator {

        virtual ~address_calculator();

        std::string get_addr(const std::string& base_addr, int delta = 0);
        std::string get_next_addr(const std::string& base_addr, int delta = 1);
        std::string get_prev_addr(const std::string& base_addr, int delta = -1);

    protected:

        address_calculator(char port_delim = '\0');

    private:

        static int port;

        std::string _port_delim;

        int get_port(int delta);
    };

    struct transport_fixture : public basic_fixture, public address_calculator {

        typedef std::function<void(address_calculator&)> run_one_func;

        transport_fixture(const std::string& base_addr, char port_delim = '\0');

        virtual ~transport_fixture();

        void run_one(const run_one_func& func);

        template<class Func>
        void run_many(const Func& func) {
            address_calculator& self = *this;
            func(self);
        }

        template<class Func, class ...Funcs_>
        void run_many(const Func& func, const Funcs_&... funcs) {
            run_many(func);
            run_many(funcs...);
        }

        template<class ...Funcs_>
        void run_many(const Funcs_&... funcs) {
            run_many(funcs);
        }

        virtual void run_all();

    protected:

        std::string _base_addr;
        std::unique_ptr<protocol::latest_rep_socket> _repp;
        std::unique_ptr<protocol::latest_req_socket> _reqp;

        virtual void run_all(const std::string& addr);

    private:

        void run_connection_refused_works(const std::string& addr);
        void run_dup_listeners_rejected(const std::string& addr);
        void run_listener_and_dialer_accepted(const std::string& addr);
        void run_send_and_receive(const std::string& addr);
        void run_send_and_receive_large_data(const std::string& addr);
    };

    struct c_style_transport_fixture : public basic_fixture, public address_calculator {

        c_style_transport_fixture(const std::string& base_addr, char port_delim = '\0');

        virtual ~c_style_transport_fixture();

        virtual void run_all();
 
        ::nng_socket _rep, _req;

    protected:

        std::string _base_addr;

        virtual void run_all(const std::string& addr);

    private:

        void run_connection_refused_works(const char* addr);
        void run_dup_listeners_rejected(const char* addr);
        void run_listener_and_dialer_accepted(const char* addr);
        void run_send_and_receive(const char* addr);
        void run_send_and_receive_large_data(const char* addr);
    };
}

#endif // NNGCPP_TESTS_TRANSPORT_FIXTURE_H
