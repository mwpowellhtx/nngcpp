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

#ifndef NNGCPP_TESTS_EXTENDED_TRANSPORT_FIXTURE_H
#define NNGCPP_TESTS_EXTENDED_TRANSPORT_FIXTURE_H

#include "transport.h"

#include <core/listener.h>
#include <core/dialer.h>
#include <messaging/binary_message.h>

#include <string>
#include <functional>

namespace nng {

    struct extended_transport_fixture : public transport_fixture {

        typedef std::function<void(messaging::binary_message* bmp
            , listener* lp, dialer* dp)> run_property_tests_func;

        extended_transport_fixture(const std::string& base_addr
            , const run_property_tests_func& property_tests);

        virtual ~extended_transport_fixture();

        virtual void run_all() override;

    protected:

        virtual void run_all(const std::string& addr) override;

    private:

        const run_property_tests_func _test_properties;

        void run_property_tests(const std::string& addr);
    };
}

#endif // NNGCPP_TESTS_EXTENDED_TRANSPORT_FIXTURE_H
