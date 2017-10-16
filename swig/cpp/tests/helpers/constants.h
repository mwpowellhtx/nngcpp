#ifndef NNGCPP_TESTS_CONSTANTS_HELPERS_H
#define NNGCPP_TESTS_CONSTANTS_HELPERS_H

#include <nngcpp.h>

#include <algorithm>
#include <type_traits>

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

// Provide a little shorthand to save some calories.

// Designed for use within the scope of the constants namespace.
#define TO_BUFFER_RETVAL std::result_of<decltype(&to_buffer)(const std::string&)>::type

#endif // NNGCPP_TESTS_CONSTANTS_HELPERS_H
