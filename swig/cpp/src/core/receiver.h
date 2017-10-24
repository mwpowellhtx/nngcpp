#ifndef NNGCPP_RECEIVER_H
#define NNGCPP_RECEIVER_H

#include "types.h"
#include "enums.h"

#include "../messaging/messaging.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace nng {

    class receiver {
    protected:

        receiver();

    private:

        typedef messaging::binary_message binary_message_type;
        typedef messaging::buffer_vector_type buffer_vector_type;

    public:

        // TODO: TBD: this one may get somewhat involved...
        virtual std::unique_ptr<binary_message_type> receive(flag_type flags = flag_none) = 0;
        virtual bool try_receive(binary_message_type* const bmp, flag_type flags = flag_none) = 0;

        virtual messaging::buffer_vector_type receive(size_type& sz, flag_type flags = flag_none) = 0;
        virtual bool try_receive(buffer_vector_type* const bufp, size_type& sz, flag_type flags = flag_none) = 0;
    };
}

#endif // NNGCPP_RECEIVER_H
