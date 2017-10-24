#ifndef NNGCPP_EXCEPTIONS_INVALID_OPERATION_H
#define NNGCPP_EXCEPTIONS_INVALID_OPERATION_H

#include <exception>
#include <string>

namespace nng {
    namespace exceptions {

        class invalid_operation : public std::logic_error {
        public:

            invalid_operation();

            invalid_operation(const char* _Message);

            invalid_operation(const std::string& _Message);

            virtual ~invalid_operation();
        };
    }
}

#endif // NNGCPP_EXCEPTIONS_INVALID_OPERATION_H
