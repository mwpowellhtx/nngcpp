#ifndef NNGCPP_BINARY_MESSAGE_H
#define NNGCPP_BINARY_MESSAGE_H

#include "../core/types.h"
#include "../core/invocation.hpp"

#include "message_pipe.h"
#include "binary_message_header.h"
#include "binary_message_body.h"

#include <string>

namespace nng {

#ifndef NNGCPP_MESSAGE_PIPE_H
    class message_pipe;
#endif //NNGCPP_MESSAGE_PIPE_H

    // TODO: TBD: of course they are all "binary" messages close to the wire; however, we may shim a string-based API around this one...
    template<class Body_, class Header_>
    class basic_binary_message : public _MessageBase {
    public:

        typedef Header_ header_type;
        typedef Body_ body_type;

    private:

        friend class socket;

        header_type _header;
        body_type _body;

    protected:

        basic_binary_message()
            : _MessageBase()
            , _header(this), _body(this) {
        }

        basic_binary_message(size_type sz)
            : _MessageBase(sz)
            , _header(this), _body(this) {
        }

        basic_binary_message(msg_type* msgp)
            : _MessageBase(msgp)
            , _header(this), _body(this) {
        }

    public:

        virtual ~basic_binary_message() {
        }

        virtual header_type* const header() {
            on_one_required();
            return &_header;
        }

        virtual body_type* const body() {
            on_one_required();
            return &_body;
        }

        virtual size_type get_size() {
            // Should be both (Header.Size + Body.Size).
            on_one_required();
            return _header.get_size() + _body.get_size();
        }

        virtual void Clear() override {
            if (!HasOne()) { return; }
            _header.Clear();
            _body.Clear();
        }

        virtual operator std::string() {
            std::string s;
            const auto buf = body()->get();
            for (auto it = buf.begin(); it != buf.end(); it++) {
                s.push_back(((std::string::value_type*)*it)[0]);
            }
            return s;
        }
    };

    // TODO: TBD: any reason to define a full on class derivation? perhaps for SWIG purposes? or just type-define it?
    class binary_message : public basic_binary_message<binary_message_body, binary_message_header> {
    public:

        binary_message();

        binary_message(size_type sz);

        binary_message(msg_type* msgp);

        virtual ~binary_message();
    };
}

#endif // NNGCPP_BINARY_MESSAGE_H
