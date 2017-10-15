#ifndef NNGCPP_MESSAGE_BASE_H
#define NNGCPP_MESSAGE_BASE_H

#include <nng/nng.h>

#include <cstddef>
#include <cstdint>
#include <vector>
#include <functional>

namespace nng {

#ifndef NNGCPP_SOCKET_H
    class socket;
#endif // NNGCPP_SOCKET_H

    namespace messaging {

#ifndef NNGCPP_MESSAGE_PIPE_H
        class message_pipe;
#endif // NNGCPP_MESSAGE_PIPE_H

        // TODO: TBD: not the best placement for this one, but it will have to do.
        class message_base_api {
        protected:

            message_base_api();

            typedef std::function<void(::nng_msg*)> clear_op;

            static void do_clear_op(const clear_op& op, ::nng_msg* msgp) {
                if (msgp == nullptr) { return; }
                op(msgp);
            }

            virtual void clear() = 0;
        };

        class message_base : public message_base_api {
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

            virtual void set_msgp(::nng_msg* msgp);

            virtual ::nng_msg* get_msgp() const;

            virtual bool has_message() const;
        };
    }
}

#endif // NNGCPP_MESSAGE_BASE_H
