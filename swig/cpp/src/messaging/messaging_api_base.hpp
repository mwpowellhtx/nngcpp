#ifndef NNGCPP_MESSAGING_API_HPP
#define NNGCPP_MESSAGING_API_HPP

#include "message_base.h"

namespace nng {

    template<typename Type_>
    class ICanGet {
    protected:

        typedef ICanGet<Type_> ICanGetType;

    public:

        typedef Type_ getter_type;

        virtual size_type GetSize() = 0;

        virtual bool TryGet(getter_type const* resulp) = 0;

        virtual const getter_type Get() {
            getter_type result;
            return TryGet(&result) ? result : getter_type();
        }
    };

    template<typename Result_, typename Intermediate_>
    struct message_getter_try_get_policy {

        typedef Result_ result_type;

        typedef Intermediate_ intermediate_type;

        typedef std::function<intermediate_type(::nng_msg* const)> get_intermediate_func;

        typedef std::function<bool(intermediate_type, result_type)> convert_result_to_func;

        // TODO: TBD: I'm not sure we really gain that much by this template specialization "policy" except perhaps a handshake with the API
        static bool TryGet(result_type resultp, ::nng_msg* const msgp
            , const get_intermediate_func& get
            , const convert_result_to_func& convert_to) {

            auto* x = get(msgp);
            return convert_to(x, resultp);

            //// TODO: TBD: exception may not be necessary here ...
            //throw trx::not_implemented("the try get policy is not implemented");
        }
    };

    template<typename Type_>
    struct ICanAppend {
        virtual void Append(Type_ val) = 0;
    };

    template<typename Type_>
    struct ICanPrepend {
        virtual void Prepend(Type_ val) = 0;
    };

    template<typename Type_>
    struct ICanTrimRight {
        virtual void TrimRight(Type_ val) = 0;
    };

    template<typename Type_>
    struct ICanTrimLeft {
        virtual void TrimLeft(Type_ val) = 0;
    };
}

#endif // NNGCPP_MESSAGING_API_HPP
