#ifndef NNGCPP_EXCEPTIONS_NNG_EXCEPTION_H
#define NNGCPP_EXCEPTIONS_NNG_EXCEPTION_H

#include "../enums.h"

// nng should be in the include path.
#include <exception>
#include <string>

namespace nng {


    namespace exceptions {

        class nng_exception : public std::exception {
        public:

            typedef uint32_t error_type;

            const error_code_type error_code;

        public:

            nng_exception();

            nng_exception(uint32_t errnum);

            nng_exception(error_code_type ec);

            virtual ~nng_exception();
        };
    }
}


#endif // NNGCPP_EXCEPTIONS_NNG_EXCEPTION_H
