#ifndef NNGCPP_TESTS_BASIC_FIXTURE_HELPERS_H
#define NNGCPP_TESTS_BASIC_FIXTURE_HELPERS_H

#include <catch.hpp>

namespace nng {

    struct basic_fixture {
        basic_fixture();
        virtual ~basic_fixture();
    };
}

#endif // NNGCPP_TESTS_BASIC_FIXTURE_HELPERS_H
