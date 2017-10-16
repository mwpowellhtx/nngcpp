#include "pub.h"
#include "../../core/exceptions.hpp"

#define THROW_PUB_SOCKET_INV_OP() throw trx::invalid_operation("publisher sockets cannot receive")

namespace nng {
    namespace protocol {
        namespace v0 {

            using std::placeholders::_1;

            // While we could use nng_pub_open, I think it is sufficient to just use the versioned symbol.
            pub_socket::pub_socket() : socket(std::bind(&(::nng_pub0_open), _1)) {
            }

            pub_socket::~pub_socket() {
            }

            std::unique_ptr<pub_socket::binary_message_type> pub_socket::receive(flag_type flags) {
                THROW_PUB_SOCKET_INV_OP();
            }

            int pub_socket::try_receive(binary_message_type* const bmp, flag_type flags) {
                THROW_PUB_SOCKET_INV_OP();
            }

            pub_socket::buffer_vector_type pub_socket::receive(size_type& sz, flag_type flags) {
                THROW_PUB_SOCKET_INV_OP();
            }

            int pub_socket::try_receive(buffer_vector_type* const bufp, size_type& sz, flag_type flags) {
                THROW_PUB_SOCKET_INV_OP();
            }
        }
    }
}
