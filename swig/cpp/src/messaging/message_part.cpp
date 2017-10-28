#include "message_part.h"
#include "../core/exceptions.hpp"

namespace nng {

#define THROW_MSG_PART_INV_OP(basep) \
        if (!basep) \
            throw exceptions::invalid_operation( \
                "message part cannot exist apart from a parent message")

    using std::placeholders::_1;
    using std::bind;

    _MessagePart::_MessagePart(_MessageBase* basep)
        : IHaveOne(), IClearable(), supports_getting_msg()
        , _basep(basep) {
        THROW_MSG_PART_INV_OP(_basep);
    }

    _MessagePart::~_MessagePart() {
        _basep = nullptr;
    }

    msg_type* _MessagePart::get_message() const {
        return nng::get_msgp(_basep);
    }

    //bool _MessagePart::HasOne() const {
    //    return get_message() != nullptr;
    //}
}
