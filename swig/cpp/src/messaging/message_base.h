#ifndef NNGCPP_MESSAGE_BASE_H
#define NNGCPP_MESSAGE_BASE_H

#include "../core/types.h"

#include "../core/having_one.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>
#include <functional>

namespace nng {

#ifndef NNGCPP_SOCKET_H
    class socket;
#endif // NNGCPP_SOCKET_H

    namespace messaging {

        typedef ::nng_msg msg_type;

        struct supports_clear_api {
            virtual void clear() = 0;
        };

        struct supports_getting_msg {
        protected:
            virtual msg_type* get_msgp() const = 0;
        };

        typedef std::vector<uint8_t> buffer_vector_type;

        class message_base : public having_one, public supports_clear_api, public supports_getting_msg {
        private:

            friend class socket;

            msg_type* _msgp;

        public:

            typedef void* part_type;

        protected:

            message_base();

            friend msg_type* get_msgp(message_base* const mbp);

            virtual msg_type* get_msgp() const override;

        public:

            virtual ~message_base();

            virtual bool has_one() const override;
        };

        msg_type* get_msgp(message_base* const mbp);

        class message_part : public having_one, public supports_clear_api, public supports_getting_msg {
        private:

            message_base* _basep;

        protected:

            message_part(message_base* basep);

            virtual ~message_part();

            virtual msg_type* get_msgp() const override;

        public:

            virtual bool has_one() const override;
        };
    }
}

#endif // NNGCPP_MESSAGE_BASE_H
