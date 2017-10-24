#include "constants.h"

#include <algorithm>

namespace constants {

    nng::buffer_vector_type to_buffer(const std::string& s) {
        nng::buffer_vector_type buf;
        std::for_each(s.cbegin(), s.cend(), [&buf](const std::string::value_type& ch) {
            buf.push_back(ch);
        });
        return buf;
    }
}
