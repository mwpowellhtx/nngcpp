#ifndef NNGCPP_BINARY_MESSAGE_H
#define NNGCPP_BINARY_MESSAGE_H

#include "basic_binary_message.hpp"

#include "binary_message_header.h"
#include "binary_message_body.h"

namespace nng {

    // TODO: TBD: any reason to define a full on class derivation? perhaps for SWIG purposes? or just type-define it?
    class _Message : public _BasicMessage<_BodyMessagePart, _HeaderMessagePart> {
    public:

        _Message();

        _Message(size_type sz);

        _Message(msg_type* msgp);

        _Message(const _Message& other);

        virtual ~_Message();

        // TODO: TBD: yes, I know we can leverage C++ type definitions here, but we need to help the SWIG mapping out a little.

#ifdef USING_SWIG
        virtual _HeaderMessagePart* const GetHeader();
        virtual _BodyMessagePart* const GetBody();
#else // USING_SWIG
        virtual header_type* const GetHeader();
        virtual body_type* const GetBody();
#endif //USING_SWIG

        virtual size_type GetSize() override;

        virtual void Clear() override;
    };

    typedef _Message binary_message;
}

#endif // NNGCPP_BINARY_MESSAGE_H
