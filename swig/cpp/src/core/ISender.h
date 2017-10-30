#ifndef NNGCPP_SENDER_H
#define NNGCPP_SENDER_H

#include "types.h"
#include "enums.h"

#include "async/basic_async_service.h"

#include "../messaging/messaging.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace nng {

    class ISender {
    protected:

        ISender();

    public:

        virtual ~ISender();

        virtual void send(binary_message* const bmp, flag_type flags = flag_none) = 0;

        virtual void send(const buffer_vector_type* const bufp, flag_type flags = flag_none) = 0;
        virtual void send(const buffer_vector_type* const bufp, size_type sz, flag_type flags = flag_none) = 0;

        virtual void send_async(const basic_async_service* const svcp) = 0;
    };
}

#endif // NNGCPP_SENDER_H
