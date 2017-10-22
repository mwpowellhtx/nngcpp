#ifndef NNGCPP_BINARY_MESSAGE_H
#define NNGCPP_BINARY_MESSAGE_H

#include "../core/types.h"
#include "../core/exceptions.hpp"

#include "message_pipe.h"
#include "binary_message_header.h"
#include "binary_message_body.h"

#include <string>

namespace nng {

    namespace messaging {

#ifndef NNGCPP_MESSAGE_PIPE_H
        class message_pipe;
#endif //NNGCPP_MESSAGE_PIPE_H

        // TODO: TBD: of course they are all "binary" messages close to the wire; however, we may shim a string-based API around this one...
        template<class Body_, class Header_>
        class basic_binary_message : public message_base {
        public:

            typedef Body_ body_type;

            typedef Header_ header_type;

            typedef basic_binary_message<Body_, Header_> basic_binary_message_type;

        private:

            friend class socket;

            ::nng_msg* _msgp;

            header_type _header;

            body_type _body;

        protected:

            basic_binary_message()
                : message_base()
                , _msgp(nullptr)
                , _header(this)
                , _body(this) {

                allocate();
            }

            basic_binary_message(size_type sz)
                : message_base()
                , _msgp(nullptr)
                , _header(this)
                , _body(this) {

                allocate(sz);
            }

            basic_binary_message(::nng_msg* msgp)
                : message_base()
                , _msgp(msgp)
                , _header(this)
                , _body(this) {
            }

        public:

            virtual ~basic_binary_message() { free(); }

            header_type* const header() {
                if (!has_one()) { allocate(); }
                return &_header;
            }

            body_type* const body() {
                if (!has_one()) { allocate(); }
                return &_body;
            }

            virtual ::nng_msg* get_msgp() override {
                return _msgp;
            }

            virtual void set_msgp(::nng_msg* msgp) {
                // Yes, do free whatever message we previously had.
                free();
                // Then set the message, however was intended, which may also be nullptr.
                _msgp = msgp;
            }

            virtual void on_sent() {
                _msgp = nullptr;
            }

            virtual size_type get_size() {
                // Should be both (Header.Size + Body.Size).
                if (!has_one()) { allocate(); }
                return _header.get_size() + _body.get_size();
            }

            virtual void clear() override {
                if (!has_one()) { return; }
                _header.clear();
                _body.clear();
            }

            virtual void allocate(size_type sz = 0) {
                if (has_one()) { return; }
                ::nng_msg* msgp;
                try {
                    using std::placeholders::_1;
                    const auto op = std::bind(&::nng_msg_alloc, &msgp, _1);
                    const auto errnum = op(sz);
                    THROW_NNG_EXCEPTION_EC(errnum);
                }
                catch (...) {
                    //// TODO: TBD: not sure this would be valid... or depending on the error...
                    //if (msgp) {
                    //    const auto __free = std::bind(::nng_msg_free, _1);
                    //    __free(msgp);
                    //}
                    throw;
                }
                set_msgp(msgp);
            }

            virtual void set_pipe(const message_pipe* const mpp) {
                if (mpp == nullptr) { return; }
                if (!has_one()) { allocate(); }
                using std::placeholders::_1;
                // This is another convenience moment: allocate beforehand, if necessary.
                // TODO: TBD: there is no return value here unfortunately... perhaps there should be?
                const auto op = std::bind(&::nng_msg_set_pipe, _msgp, _1);
                op(mpp->pid);
            }

        protected:

            virtual void free() {
                if (!has_one()) { return; }
                const auto op = std::bind(&::nng_msg_free, _msgp);
                op();
                // Just set the member directly since we own it.
                _msgp = nullptr;
            }

        public:

            operator std::string() {
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

            binary_message(::nng_msg* msgp);

            virtual ~binary_message();
        };
    }
}

#endif // NNGCPP_BINARY_MESSAGE_H
