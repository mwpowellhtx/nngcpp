#include "push.h"

#define THROW_PUSH_SOCKET_INV_OP() throw trx::invalid_operation("push sockets cannot receive")

namespace nng {
    namespace protocol {
        namespace v0 {

            using std::placeholders::_1;

            // While we could use nng_push_open, I think it is sufficient to just use the versioned symbol.
            push_socket::push_socket() : socket(std::bind(&(::nng_push0_open), _1)) {
            }

            push_socket::~push_socket() {
            }

            std::unique_ptr<push_socket::binary_message_type> push_socket::receive(flag_type flags) {
                THROW_PUSH_SOCKET_INV_OP();
            }

            int push_socket::try_receive(binary_message_type* const bmp, flag_type flags) {
                THROW_PUSH_SOCKET_INV_OP();
            }

            push_socket::buffer_vector_type push_socket::receive(size_type& sz, flag_type flags) {
                THROW_PUSH_SOCKET_INV_OP();
            }

            int push_socket::try_receive(buffer_vector_type* const bufp, size_type& sz, flag_type flags) {
                THROW_PUSH_SOCKET_INV_OP();
            }
        }
    }
}
