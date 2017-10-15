#include "message_base.h"

namespace nng {

    namespace messaging {

        message_base_api::message_base_api() {
        }

        message_base::message_base() : message_base_api(), _msgp(nullptr) {
        }

        message_base::message_base(::nng_msg* msgp) : message_base_api(), _msgp(msgp) {
        }

        message_base::~message_base() {
            /* Derived classes must do the right thing with their own destructors.
            For Header and Body and so forth it is inappropriate to assume ownership.
            However, for Binary Message and sister classes it is entirely correct. */
        }

        void message_base::set_msgp(::nng_msg* msgp) {
            _msgp = msgp;
        }

        ::nng_msg* message_base::get_msgp() const {
            return _msgp;
        }

        bool message_base::has_message() const {
            return _msgp != nullptr;
        }
    }
}
