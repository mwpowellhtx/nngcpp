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

        virtual void Send(binary_message& m, flag_type flags = flag_none) = 0;

        virtual void Send(const buffer_vector_type& buf, flag_type flags = flag_none) = 0;
        virtual void Send(const buffer_vector_type& buf, size_type sz, flag_type flags = flag_none) = 0;

        // TODO: TBD: make sure that the async coverage extends, even through the recv-only API.
        virtual void SendAsync(const basic_async_service* const svcp) = 0;
    };
}

#endif // NNGCPP_SENDER_H
