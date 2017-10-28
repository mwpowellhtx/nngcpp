//
// Copyright (c) 2017 Michel W Powell <mwpowellhtx@gmail.com>
//
// This software is supplied under the terms of the MIT License, a
// copy of which should be located in the distribution where this
// file was obtained (LICENSE.txt).  A copy of the license may also be
// found online at https://opensource.org/licenses/MIT.
//

#ifndef NNGCPP_BINARY_MESSAGE_BODY_H
#define NNGCPP_BINARY_MESSAGE_BODY_H

#include "../core/types.h"

#include "message_part.h"

#include "messaging_api_base.hpp"

#include <cstdint>

namespace nng {

#ifndef NNGCPP_STRING_BASED_MESSAGE_H
    template<class Body_, class Header_> class _BasicMessage;
#endif //NNGCPP_STRING_BASED_MESSAGE_H

    class _BodyMessagePart : public _MessagePart {
    protected:

        template<class Body_, class Header_> friend class _BasicMessage;

        _BodyMessagePart(_MessageBase* const basep);

    public:

        virtual ~_BodyMessagePart();

        virtual bool HasOne() const override;

        virtual bool TryGet(buffer_vector_type const* resultp) override;

        virtual const buffer_vector_type Get() override;

        virtual size_type GetSize() override;

        virtual void Clear() override;

        virtual void Append(const buffer_vector_type& buf) override;

        virtual void Append(const std::string& s) override;

        virtual void Append(uint32_t val) override;

        virtual void Prepend(const buffer_vector_type& buf) override;

        virtual void Prepend(const std::string& s) override;

        virtual void Prepend(uint32_t val) override;

        virtual void TrimLeft(size_type sz) override;

        virtual void TrimLeft(uint32_t* resultp) override;

        virtual void TrimRight(size_type sz) override;

        virtual void TrimRight(uint32_t* resultp) override;
    };

    typedef _BodyMessagePart binary_message_body;
}

#endif // NNGCPP_BINARY_MESSAGE_BODY_H
