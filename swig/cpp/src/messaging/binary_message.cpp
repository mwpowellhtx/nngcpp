#include "binary_message.h"

namespace nng {

    binary_message::binary_message() : basic_binary_message() {}

    binary_message::binary_message(size_type sz) : basic_binary_message(sz) {}

    binary_message::binary_message(msg_type* msgp) : basic_binary_message(msgp) {}

    binary_message::~binary_message() {}
}
