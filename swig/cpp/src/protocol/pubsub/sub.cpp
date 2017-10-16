#include "sub.h"
#include "../../core/exceptions.hpp"

#define THROW_SUB_SOCKET_INV_OP() throw trx::invalid_operation("subscriber sockets cannot send")

namespace nng {
    namespace protocol {
        namespace v0 {

            using std::placeholders::_1;

            // While we could use nng_sub_open, I think it is sufficient to just use the versioned symbol.
            sub_socket::sub_socket() : socket(std::bind(&(::nng_sub0_open), _1)) {
            }

            sub_socket::~sub_socket() {
            }

            void sub_socket::send(binary_message_type* const bmp, flag_type flags) {
                THROW_SUB_SOCKET_INV_OP();
            }

            int sub_socket::send(const buffer_vector_type* const bufp, flag_type flags) {
                THROW_SUB_SOCKET_INV_OP();
            }

            int sub_socket::send(const buffer_vector_type* const bufp, size_type sz, flag_type flags) {
                THROW_SUB_SOCKET_INV_OP();
            }
        }
    }
}
