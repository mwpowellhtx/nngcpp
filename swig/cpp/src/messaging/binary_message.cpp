#include "binary_message.h"
#include "message_pipe.h"

#include "../core/exceptions.hpp"

#include <functional>

namespace nng {

    namespace messaging {

        using std::placeholders::_1;
        using std::placeholders::_2;

        binary_message::binary_message()
            : message_base()
            , _msgp(nullptr)
            , _header(this)
            , _body(this) {

            allocate();
        }

        binary_message::binary_message(size_type sz)
            : message_base()
            , _msgp(nullptr)
            , _header(this)
            , _body(this) {

            allocate(sz);
        }

        binary_message::binary_message(::nng_msg* msgp)
            : message_base()
            , _msgp(msgp)
            , _header(this)
            , _body(this) {

            // Since the pointer is intentional, which may also be nullptr, do not allocate.
        }

        binary_message::~binary_message() {
            free();
        }

        binary_message::operator std::string() {
            std::string s;
            const auto buf = body()->get();
            for (auto it = buf.begin(); it != buf.end(); it++) {
                s.push_back(((std::string::value_type*)*it)[0]);
            }
            return s;
        }

        binary_message_header* const binary_message::header() {
            if (!has_one()) { allocate(); }
            return &_header;
        }

        binary_message_body* const binary_message::body() {
            if (!has_one()) { allocate(); }
            return &_body;
        }

        message_base::size_type binary_message::get_size() {
            // Should be both (Header.Size + Body.Size).
            if (!has_one()) { allocate(); }
            return _header.get_size() + _body.get_size();
        }

        void binary_message::clear() {
            if (!has_one()) { return; }
            _header.clear();
            _body.clear();
        }

        ::nng_msg* binary_message::get_msgp() {
            return _msgp;
        }

        void binary_message::set_msgp(::nng_msg* msgp) {
            // Yes, do free whatever message we previously had.
            free();
            // Then set the message, however was intended, which may also be nullptr.
            _msgp = msgp;
        }

        void binary_message::on_sent() {
            _msgp = nullptr;
        }

        void binary_message::allocate(size_type sz) {
            if (has_one()) { return; }
            ::nng_msg* msgp;
            try {
                const auto op = std::bind(::nng_msg_alloc, _1, _2);
                const auto errnum = op(&msgp, sz);
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

        void binary_message::free() {
            if (!has_one()) { return; }
            const auto op = std::bind(::nng_msg_free, _1);
            op(_msgp);
            // Just set the member directly since we own it.
            _msgp = nullptr;
        }

        void binary_message::set_pipe(const message_pipe* const mpp) {
            if (mpp == nullptr) { return; }
            if (!has_one()) { allocate(); }
            // This is another convenience moment: allocate beforehand, if necessary.
            // TODO: TBD: there is no return value here unfortunately... perhaps there should be?
            const auto op = std::bind(&::nng_msg_set_pipe, _1, _2);
            op(_msgp, mpp->pid);
        }
    }
}
