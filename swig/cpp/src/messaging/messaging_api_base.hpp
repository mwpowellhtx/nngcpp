#ifndef NNGCPP_MESSAGING_API_HPP
#define NNGCPP_MESSAGING_API_HPP

#include "message_base.h"
#include "../core/nng_exception.h"

#include <memory>
#include <functional>

namespace nng {

    namespace messaging {

        class message_header;
        class message_body;

        template<typename Type_>
        class messaging_api : public message_base {
        protected:

            messaging_api() : message_base() {}

            messaging_api(::nng_msg* msgp) : message_base(msgp) {};

        protected:

            typedef std::function<int(::nng_msg*, const message_base::part_type, message_base::size_type)> type_based_op;

            static void do_type_based_op(const type_based_op& op, ::nng_msg* msgp, const Type_& x) {
                if (msgp == nullptr) { return; }
                const auto* _x_data = x.data();
                const auto _x_sz = x.size();
                // Be careful of the operation. We want to expand errnum in the macro, NOT the operation itself.
                const auto errnum = op(msgp, (const message_base::part_type)_x_data, _x_sz);
                THROW_NNG_EXCEPTION_EC(errnum);
            }

            typedef std::function<int(::nng_msg*, message_base::size_type)> size_based_op;

            static void do_size_based_op(const size_based_op& op, ::nng_msg* msgp, message_base::size_type sz) {
                // Ditto above. Be careful of the macro expansion.
                const auto errnum = op(msgp, sz);
                THROW_NNG_EXCEPTION_EC(errnum);
            }

        public:

            virtual ~messaging_api() {}

            virtual const Type_ get() const = 0;

            virtual void append(const Type_& x) = 0;

            virtual void insert(const Type_& x) = 0;

            virtual void trim(size_type sz) = 0;

            virtual void chop(size_type sz) = 0;
        };

        template<typename Type_>
        class readonly_messaging_api : protected messaging_api<Type_> {
        protected:

            readonly_messaging_api() : messaging_api() {}

            readonly_messaging_api(::nng_msg* msgp) : messaging_api(msgp) {}

        public:

            virtual ~readonly_messaging_api() {}
        };
    }
}

#endif // NNGCPP_MESSAGING_API_HPP
