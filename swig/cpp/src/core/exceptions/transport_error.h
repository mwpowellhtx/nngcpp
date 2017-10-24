#ifndef NNGCPP_EXCEPTIONS_TRANSPORT_ERROR_H
#define NNGCPP_EXCEPTIONS_TRANSPORT_ERROR_H

#include <exception>
#include <string>

namespace nng {
    namespace exceptions {

        class transport_error : public std::runtime_error {
        public:

            const int32_t raw_number;

            const int32_t error_number;

            transport_error(int32_t errnum);

            transport_error(int32_t errnum, const char* _Message);

            transport_error(int32_t errnum, const std::string& message);

            virtual ~transport_error();
        };
    }
}

#endif // NNGCPP_EXCEPTIONS_TRANSPORT_ERROR_H
