#ifndef NNGCPP_BASIC_BINARY_MESSAGE_H
#define NNGCPP_BASIC_BINARY_MESSAGE_H

#include "message_base.h"

#include <string>

namespace nng {

#ifndef NNGCPP_SOCKET_H
    class _Socket;
#endif // NNGCPP_SOCKET_H

    // TODO: TBD: of course they are all "binary" messages close to the wire; however, we may shim a string-based API around this one...
    template<class Body_, class Header_>
    class _BasicMessage : public _MessageBase {
    public:

        typedef Header_ header_type;
        typedef Body_ body_type;

    private:

        friend class _Socket;

        header_type _header;
        body_type _body;

    protected:

        typedef _BasicMessage<Body_, Header_> basic_message_type;

        _BasicMessage()
            : _MessageBase()
            , _header(this), _body(this) {
        }

        _BasicMessage(size_type sz)
            : _MessageBase(sz)
            , _header(this), _body(this) {
        }

        _BasicMessage(msg_type* msgp)
            : _MessageBase(msgp)
            , _header(this), _body(this) {
        }

    public:

        virtual ~_BasicMessage() {
        }

        virtual header_type* const GetHeader() {
            on_one_required();
            return &_header;
        }

        virtual body_type* const GetBody() {
            on_one_required();
            return &_body;
        }

        virtual size_type GetSize() {
            // Should be both (Header.Size + Body.Size).
            on_one_required();
            return _header.GetSize() + _body.GetSize();
        }

        virtual void Clear() override {
            if (!HasOne()) { return; }
            _header.Clear();
            _body.Clear();
        }

        virtual operator std::string() {
            std::string s;
            const auto buf = GetBody()->Get();
            for (auto it = buf.begin(); it != buf.end(); it++) {
                s.push_back(((std::string::value_type*)*it)[0]);
            }
            return s;
        }
    };
}

#endif // NNGCPP_BASIC_BINARY_MESSAGE_H
