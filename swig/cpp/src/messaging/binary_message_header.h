//
// Copyright (c) 2017 Michel W Powell <mwpowellhtx@gmail.com>
//
// This software is supplied under the terms of the MIT License, a
// copy of which should be located in the distribution where this
// file was obtained (LICENSE.txt).  A copy of the license may also be
// found online at https://opensource.org/licenses/MIT.
//

#ifndef NNGCPP_MESSAGE_HEADER_H
#define NNGCPP_MESSAGE_HEADER_H

#include "../core/types.h"

#include "message_base.h"

#include "messaging_api_base.hpp"

#include <cstdint>

namespace nng {

#ifndef NNGCPP_BINARY_MESSAGE_H
    template<class Body_, class Header_> class _BasicMessage;
#endif //NNGCPP_BINARY_MESSAGE_H

    // TODO: TBD: potentially handling these as a string-based as well...
    class _HeaderMessagePart
        : public _MessagePart
        , public ISupportsGet<buffer_vector_type>
        , public ISupportsAppend<uint32_t>
        , public ISupportsAppend<const buffer_vector_type&>
        , public ISupportsAppend<const std::string&>
        , public ISupportsPrepend<uint32_t>
        , public ISupportsPrepend<const buffer_vector_type&>
        , public ISupportsPrepend<const std::string&>
        , public ISupportsTrimLeft<size_type>
        , public ISupportsTrimLeft<uint32_t*>
        , public ISupportsTrimRight<size_type>
        , public ISupportsTrimRight<uint32_t*> {
    protected:

        template<class Body_, class Header_> friend class _BasicMessage;

        _HeaderMessagePart(_MessageBase* const msgp);

    public:

        virtual ~_HeaderMessagePart();

        // TODO: TBD: we could leverage the base class type definitions; hwoever, the SWIG exposure forces us to be explicit here, unfortunately
        virtual bool TryGet(buffer_vector_type const* resultp) override;

        virtual const buffer_vector_type Get() override;

        virtual size_type GetSize() override;

        // TODO: TBD: so if header is truly "read-only" then it is debatable whether "clear" should be exposed via header...
        virtual void Clear() override;

        virtual void Append(uint32_t val) override;

        virtual void Append(const buffer_vector_type& buf) override;

        virtual void Append(const std::string& s) override;

        virtual void Prepend(uint32_t val) override;

        virtual void Prepend(const buffer_vector_type& buf) override;

        virtual void Prepend(const std::string& s) override;

        virtual void TrimLeft(size_type sz) override;

        virtual void TrimLeft(uint32_t* resultp) override;

        virtual void TrimRight(size_type sz) override;

        virtual void TrimRight(uint32_t* resultp) override;
    };

    typedef _HeaderMessagePart binary_message_header;
}

#endif // NNGCPP_MESSAGE_HEADER_H
