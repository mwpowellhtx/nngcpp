#ifndef NNGCPP_MESSAGE_BASE_H
#define NNGCPP_MESSAGE_BASE_H

#include "../core/types.h"
#include "../core/having_one.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>
#include <functional>

namespace nng {

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

        void allocate(size_type sz = 0);

    protected:

        msg_type* _msgp;

        message_base();

        message_base(size_type sz);

        message_base(msg_type* msgp);

        friend msg_type* get_msgp(message_base* const mbp);

        void on_one_required();

        virtual void free();

    public:

        virtual ~message_base();

        virtual bool has_one() const override;

        virtual void set_msgp(msg_type* msgp);

        virtual msg_type* get_msgp() const override;

        virtual void on_sent();
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

#endif // NNGCPP_MESSAGE_BASE_H
