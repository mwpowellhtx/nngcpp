#ifndef NNGCPP_TESTS_MESSAGING_BINARY_MESSAGE_PART_FIXTURES_HPP
#define NNGCPP_TESTS_MESSAGING_BINARY_MESSAGE_PART_FIXTURES_HPP

#include <catch.hpp>
#include <nng/nng.h>

namespace nng {
    namespace messaging {

        template<class Base_>
        struct binary_message_part_fixture : Base_ {

            binary_message_part_fixture()
                : Base_() {
            }

            binary_message_part_fixture(::nng_msg* msgp)
                : Base_(msgp) {
            }

            virtual ~binary_message_part_fixture() {
                if (_msgp == nullptr) { return; }
                REQUIRE(_msgp != nullptr);
                CHECK_NOTHROW(::nng_msg_free(_msgp));
                _msgp = nullptr;
                CHECK(_msgp == nullptr);
            }
        };

        template<class Part_>
        void verify_default_message_part(const Part_& part, ::nng_msg* msgp = nullptr) {
            REQUIRE(part.get_msgp() == msgp);
            if (msgp) {
                REQUIRE(part.has_message());
            }
            else {
                REQUIRE(!part.has_message());
            }
            REQUIRE(part.get_size() == 0);
        }
    }
}

#endif // NNGCPP_TESTS_MESSAGING_BINARY_MESSAGE_PART_FIXTURES_HPP
