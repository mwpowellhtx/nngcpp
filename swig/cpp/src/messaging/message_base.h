#ifndef NNGCPP_MESSAGE_BASE_H
#define NNGCPP_MESSAGE_BASE_H

#include "../core/types.h"
#include "../core/having_one.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>
#include <functional>

namespace nng {

    struct IClearable {
        virtual void Clear() = 0;
    };

    class supports_getting_msg {
    protected:
        virtual msg_type* get_message() const = 0;
    };

    typedef std::vector<uint8_t> buffer_vector_type;

    class _MessageBase
        : public IHaveOne
        , public IClearable
        , public supports_getting_msg {
    private:

        void allocate(size_type sz = 0);

    protected:

        msg_type* _msgp;

        _MessageBase();

        _MessageBase(size_type sz);

        _MessageBase(msg_type* msgp);

        friend msg_type* get_msgp(_MessageBase* const mbp);

        void on_one_required();

        virtual void free();

    public:

        virtual ~_MessageBase();

        virtual bool HasOne() const override;

        virtual msg_type* get_message() const override;

        virtual msg_type* cede_message();

        virtual void retain(msg_type* msgp);
    };

    msg_type* get_msgp(_MessageBase* const mbp);

    // Define for internal use.
    typedef _MessageBase message_base;
}

#endif // NNGCPP_MESSAGE_BASE_H
