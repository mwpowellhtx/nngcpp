#ifndef NNGCPP_SENDER_H
#define NNGCPP_SENDER_H

#include "../messaging/messaging.h"

#include "enums.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace nng {

    class sender {
    protected:

        sender();

    private:

        typedef messaging::binary_message binary_message_type;
        typedef messaging::message_base::size_type size_type;
        typedef messaging::message_base::buffer_vector_type buffer_vector_type;

    public:

        virtual void send(binary_message_type* const bmp, flag_type flags = flag_none) = 0;

        virtual int send(const buffer_vector_type* const bufp, flag_type flags = flag_none) = 0;
        virtual int send(const buffer_vector_type* const bufp, size_type sz, flag_type flags = flag_none) = 0;
    };
}

#endif // NNGCPP_SENDER_H
