#include "sub.h"

namespace nng {
    namespace protocol {
        namespace v0 {

            using std::placeholders::_1;
            using std::bind;

            // While we could use nng_sub_open, I think it is sufficient to just use the versioned symbol.
            sub_socket::sub_socket() : _Socket(bind(&(::nng_sub0_open), _1)) {
            }

            sub_socket::~sub_socket() {
            }

            void sub_socket::Send(binary_message& m, flag_type flags) {
                THROW_SOCKET_INV_OP(Subscribers, Send);
            }

            void sub_socket::Send(const buffer_vector_type& buf, flag_type flags) {
                THROW_SOCKET_INV_OP(Subscribers, Send);
            }

            void sub_socket::Send(const buffer_vector_type& buf, size_type sz, flag_type flags) {
                THROW_SOCKET_INV_OP(Subscribers, Send);
            }

            void sub_socket::SendAsync(const basic_async_service* const svcp) {
                THROW_SOCKET_INV_OP(Subscribers, SendAsync);
            }
        }
    }
}
