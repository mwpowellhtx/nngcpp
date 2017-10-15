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
            , _header()
            , _body() {

            allocate();
        }

        binary_message::binary_message(size_type sz)
            : message_base()
            , _header()
            , _body() {

            allocate(sz);
        }

        binary_message::binary_message(::nng_msg* msgp)
            : message_base(msgp)
            , _header(msgp)
            , _body(msgp) {

            allocate();
        }

        binary_message::binary_message(::nng_msg* msgp, size_type sz)
            : message_base(msgp)
            , _header(msgp)
            , _body(msgp) {

            allocate(sz);
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
            if (!has_message()) { allocate(); }
            return &_header;
        }

        binary_message_body* const binary_message::body() {
            if (!has_message()) { allocate(); }
            return &_body;
        }

        message_base::size_type binary_message::get_size() {
            // Should be both (Header.Size + Body.Size).
            if (!has_message()) { allocate(); }
            return _header.get_size() + _body.get_size();
        }

        void binary_message::clear() {
            if (!has_message()) { return; }
            _header.clear();
            _body.clear();
        }

        void binary_message::set_msgp(::nng_msg* msgp) {
            // Frees any previously existing internal message beforehand.
            message_base::set_msgp(msgp);
            _header.set_msgp(msgp);
            _body.set_msgp(msgp);
        }

        void binary_message::allocate(size_type sz) {
            if (has_message()) { return; }
            const auto op = std::bind(::nng_msg_alloc, _1, _2);
            ::nng_msg* msgp;
            const auto errnum = op(&msgp, sz);
            THROW_NNG_EXCEPTION_EC(errnum);
            binary_message::set_msgp(msgp);
        }

        void binary_message::free() {
            if (!has_message()) { return; }
            const auto op = std::bind(::nng_msg_free, _1);
            op(_msgp);
            binary_message::set_msgp(nullptr);
        }

        void binary_message::set_pipe(const message_pipe* const mpp) {
            if (mpp == nullptr) { return; }
            if (!has_message()) { allocate(); }
            // This is another convenience moment: allocate beforehand, if necessary.
            // TODO: TBD: there is no return value here unfortunately... perhaps there should be?
            const auto op = std::bind(&::nng_msg_set_pipe, _1, _2);
            op(_msgp, mpp->pid);
        }
    }
}
