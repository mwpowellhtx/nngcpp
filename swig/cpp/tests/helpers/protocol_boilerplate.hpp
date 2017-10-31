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

            virtual void Send(binary_message* const bmp, flag_type flags = flag_none) override {
                Socket_::Send(bmp, flags);
            }

            virtual void Send(const buffer_vector_type* const bufp, flag_type flags = flag_none) override {
                Socket_::Send(bufp, flags);
            }

            virtual void Send(const buffer_vector_type* const bufp, size_type sz, flag_type flags = flag_none) override {
                Socket_::Send(bufp, sz, flags);
            }

            virtual void SendAsync(const basic_async_service* const svcp) override {
                Socket_::SendAsync(svcp);
            }
        };

        template<class Socket_>
        struct with_recv_and_protocols : public with_protocols<Socket_> {

            virtual std::unique_ptr<binary_message> Receive(flag_type flags = flag_none) override {
                return Socket_::Receive(flags);
            }

            virtual bool TryReceive(binary_message* const bmp, flag_type flags = flag_none) override {
                return Socket_::TryReceive(bmp, flags);
            }

            virtual buffer_vector_type Receive(size_type& sz, flag_type flags = flag_none) override {
                return Socket_::Receive(sz, flags);
            }

            virtual bool TryReceive(buffer_vector_type* const bufp, size_type& sz, flag_type flags = flag_none) override {
                return Socket_::TryReceive(bufp, sz, flags);
            }

            virtual void ReceiveAsync(basic_async_service* const svcp) {
                Socket_::ReceiveAsync(svcp);
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
