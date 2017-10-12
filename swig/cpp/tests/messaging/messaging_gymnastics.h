#ifndef NNGCPP_TESTS_MESSAGING_GYMNASTICS_H
#define NNGCPP_TESTS_MESSAGING_GYMNASTICS_H

#include "../../src/nngcpp_messaging.hpp"

#include <string>

namespace nng {

    namespace messaging {

        // TODO: TBD: re-factor this to a helpful header file.
        // TODO: TBD: may also need/want to add the source file if there is one to the project builder.
        struct binary_message_converter {

            typedef std::string string_type;
            typedef nng::messaging::message_base::size_type size_type;
            typedef nng::messaging::message_base::buffer_vector_type buffer_vector_type;
            typedef nng::messaging::binary_message binary_message_type;
            typedef nng::messaging::messaging_utils messaging_utils;

            static void append_to(const string_type& src, binary_message_type& dest);

            static void get_from(string_type& dest, binary_message_type& src);
        };

        binary_message_converter::binary_message_type& operator<<(
            binary_message_converter::binary_message_type& bm
            , const binary_message_converter::string_type& s);

        binary_message_converter::string_type& operator<<(
            binary_message_converter::string_type& s
            , binary_message_converter::binary_message_type& bm);
    }
}

#endif // NNGCPP_TESTS_MESSAGING_GYMNASTICS_H
