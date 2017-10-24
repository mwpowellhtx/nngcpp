#include "message_base.h"
#include "../core/exceptions.hpp"

#define THROW_BIN_MSG_BODY_INV_OP(basep) \
            if (!basep) \
                throw nng::exceptions::invalid_operation("binary message body cannot exist apart from a parent message")

namespace nng {

    namespace messaging {

        msg_type* get_msgp(message_base* const mbp) {
            // TODO: TBD: this should never be the case I think.
            return mbp == nullptr ? nullptr : mbp->_msgp;
        }

        message_base::message_base()
            : having_one(), supports_clear_api(), supports_getting_msg()
            , _msgp(nullptr) {
        }

        message_base::~message_base() {
            // TODO: TBD: consider whether at this point message_base ought not to do the clean up...
            /* Derived classes must do the right thing with their own destructors.
            For Header and Body and so forth it is inappropriate to assume ownership.
            However, for Binary Message and sister classes it is entirely correct. */
        }

        msg_type* message_base::get_msgp() const {
            return _msgp;
        }

        bool message_base::has_one() const {
            return _msgp != nullptr;
        }

        message_part::message_part(message_base* basep)
            : having_one(), supports_clear_api(), supports_getting_msg()
            , _basep(basep) {
            THROW_BIN_MSG_BODY_INV_OP(_basep);
        }

        message_part::~message_part() {
            _basep = nullptr;
        }

        ::nng_msg* message_part::get_msgp() const {
            return messaging::get_msgp(_basep);
        }

        bool message_part::has_one() const {
            return get_msgp() != nullptr;
        }
    }
}
