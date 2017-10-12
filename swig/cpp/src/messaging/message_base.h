#ifndef NNGCPP_MESSAGE_BASE_H
#define NNGCPP_MESSAGE_BASE_H

#include <nng/nng.h>

#include <cstddef>
#include <cstdint>
#include <vector>

namespace nng {

#ifndef NNGCPP_SOCKET_H
    class socket;
#endif // NNGCPP_SOCKET_H

    namespace messaging {

#ifndef NNGCPP_MESSAGE_PIPE_H
        class message_pipe;
#endif // NNGCPP_MESSAGE_PIPE_H

        class message_base {
        private:

            friend class socket;

        public:

            typedef void* part_type;

            typedef std::size_t size_type;

            typedef std::vector<uint8_t> buffer_vector_type;

            friend class message_pipe;

        protected:

            ::nng_msg* _msgp;

            message_base();

            message_base(::nng_msg* msgp);

        public:

            virtual ~message_base();

            virtual size_type get_size() const = 0;

            virtual void clear() = 0;

            virtual void set_msgp(::nng_msg* msgp);

            ::nng_msg* get_msgp() const;

            virtual bool has_message() const;
        };
    }
}

#endif // NNGCPP_MESSAGE_BASE_H
