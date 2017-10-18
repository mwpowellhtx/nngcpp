#ifndef NNGCPP_MESSAGE_BASE_H
#define NNGCPP_MESSAGE_BASE_H

#define NNG_ONLY
#include <nngcpp.h>

#include <cstddef>
#include <cstdint>
#include <vector>
#include <functional>

namespace nng {

#ifndef NNGCPP_SOCKET_H
    class socket;
#endif // NNGCPP_SOCKET_H

    namespace messaging {

        // TODO: TBD: not the best placement for this one, but it will have to do.
        class message_base_api {
        protected:

            friend class message_part;

            message_base_api();

            virtual message_base_api& get_base() = 0;

            virtual ::nng_msg* get_msgp() = 0;

            typedef std::function<void(::nng_msg*)> clear_op;

            static void do_clear_op(const clear_op& op, ::nng_msg* msgp);

            virtual void clear() = 0;

        public:

            bool has_one();
        };

#ifndef NNGCPP_MESSAGE_PIPE_H
        class message_pipe;
#endif // NNGCPP_MESSAGE_PIPE_H

        class message_base : public message_base_api {
        private:

            friend class message_base_api;

            friend class message_pipe;

            friend class socket;

        public:

            typedef void* part_type;

            typedef std::size_t size_type;

            typedef std::vector<uint8_t> buffer_vector_type;

        protected:

            message_base();

            virtual message_base& get_base() override;

        public:

            virtual ~message_base();
        };

        class message_part : public message_base_api {
        private:

            message_base* _basep;

            virtual message_base_api& get_base() override;

        protected:

            message_part(message_base* basep);

            virtual ~message_part();

            virtual ::nng_msg* get_msgp() override;
        };
    }
}

#endif // NNGCPP_MESSAGE_BASE_H
