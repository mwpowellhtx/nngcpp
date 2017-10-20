#ifndef NNGCPP_SENDER_H
#define NNGCPP_SENDER_H

#include "types.h"
#include "enums.h"

#include "../messaging/messaging.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace nng {

    class sender {
    protected:

        sender();

    private:

        typedef messaging::binary_message binary_message_type;

    public:

        virtual void send(binary_message_type* const bmp, flag_type flags = flag_none) = 0;

        virtual int send(const messaging::buffer_vector_type* const bufp, flag_type flags = flag_none) = 0;
        virtual int send(const messaging::buffer_vector_type* const bufp, size_type sz, flag_type flags = flag_none) = 0;
    };
}

#endif // NNGCPP_SENDER_H
