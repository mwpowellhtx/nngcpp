#include "message_base.h"
#include "../core/exceptions.hpp"

#define THROW_BIN_MSG_BODY_INV_OP(basep) \
            if (!basep) \
                throw nng::exceptions::invalid_operation("binary message body cannot exist apart from a parent message")

namespace nng {

    namespace messaging {

        ::nng_msg* get_msgp(message_base_api& mb) {
            return mb.get_msgp();
        }

        void message_base_api::do_clear_op(const clear_op& op) {
            op();
        }

        message_base_api::message_base_api() {
        }

        message_base::message_base() : message_base_api() {
        }

        message_base::~message_base() {
            /* Derived classes must do the right thing with their own destructors.
            For Header and Body and so forth it is inappropriate to assume ownership.
            However, for Binary Message and sister classes it is entirely correct. */
        }

        message_base& message_base::get_base() {
            return *this;
        }

        bool message_base_api::has_one() {
            const auto msgp = get_base().get_msgp();
            return msgp != nullptr;
        }

        message_part::message_part(message_base* basep) : message_base_api(), _basep(basep) {
            THROW_BIN_MSG_BODY_INV_OP(_basep);
        }

        message_part::~message_part() {
            _basep = nullptr;
        }

        message_base_api& message_part::get_base() {
            return *_basep;
        }

        ::nng_msg* message_part::get_msgp() {
            return get_base().get_msgp();
        }
    }
}
