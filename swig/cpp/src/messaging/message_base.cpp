#include "message_base.h"
#include "../core/invocation.hpp"

namespace nng {

    using std::placeholders::_1;
    using std::bind;

    msg_type* get_msgp(_MessageBase* const mbp) {
        // TODO: TBD: this should never be the case I think.
        return mbp == nullptr ? nullptr : mbp->_msgp;
    }

    _MessageBase::_MessageBase()
        : IHaveOne(), IClearable(), supports_getting_msg()
        , _msgp(nullptr) {

        allocate();
    }

    _MessageBase::_MessageBase(size_type sz)
        : IHaveOne(), IClearable(), supports_getting_msg()
        , _msgp(nullptr) {

        allocate(sz);
    }

    _MessageBase::_MessageBase(msg_type* msgp)
        : IHaveOne(), IClearable(), supports_getting_msg()
        , _msgp(msgp) {
    }

    _MessageBase::~_MessageBase() {
        free();
    }

    void _MessageBase::on_one_required() {
        if (HasOne()) { return; }
    }

    void _MessageBase::allocate(size_type sz) {
        if (HasOne()) { return; }
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
        retain(msgp);
    }

    void _MessageBase::free() {
        if (!HasOne()) { return; }
        const auto op = bind(&::nng_msg_free, _msgp);
        invocation::with_void_return_value(op);
        // Just set the member directly since we own it.
        _msgp = nullptr;
    }

    msg_type* _MessageBase::get_message() const {
        return _msgp;
    }

    bool _MessageBase::HasOne() const {
        return _msgp != nullptr;
    }

    msg_type* _MessageBase::cede_message() {
        // Keep hold of the ptr locally while we also nullify the internal ptr.
        auto msgp = _msgp;
        _msgp = nullptr;
        return msgp;
    }

    void _MessageBase::retain(msg_type* msgp) {
        free();
        _msgp = msgp;
    }
}
