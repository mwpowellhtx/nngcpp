#ifndef NNGCPP_SENDER_H
#define NNGCPP_SENDER_H

#include "../nngcpp_integration.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace nng {

    class sender {
        protected:

            typedef std::size_t send_size_type;
            typedef std::vector<uint8_t> send_vector;

            sender();

        public:

            virtual int send(const std::string& str, int flags = 0) = 0;
            virtual int send(const std::string& str, send_size_type sz, int flags = 0) = 0;

            virtual int send(const send_vector& buffer, int flags = 0) = 0;
            virtual int send(const send_vector& buffer, send_size_type sz, int flags = 0) = 0;
    };
}

#endif // NNGCPP_SENDER_H
