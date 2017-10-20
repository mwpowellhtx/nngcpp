//
// Copyright (c) 2017 Michel W Powell <mwpowellhtx@gmail.com>
//
// This software is supplied under the terms of the MIT License, a
// copy of which should be located in the distribution where this
// file was obtained (LICENSE.txt).  A copy of the license may also be
// found online at https://opensource.org/licenses/MIT.
//

#include <nngcpp.h>

#include "../catch/catch_exception_translations.hpp"
#include "binary_message_part_fixtures.hpp"

#include "../src/messaging/binary_message.h"
#include "../src/messaging/message_pipe.h"

// We are not testing the ptr per se except to establish the baseline for the next require.
#define NNGCPP_TESTS_INIT_SMART_PTR(sp, init) \
    REQUIRE(sp.get() == nullptr); \
    REQUIRE_NOTHROW(sp = init); \
    REQUIRE(sp.get() != nullptr)


#define NNGCPP_TESTS_RESET_SMART_PTR(sp) \
    REQUIRE_NOTHROW(sp.reset()); \
    REQUIRE(sp.get() == nullptr)

#define NNG_TESTS_ALLOC_MSG_SZ(m, sz) \
    REQUIRE(::nng_msg_alloc(&m, sz) == 0); \
    REQUIRE(m != nullptr)

#define NNG_TESTS_ALLOC_MSG(m) NNG_TESTS_ALLOC_MSG_SZ(m, 0)

// We are not here to test the header or the body, per se, but we do need to ensure that they agree.


namespace constants {
    const std::string hello = "hello";
    const nng::messaging::buffer_vector_type hello_buf = { 'h','e','l','l','o' };
}

namespace nng {
    namespace messaging {

        struct message_pipe_fixture : public message_pipe {

            message_pipe_fixture(message_base* const mbp) : message_pipe(mbp) {
            }

            ::nng_pipe get_pid() const {
                return pid;
            }
        };

        struct binary_message_fixture : public binary_message {

            binary_message_fixture() : binary_message() {
            }

            binary_message_fixture(::nng_msg* msgp) : binary_message(msgp) {
            }

            virtual void free() override {
                binary_message::free();
            }
        };
    }
}

TEST_CASE("Binary messages work", "[binary_message][messaging][cxx]") {

    using namespace std;
    using namespace nng::messaging;
    using namespace constants;
    using namespace Catch::Matchers;

    // TODO: TBD: revisit the utility of the Binary Message macros in general with this one; and reconsider the kinds of unit/integration testing with header and body
    SECTION("Constructors and destructors work") {

        SECTION("Constructors work") {

            // Yes, this is not a mistake. We want to use the same bmp during both paths.
            std::unique_ptr<binary_message_fixture> bmp;

            SECTION("Default constructor means allocated message") {

                NNGCPP_TESTS_INIT_SMART_PTR(bmp, std::make_unique<binary_message_fixture>());

                REQUIRE(bmp->body() != nullptr);
                REQUIRE(bmp->header() != nullptr);
                REQUIRE(bmp->has_one() == true);
                REQUIRE(bmp->get_msgp() != nullptr);

                SECTION("Destructor works") {
                    NNGCPP_TESTS_RESET_SMART_PTR(bmp);
                }
            }

            SECTION("C API constructor means having a message") {

                ::nng_msg* msgp;

                NNG_TESTS_ALLOC_MSG(msgp);

                NNGCPP_TESTS_INIT_SMART_PTR(bmp, std::make_unique<binary_message_fixture>(msgp));

                REQUIRE(bmp->body() != nullptr);
                REQUIRE(bmp->header() != nullptr);
                REQUIRE(bmp->has_one() == true);
                REQUIRE(bmp->get_msgp() == msgp);

                SECTION("Destructor works") {
                    NNGCPP_TESTS_RESET_SMART_PTR(bmp);
                }
            }
        }
    }
}
