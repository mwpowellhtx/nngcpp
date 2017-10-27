#ifndef NNGCPP_MESSAGING_API_HPP
#define NNGCPP_MESSAGING_API_HPP

#include "message_base.h"

namespace nng {

    template<typename Type_>
    class ISupportsGet {
    protected:

        typedef Type_ getter_type;

        typedef ISupportsGet<Type_> ISupportsGetType;

    public:

        virtual size_type get_size() = 0;

        virtual bool try_get(getter_type& value) = 0;

        virtual const getter_type get() {
            getter_type result;
            return try_get(result) ? result : getter_type();
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

    template<typename Type_>
    struct ISupportsAppend {
        virtual void Append(Type_ val) = 0;
    };

    template<typename Type_>
    struct ISupportsPrepend {
        virtual void Prepend(Type_ val) = 0;
    };

    template<typename Type_>
    struct ISupportsTrimRight {
        virtual void TrimRight(Type_ val) = 0;
    };

    template<typename Type_>
    struct ISupportsTrimLeft {
        virtual void TrimLeft(Type_ val) = 0;
    };
}

#endif // NNGCPP_MESSAGING_API_HPP
