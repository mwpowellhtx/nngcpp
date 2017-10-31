#ifndef NNGCPP_TESTS_PROTOCOL_BOILERPLATE_HPP
#define NNGCPP_TESTS_PROTOCOL_BOILERPLATE_HPP

#include "../../src/messaging/messaging.h"
#include "../../src/protocol/protocol.h"

namespace nng {
    namespace protocol {

        template<class Socket_>
        struct with_protocols : public Socket_ {

            virtual protocol_type get_protocol() const override {
                return Socket_::get_protocol();
            }

            virtual protocol_type get_peer() const override {
                return Socket_::get_peer();
            }
        };

        template<class Socket_>
        struct with_send_and_protocols : public with_protocols<Socket_> {

            virtual void send(binary_message* const bmp, flag_type flags = flag_none) override {
                Socket_::send(bmp, flags);
            }

            virtual void send(const buffer_vector_type* const bufp, flag_type flags = flag_none) override {
                Socket_::send(bufp, flags);
            }

            virtual void send(const buffer_vector_type* const bufp, size_type sz, flag_type flags = flag_none) override {
                Socket_::send(bufp, sz, flags);
            }
        };

        template<class Socket_>
        struct with_recv_and_protocols : public with_protocols<Socket_> {

            virtual std::unique_ptr<binary_message> receive(flag_type flags = flag_none) override {
                return Socket_::receive(flags);
            }

            virtual bool try_receive(binary_message* const bmp, flag_type flags = flag_none) override {
                return Socket_::try_receive(bmp, flags);
            }

            virtual buffer_vector_type receive(size_type& sz, flag_type flags = flag_none) override {
                return Socket_::receive(sz, flags);
            }

            virtual bool try_receive(buffer_vector_type* const bufp, size_type& sz, flag_type flags = flag_none) override {
                return Socket_::try_receive(bufp, sz, flags);
            }
        };
    }
}

// Defined for convenience throughout Protocol unit testing.
#define DEFINE_SOCKET_FIXTURE(__NS__, __FN__, __N__) \
namespace nng { \
    namespace protocol { \
        namespace __NS__ { \
            struct __FN__ : with_protocols<__N__> {}; \
        } \
    } \
}

#define DEFINE_SOCKET_FIXTURE_WITH_RECV_EXPOSURE(__NS__, __FN__, __N__) \
namespace nng { \
    namespace protocol { \
        namespace __NS__ { \
            struct __FN__ : with_recv_and_protocols<__N__> {}; \
        } \
    } \
}

#define DEFINE_SOCKET_FIXTURE_WITH_SEND_EXPOSURE(__NS__, __FN__, __N__) \
namespace nng { \
    namespace protocol { \
        namespace __NS__ { \
            struct __FN__ : with_send_and_protocols<__N__> {}; \
        } \
    } \
}

#endif // NNGCPP_TESTS_PROTOCOL_BOILERPLATE_HPP
