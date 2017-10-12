#include "binary_message_header.h"

namespace nng {

    namespace messaging {

        using std::placeholders::_1;
        using std::placeholders::_2;
        using std::placeholders::_3;

        binary_message_header::binary_message_header() : readonly_messaging_api() {
        }

        binary_message_header::binary_message_header(::nng_msg* msgp) : readonly_messaging_api(msgp) {
        }

        binary_message_header::~binary_message_header() {
        }

        const message_base::buffer_vector_type binary_message_header::get() const {

            // TODO: TBD: but for the binding, this could be a useful templatized function.
            const size_type sz = get_size();

            const auto op = std::bind(&::nng_msg_header, _1);
            part_type data = op(_msgp);

            // TODO: TBD: string may not be a great assumption in the long run; rather we would want binary. then leave it to layers closer to the application whether buffers were string or not.
            message_base::buffer_vector_type buf;

            for (size_type i = 0; i < sz; i++) {
                const auto x = (((std::string::value_type*)data) + i)[0];
                buf.push_back(x);
            }

            return buf;
        }

        message_base::size_type binary_message_header::get_size() const {
            return ::nng_msg_header_len(_msgp);
        }

        void binary_message_header::clear() {
            ::nng_msg_header_clear(_msgp);
        }

        bool binary_message_header::has_message() const {
            // Which re-exposes has_message for public consumption.
            return message_base::has_message();
        }

        void binary_message_header::append(const message_base::buffer_vector_type& buf) {
            static const auto op = std::bind(&::nng_msg_header_append, _1, _2, _3);
            do_type_based_op(op, _msgp, buf);
        }

        void binary_message_header::insert(const message_base::buffer_vector_type& buf) {
            static const auto op = std::bind(&::nng_msg_header_insert, _1, _2, _3);
            do_type_based_op(op, _msgp, buf);
        }

        void binary_message_header::trim(size_type sz) {
            static const auto op = std::bind(&::nng_msg_header_trim, _1, _2);
            do_size_based_op(op, _msgp, sz);
        }

        void binary_message_header::chop(size_type sz) {
            static const auto op = std::bind(&::nng_msg_header_chop, _1, _2);
            do_size_based_op(op, _msgp, sz);
        }
    }
}
