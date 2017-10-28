#ifndef NNGCPP_MESSAGE_PART_H
#define NNGCPP_MESSAGE_PART_H

#include "message_base.h"
#include "messaging_api_base.hpp"

namespace nng {

    class _MessagePart
        : public IHaveOne
        , public IClearable
        , public supports_getting_msg
        , public ICanGet<buffer_vector_type>
        , public ICanAppend<const buffer_vector_type&>
        , public ICanAppend<const std::string&>
        , public ICanAppend<uint32_t>
        , public ICanPrepend<const buffer_vector_type&>
        , public ICanPrepend<const std::string&>
        , public ICanPrepend<uint32_t>
        , public ICanTrimLeft<size_type>
        , public ICanTrimLeft<uint32_t*>
        , public ICanTrimRight<size_type>
        , public ICanTrimRight<uint32_t*> {
    private:

        _MessageBase* _basep;

    protected:

        _MessagePart(_MessageBase* basep);

        virtual ~_MessagePart();

        virtual msg_type* get_message() const override;

    public:

        //virtual bool HasOne() const = 0;
    };

    // Define for internal use.
    typedef _MessagePart message_part;
}

#endif // NNGCPP_MESSAGE_PART_H
