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
    template<class Body_, class Header_> class basic_binary_message;
#endif //NNGCPP_BINARY_MESSAGE_H

    // TODO: TBD: potentially handling these as a string-based as well...
    class _HeaderMessagePart
        : public _MessagePart
        , public ISupportsGet<std::vector<uint8_t>>
        , public ISupportsAppend<uint32_t>
        , public ISupportsAppend<const std::vector<uint8_t>&>
        , public ISupportsAppend<const std::string&>
        , public ISupportsPrepend<uint32_t>
        , public ISupportsPrepend<const std::vector<uint8_t>&>
        , public ISupportsPrepend<const std::string&>
        , public ISupportsTrimLeft<size_type>
        , public ISupportsTrimLeft<uint32_t&>
        , public ISupportsTrimRight<size_type>
        , public ISupportsTrimRight<uint32_t&> {
    protected:

        template<class Body_, class Header_> friend class basic_binary_message;

        _HeaderMessagePart(_MessageBase* const msgp);

    public:

        virtual ~_HeaderMessagePart();

        virtual bool try_get(std::vector<uint8_t>& value) override;

        virtual size_type get_size() override;

        // TODO: TBD: so if header is truly "read-only" then it is debatable whether "clear" should be exposed via header...
        virtual void Clear() override;

        virtual void Append(uint32_t val) override;

        virtual void Append(const std::vector<uint8_t>& buf) override;

        virtual void Append(const std::string& s) override;

        virtual void Prepend(uint32_t val) override;

        virtual void Prepend(const std::vector<uint8_t>& buf) override;

        virtual void Prepend(const std::string& s) override;

        virtual void TrimLeft(uint32_t& val) override;

        virtual void TrimLeft(size_type sz) override;

        virtual void TrimRight(uint32_t& val) override;

        virtual void TrimRight(size_type sz) override;
    };

    typedef _HeaderMessagePart binary_message_header;
}

#endif // NNGCPP_MESSAGE_HEADER_H
