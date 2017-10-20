#ifndef NNGCPP_MESSAGING_UTILS_H
#define NNGCPP_MESSAGING_UTILS_H

#include "message_base.h"

#include <cstddef>
#include <vector>
#include <string>

namespace nng {

    namespace messaging {

        struct messaging_utils {
        public:

        private:

            messaging_utils();

            virtual ~messaging_utils();

        public:

            static std::string to_string(const buffer_vector_type* const bufp);
            static std::string to_string(const buffer_vector_type* const bufp, size_type sz);

            static buffer_vector_type to_buffer(const std::string::value_type* cp);
            static buffer_vector_type to_buffer(const std::string::value_type* cp, size_type sz);

            static buffer_vector_type to_buffer(const std::string* const sp);
            static buffer_vector_type to_buffer(const std::string* const sp, size_type sz);
        };
    }
}

#endif // NNGCPP_MESSAGING_UTILS_H
