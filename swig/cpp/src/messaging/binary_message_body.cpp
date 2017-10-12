#include "binary_message_body.h"

namespace nng {

    namespace messaging {

        using std::placeholders::_1;
        using std::placeholders::_2;
        using std::placeholders::_3;

        binary_message_body::binary_message_body() : messaging_api() {
        }

        binary_message_body::binary_message_body(::nng_msg* msgp) : messaging_api(msgp) {
        }

        binary_message_body::~binary_message_body() {
        }

        const binary_message_body::buffer_vector_type binary_message_body::get() const {

            // TODO: TBD: string may not be a great assumption in the long run; rather we would want binary. then leave it to layers closer to the application whether buffers were string or not.
            buffer_vector_type buf;

            const size_type sz = get_size();

            // TODO: TBD: but for the binding, this could be a useful templatized function.
            part_type data = ::nng_msg_body(_msgp);

            auto src = (buffer_vector_type::value_type*) data;

            for (size_type i = 0; i < sz; i++) {
                buf.push_back((src + i)[0]);
            }

            return buf;
        }

        binary_message_body::size_type binary_message_body::get_size() const {
            return _msgp == nullptr ? 0 : ::nng_msg_len(_msgp);
        }

        void binary_message_body::clear() {
            ::nng_msg_clear(_msgp);
        }

        void binary_message_body::append(const buffer_vector_type& buf) {
            static const auto op = std::bind(&::nng_msg_append, _1, _2, _3);
            do_type_based_op(op, _msgp, buf);
        }

        void binary_message_body::insert(const buffer_vector_type& buf) {
            if (_msgp == nullptr) { return; }
            static const auto op = std::bind(&::nng_msg_insert, _1, _2, _3);
            do_type_based_op(op, _msgp, buf);
        }

        void binary_message_body::trim(size_type sz) {
            if (_msgp == nullptr) { return; }
            static const auto op = std::bind(&::nng_msg_trim, _1, _2);
            do_size_based_op(op, _msgp, sz);
        }

        void binary_message_body::chop(size_type sz) {
            if (_msgp == nullptr) { return; }
            static const auto op = std::bind(&::nng_msg_chop, _1, _2);
            do_size_based_op(op, _msgp, sz);
        }
    }
}
