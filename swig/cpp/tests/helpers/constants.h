#ifndef NNGCPP_TESTS_CONSTANTS_HELPERS_H
#define NNGCPP_TESTS_CONSTANTS_HELPERS_H

#include <messaging/binary_message.h>

#include <string>
#include <ostream>

namespace constants {

    // TODO: TBD: this really deserves first class support throughout test framework
    nng::messaging::binary_message::buffer_vector_type to_buffer(const std::string& s);

    template<typename Type_>
    std::string to_str(Type_ value) {
        std::ostringstream os;
        os << value;
        return os.str();
    }
}

#endif // NNGCPP_TESTS_CONSTANTS_HELPERS_H
