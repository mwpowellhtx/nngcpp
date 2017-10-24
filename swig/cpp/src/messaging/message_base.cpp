#include "message_base.h"
#include "message_pipe.h"
#include "../core/invocation.hpp"

#define THROW_BIN_MSG_BODY_INV_OP(basep) \
            if (!basep) \
                throw nng::exceptions::invalid_operation("binary message body cannot exist apart from a parent message")

namespace nng {

    using std::placeholders::_1;
    using std::bind;

    msg_type* get_msgp(message_base* const mbp) {
        // TODO: TBD: this should never be the case I think.
        return mbp == nullptr ? nullptr : mbp->_msgp;
    }

    message_base::message_base()
        : having_one(), supports_clear_api(), supports_getting_msg()
        , _msgp(nullptr) {

        allocate();
    }

    message_base::message_base(size_type sz)
        : having_one(), supports_clear_api(), supports_getting_msg()
        , _msgp(nullptr) {

        allocate(sz);
    }

    message_base::message_base(msg_type* msgp)
        : having_one(), supports_clear_api(), supports_getting_msg()
        , _msgp(msgp) {
    }

    message_base::~message_base() {
        free();
    }

    void message_base::on_one_required() {
        if (has_one()) { return; }
    }

    void message_base::allocate(size_type sz) {
        if (has_one()) { return; }
        msg_type* msgp = nullptr;
        try {
            const auto op = bind(&::nng_msg_alloc, &msgp, _1);
            invocation::with_default_error_handling(op, sz);
        }
#if 0
        catch (std::exception& ex) {
#else
        catch (...) {
#endif
            //// TODO: TBD: not sure this would be valid... or depending on the error...
            //if (msgp) {
            //    const auto __free = std::bind(::nng_msg_free, _1);
            //    __free(msgp);
            //}
            throw;
        }
        set_msgp(msgp);
    }

    void message_base::free() {
        if (!has_one()) { return; }
        const auto op = bind(&::nng_msg_free, _msgp);
        invocation::with_void_return_value(op);
        // Just set the member directly since we own it.
        _msgp = nullptr;
    }

    msg_type* message_base::get_msgp() const {
        return _msgp;
    }

    void message_base::set_msgp(msg_type* msgp) {
        free();
        _msgp = msgp;
    }

    bool message_base::has_one() const {
        return _msgp != nullptr;
    }

    void message_base::on_sent() {
        _msgp = nullptr;
    }

    void message_base::set_pipe(const message_pipe* const mpp) {
        if (mpp == nullptr) { return; }
        on_one_required();
        // This is another convenience moment: allocate beforehand, if necessary.
        // TODO: TBD: there is no return value here unfortunately... perhaps there should be?
        const auto op = bind(&::nng_msg_set_pipe, _msgp, get_id(*mpp));
        invocation::with_void_return_value(op);
        // TODO: TBD: upon "setting the pipe" is that akin to handing of ownership of the pipe back to the message?
    }

    message_part::message_part(message_base* basep)
        : having_one(), supports_clear_api(), supports_getting_msg()
        , _basep(basep) {
        THROW_BIN_MSG_BODY_INV_OP(_basep);
    }

    message_part::~message_part() {
        _basep = nullptr;
    }

    msg_type* message_part::get_msgp() const {
        return nng::get_msgp(_basep);
    }

    bool message_part::has_one() const {
        return get_msgp() != nullptr;
    }
}
