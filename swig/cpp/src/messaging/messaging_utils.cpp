#include "messaging_utils.h"

namespace nng {

    namespace messaging {

        messaging_utils::messaging_utils() {
        }

        messaging_utils::~messaging_utils() {
        }

        template<class Src_, class Dest_>
        Dest_ __to_dest(const Src_* const sp, size_type sz) {
            Dest_ d;
            typedef Dest_::value_type dest_value_type;
            // TODO: TBD: may need/want to account for unicode wide-characters.
            for (auto it = sp->cbegin(); ((size_type)(it - sp->cbegin())) < sz
                && it != sp->cend(); it++) {
                d.push_back((dest_value_type)*it);
            }
            return d;
        }

        std::string messaging_utils::to_string(const buffer_vector_type* const bufp) {
            return bufp == nullptr ? std::string() : to_string(bufp, bufp->size());
        }

        std::string messaging_utils::to_string(const buffer_vector_type* const bufp, size_type sz) {
            return __to_dest<buffer_vector_type, std::string>(bufp, sz);
        }

        buffer_vector_type messaging_utils::to_buffer(const std::string::value_type* cp) {
            const std::string s(cp);
            return to_buffer(&s, s.length());
        }

        buffer_vector_type messaging_utils::to_buffer(const std::string::value_type* cp, size_type sz) {
            const std::string s(cp);
            return to_buffer(&s, sz);
        }

        buffer_vector_type messaging_utils::to_buffer(const std::string* const sp) {
            return sp == nullptr ? buffer_vector_type() : to_buffer(sp, sp->length());
        }

        buffer_vector_type messaging_utils::to_buffer(const std::string* const sp, size_type sz) {
            return __to_dest<std::string, buffer_vector_type>(sp, sz);
        }
    }
}
