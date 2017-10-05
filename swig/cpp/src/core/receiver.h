#ifndef NNGCPP_RECEIVER_H
#define NNGCPP_RECEIVER_H

#include "../nngcpp_integration.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace nng {

    class receiver {
        public:

            typedef std::size_t receive_size_type;

        protected:

            typedef std::vector<uint8_t> receive_vector;

            receiver();

        public:

            virtual int try_receive(std::string& str, const receive_size_type sz, int flags = 0) = 0;
            virtual std::string receive_str(receive_size_type& sz, int flags = 0) = 0;

            virtual int try_receive(receive_vector& buffer, receive_size_type& sz, int flags = 0) = 0;
            virtual receive_vector receive_buffer(receive_size_type& sz, int flags = 0) = 0;
    };
}

#endif // NNGCPP_RECEIVER_H
