#include "push.h"

namespace nng {
    namespace protocol {
        namespace v0 {

            using std::placeholders::_1;
            using std::bind;

            // While we could use nng_push_open, I think it is sufficient to just use the versioned symbol.
            push_socket::push_socket() : _Socket(bind(&(::nng_push0_open), _1)) {
            }

            push_socket::~push_socket() {
            }

            std::unique_ptr<binary_message> push_socket::Receive(flag_type flags) {
                THROW_SOCKET_INV_OP(Pushers, Receive);
            }

            bool push_socket::TryReceive(binary_message* const bmp, flag_type flags) {
                THROW_SOCKET_INV_OP(Pushers, TryReceive);
            }

            buffer_vector_type push_socket::Receive(size_type& sz, flag_type flags) {
                THROW_SOCKET_INV_OP(Pushers, Receive);
            }

            bool push_socket::TryReceive(buffer_vector_type* const bufp, size_type& sz, flag_type flags) {
                THROW_SOCKET_INV_OP(Pushers, TryReceive);
            }

            void push_socket::ReceiveAsync(basic_async_service* const svcp) {
                THROW_SOCKET_INV_OP(Pushers, ReceiveAsync);
            }
        }
    }
}
