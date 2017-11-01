#ifndef NNGCPP_TESTS_PROTOCOL_BOILERPLATE_HPP
#define NNGCPP_TESTS_PROTOCOL_BOILERPLATE_HPP

#include "../../src/messaging/messaging.h"
#include "../../src/protocol/protocol.h"

namespace nng {

    /* We provide a friendly hook (C++ friendship) in order to access the SID. Note
    that this is not something that should ever happen in production code. */
    ::nng_socket get_sid(const _Socket& s);

    namespace protocol {

        // TODO: TBD: defining these temporarily until nni_protocol/nni_peer are refactored/reintroduced.
        nng::uint16_t __get_protocol(::nng_socket sid);
        nng::uint16_t __get_peer(::nng_socket sid);

        template<class Socket_>
        struct with_protocols : public Socket_ {
        private:

            void configure_protos(::nng_socket sid) {
                using namespace std;
                IProtocol::set_getters(
                    bind(&__get_protocol, sid)
                    , bind(&__get_peer, sid)
                );
            }

        public:

            with_protocols() : Socket_() {
                // Pass the already configured SID along to configure IProtocol.
                configure_protos(get_sid(*this));
            }

            virtual ~with_protocols() {
            }

            virtual protocol_type get_protocol() const override {
                return IProtocol::get_protocol();
            }

            virtual protocol_type get_peer() const override {
                return IProtocol::get_peer();
            }
        };

        template<class Socket_>
        struct with_send_and_protocols : public with_protocols<Socket_> {

            virtual void Send(binary_message& m, flag_type flags = flag_none) override {
                Socket_::Send(m, flags);
            }

            virtual void Send(const buffer_vector_type& buf, flag_type flags = flag_none) override {
                Socket_::Send(buf, flags);
            }

            virtual void Send(const buffer_vector_type& buf, size_type sz, flag_type flags = flag_none) override {
                Socket_::Send(buf, sz, flags);
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
