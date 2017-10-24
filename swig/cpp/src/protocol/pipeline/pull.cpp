#include "pull.h"
#include "../../core/exceptions.hpp"

#define THROW_PULL_SOCKET_INV_OP() throw nng::exceptions::invalid_operation("pull sockets cannot send")

namespace nng {
    namespace protocol {
        namespace v0 {

            using std::placeholders::_1;

            // While we could use nng_pull_open, I think it is sufficient to just use the versioned symbol.
            pull_socket::pull_socket() : socket(std::bind(&(::nng_pull0_open), _1)) {
            }

            pull_socket::~pull_socket() {
            }

            void pull_socket::send(binary_message_type* const bmp, flag_type flags) {
                THROW_PULL_SOCKET_INV_OP();
            }

            void pull_socket::send(const buffer_vector_type* const bufp, flag_type flags) {
                THROW_PULL_SOCKET_INV_OP();
            }

            void pull_socket::send(const buffer_vector_type* const bufp, size_type sz, flag_type flags) {
                THROW_PULL_SOCKET_INV_OP();
            }
        }
    }
}
