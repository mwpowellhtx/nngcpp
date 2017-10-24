#include "push.h"
#include "../../core/exceptions.hpp"

#define THROW_PUSH_SOCKET_INV_OP() throw nng::exceptions::invalid_operation("push sockets cannot receive")

namespace nng {
    namespace protocol {
        namespace v0 {

            using std::placeholders::_1;
            using std::bind;

            // While we could use nng_push_open, I think it is sufficient to just use the versioned symbol.
            push_socket::push_socket() : socket(bind(&(::nng_push0_open), _1)) {
            }

            push_socket::~push_socket() {
            }

            std::unique_ptr<binary_message> push_socket::receive(flag_type flags) {
                THROW_PUSH_SOCKET_INV_OP();
            }

            bool push_socket::try_receive(binary_message* const bmp, flag_type flags) {
                THROW_PUSH_SOCKET_INV_OP();
            }

            buffer_vector_type push_socket::receive(size_type& sz, flag_type flags) {
                THROW_PUSH_SOCKET_INV_OP();
            }

            bool push_socket::try_receive(buffer_vector_type* const bufp, size_type& sz, flag_type flags) {
                THROW_PUSH_SOCKET_INV_OP();
            }
        }
    }
}
