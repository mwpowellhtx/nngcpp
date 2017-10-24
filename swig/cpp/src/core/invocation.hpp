 #ifndef NNGCPP_INVOCATION_HPP
 #define NNGCPP_INVOCATION_HPP

#include "exceptions.hpp"

#include <vector>

 namespace nng {
     namespace invocation {

         template<typename Op_, typename... Args_>
         void with_default_error_handling(const Op_& op, Args_... args) {
             const auto errnum = op(args...);
             THROW_NNG_EXCEPTION_EC(errnum);
         }

         template<typename Op_, typename... Args_>
         void with_error_handling_if_not_one_of(const Op_& op, const std::vector<error_code_type>& ecs, Args_... args) {
             const auto errnum = op(args...);
             THROW_NNG_EXCEPTION_IF_NOT_ONE_OF(errnum, ecs);
         }

         template<typename Op_, typename... Args_>
         void with_void_return_value(const Op_& op, Args_... args) {
             op(args...);
         }

         template<typename Op_, typename Result_, typename... Args_>
         void with_result(const Op_& op, Result_* const resultp,  Args_... args) {
             if (!resultp) { return; }
             *resultp = op(args...);
         }
     }
 }

#endif // NNGCPP_INVOCATION_HPP
