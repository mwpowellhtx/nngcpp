#include "binary_message.h"

namespace nng {

    namespace messaging {

        binary_message::binary_message() : basic_binary_message() {}

        binary_message::binary_message(size_type sz) : basic_binary_message() {}

        binary_message::binary_message(::nng_msg* msgp) : basic_binary_message(msgp) {}

        binary_message::~binary_message() {}
    }
}
