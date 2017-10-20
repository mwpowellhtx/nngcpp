#ifndef NNGCPP_MESSAGING_API_HPP
#define NNGCPP_MESSAGING_API_HPP

#include "message_base.h"

#include <memory>
#include <functional>

#ifndef THROW_API_IS_READ_ONLY
#define THROW_API_IS_READ_ONLY() throw trx::not_implemented("API is read-only")
#endif //THROW_API_IS_READ_ONLY

namespace nng {

    namespace messaging {

        class message_header;
        class message_body;

        template<typename Type_>
        class supports_get_api {
        protected:

            typedef Type_ getter_type;

            typedef supports_get_api<Type_> getter_api_type;

        public:

            virtual size_type get_size() = 0;

            virtual bool try_get(Type_& value) = 0;

            virtual const Type_ get() {
                Type_ result;
                return try_get(result) ? result : Type_();
            }
        };

        template<typename Result_, typename Intermediate_>
        struct message_getter_try_get_policy {

            typedef Result_ result_type;

            typedef Intermediate_ intermediate_type;

            typedef std::function<intermediate_type(::nng_msg* const)> get_intermediate_func;

            typedef std::function<bool(intermediate_type, result_type&)> convert_result_to_func;

            // TODO: TBD: I'm not sure we really gain that much by this template specialization "policy" except perhaps a handshake with the API
            static bool try_get(result_type& result, ::nng_msg* const msgp
                , const get_intermediate_func& get
                , const convert_result_to_func& convert_to) {

                auto* x = get(msgp);
                return convert_to(x, result);

                //// TODO: TBD: exception may not be necessary here ...
                //throw trx::not_implemented("the try get policy is not implemented");
            }
        };

        template<typename... Args_> struct supports_append_api;

        template<typename Type_, typename... Args_>
        struct supports_append_api<Type_, Args_...> {
        };

        template<typename Type_>
        struct supports_append_api<Type_> {
            virtual void append(const Type_& arg) = 0;
        };

        template<typename... Args_> struct supports_insert_api;

        template<typename Type_, typename... Args_>
        struct supports_insert_api<Type_, Args_...> {};

        template<typename Type_>
        struct supports_insert_api<Type_> {
            virtual void insert(const Type_& arg) = 0;
        };

        template<typename... Type_> struct supports_chop_api;

        template<typename Type_, typename... Args_>
        struct supports_chop_api<Type_, Args_...> {};

        template<typename Type_>
        struct supports_chop_api<Type_> {
            virtual void chop(Type_ arg) = 0;
        };

        template<typename... Args_> struct supports_trim_api;

        template<typename Type_, typename... Args_>
        struct supports_trim_api<Type_, Args_...> {};

        template<typename Type_>
        struct supports_trim_api<Type_> {
            virtual void trim(Type_ arg) = 0;
        };
    }
}

#endif // NNGCPP_MESSAGING_API_HPP
