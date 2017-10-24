#ifndef NNGCPP_EXCEPTIONS_NOT_IMPLEMENTED_H
#define NNGCPP_EXCEPTIONS_NOT_IMPLEMENTED_H

#include <exception>
#include <string>

namespace nng {
    namespace exceptions {

        class not_implemented : public std::logic_error {
        public:

            not_implemented();

            not_implemented(const char* _Message);

            not_implemented(const std::string& _Message);

            virtual ~not_implemented();
        };
    }
}

#endif // NNGCPP_EXCEPTIONS_NOT_IMPLEMENTED_H
