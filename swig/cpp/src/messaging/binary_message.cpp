#include "binary_message.h"

namespace nng {

    _Message::_Message() : _BasicMessage() {}

    _Message::_Message(size_type sz) : _BasicMessage(sz) {}

    _Message::_Message(msg_type* msgp) : _BasicMessage(msgp) {}

    _Message::_Message(const  _Message& other) : _BasicMessage() {
        // We do not want to alter the given Other...
        auto op_ = const_cast<_Message*>(&other);
        // But yet we still need to thread the needle.
        header()->Append(op_->header()->Get());
        body()->Append(op_->body()->Get());
    }

    _Message::~_Message() {}
}
