#include "pub.h"
#include "../../core/exceptions.hpp"

namespace nng {
    namespace protocol {
        namespace v0 {

            using std::placeholders::_1;
            using std::bind;

            // While we could use nng_pub_open, I think it is sufficient to just use the versioned symbol.
            pub_socket::pub_socket() : _Socket(bind(&(::nng_pub0_open), _1)) {
            }

            pub_socket::~pub_socket() {
            }

            std::unique_ptr<binary_message> pub_socket::Receive(flag_type flags) {
                THROW_SOCKET_INV_OP(Publishers, Receive);
            }

            bool pub_socket::TryReceive(binary_message* const bmp, flag_type flags) {
                THROW_SOCKET_INV_OP(Publishers, TryReceive);
            }

            buffer_vector_type pub_socket::Receive(size_type& sz, flag_type flags) {
                THROW_SOCKET_INV_OP(Publishers, Receive);
            }

            bool pub_socket::TryReceive(buffer_vector_type* const bufp, size_type& sz, flag_type flags) {
                THROW_SOCKET_INV_OP(Publishers, TryReceive);
            }

            void pub_socket::ReceiveAsync(basic_async_service* const svcp) {
                THROW_SOCKET_INV_OP(Publishers, ReceiveAsync);
            }
        }
    }
}
