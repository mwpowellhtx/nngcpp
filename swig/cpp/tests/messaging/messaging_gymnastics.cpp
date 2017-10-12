#include <catch.hpp>

#include "messaging_gymnastics.h"

namespace nng {
    namespace messaging {

        void binary_message_converter::append_to(const string_type& src, binary_message_type& dest) {
            REQUIRE(src.length() > 0);
            const auto _src_buf = messaging_utils::to_buffer(&src);
            REQUIRE(src.length() == _src_buf.size());
            REQUIRE(dest.body() != nullptr);
            dest.body()->append(_src_buf);
        }

        void binary_message_converter::get_from(string_type& dest, binary_message_type& src) {
            // It is managable, but just so we are aware.
            const auto _got_src = src.body()->get();
            REQUIRE(_got_src.size() > 0);
            dest = messaging_utils::to_string(&_got_src);
            REQUIRE(dest.length() == _got_src.size());
        }

        binary_message_converter::binary_message_type& operator<<(
            binary_message_converter::binary_message_type& bm
            , const binary_message_converter::string_type& s) {

            binary_message_converter::append_to(s, bm);
            return bm;
        }

        binary_message_converter::string_type& operator<<(
            binary_message_converter::string_type& s
            , binary_message_converter::binary_message_type& bm) {

            binary_message_converter::get_from(s, bm);
            return s;
        }
    }
}
