#ifndef NNGCPP_TESTS_CONSTANTS_HELPERS_H
#define NNGCPP_TESTS_CONSTANTS_HELPERS_H

#include <nngcpp.h>

#include <algorithm>

namespace constants {

    // TODO: TBD: this really deserves first class support throughout test framework
    nng::messaging::binary_message::buffer_vector_type to_buffer(const std::string& s) {
        nng::messaging::binary_message::buffer_vector_type buf;
        std::for_each(s.cbegin(), s.cend(), [&buf](const std::string::value_type& ch) {
            buf.push_back(ch);
        });
        return buf;
    }

}

#endif // NNGCPP_TESTS_CONSTANTS_HELPERS_H
