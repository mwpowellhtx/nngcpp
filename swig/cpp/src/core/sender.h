#ifndef NNGCPP_SENDER_H
#define NNGCPP_SENDER_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace nng {

    class NNGCPP_DECLSPEC sender {
        protected:

            typedef std::size_t send_size_type;
            typedef std::vector<uint8_t> send_vector;

            sender();

        public:

            virtual int send(std::string const& str, int flags = 0) = 0;
            virtual int send(std::string const& str, send_size_type sz, int flags = 0) = 0;

            virtual int send(send_vector const& buffer, int flags = 0) = 0;
            virtual int send(send_vector const& buffer, send_size_type sz, int flags = 0) = 0;
    };
}

#endif // NNGCPP_SENDER_H
