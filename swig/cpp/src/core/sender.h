#ifndef NNGCPP_SENDER_H
#define NNGCPP_SENDER_H

#include "../nngcpp_integration.h"

#include "nngcpp_messaging.hpp"

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

        virtual void send(const binary_message_type* const bmp, int flags = 0) = 0;

        virtual int send(const buffer_vector_type* const bufp, int flags = 0) = 0;
        virtual int send(const buffer_vector_type* const bufp, size_type sz, int flags = 0) = 0;
    };
}

#endif // NNGCPP_SENDER_H
