#include "pull.h"

namespace nng {
    namespace protocol {
        namespace v0 {

            using std::placeholders::_1;
            using std::bind;

            // While we could use nng_pull_open, I think it is sufficient to just use the versioned symbol.
            pull_socket::pull_socket() : _Socket(bind(&(::nng_pull0_open), _1)) {
            }

            pull_socket::~pull_socket() {
            }

            void pull_socket::Send(binary_message& m, flag_type flags) {
                THROW_SOCKET_INV_OP(Pullers, Send);
            }

            void pull_socket::Send(const buffer_vector_type& buf, flag_type flags) {
                THROW_SOCKET_INV_OP(Pullers, Send);
            }

            void pull_socket::Send(const buffer_vector_type& buf, size_type sz, flag_type flags) {
                THROW_SOCKET_INV_OP(Pullers, Send);
            }

            void pull_socket::SendAsync(const basic_async_service* const svcp) {
                THROW_SOCKET_INV_OP(Pullers, SendAsync);
            }
        }
    }
}
