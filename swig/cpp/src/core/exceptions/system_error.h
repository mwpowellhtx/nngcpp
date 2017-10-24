#ifndef NNGCPP_EXCEPTIONS_SYSTEM_ERROR_H
#define NNGCPP_EXCEPTIONS_SYSTEM_ERROR_H

#include <exception>
#include <string>

namespace nng {
    namespace exceptions {

        class system_error : public std::runtime_error {
        public:

            const int32_t raw_number;

            const int32_t error_number;

            system_error(int32_t errnum);

            system_error(int32_t errnum, const char* _Message);

            system_error(int32_t errnum, const std::string& message);

            virtual ~system_error();
        };
    }
}

#endif // NNGCPP_EXCEPTIONS_SYSTEM_ERROR_H
