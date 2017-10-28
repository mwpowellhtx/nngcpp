#include "binary_message.h"

namespace nng {

    _Message::_Message() : _BasicMessage() {}

    _Message::_Message(size_type sz) : _BasicMessage(sz) {}

    _Message::_Message(msg_type* msgp) : _BasicMessage(msgp) {}

    _Message::_Message(const  _Message& other) : _BasicMessage() {
        // We do not want to alter the given Other...
        auto op_ = const_cast<_Message*>(&other);
        // But yet we still need to thread the needle.
        GetHeader()->Append(op_->GetHeader()->Get());
        GetBody()->Append(op_->GetBody()->Get());
    }

    _Message::~_Message() {}

    _Message::header_type* const _Message::GetHeader() {
        return basic_message_type::GetHeader();
    }

    _Message::body_type* const _Message::GetBody() {
        return basic_message_type::GetBody();
    }

    size_type _Message::GetSize() {
        return basic_message_type::GetSize();
    }

    void _Message::Clear() {
        return basic_message_type::Clear();
    }
}
