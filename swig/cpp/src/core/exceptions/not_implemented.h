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

#define THROW_NOT_IMPLEMENTED(__N__) \
    throw nng::exceptions::not_implemented( \
        #__N__ " is not implemented");

#define THROW_NOT_IMPLEMENTED_FUNC(__N__, __F__) \
    throw nng::exceptions::not_implemented( \
        #__F__ " is not implemented in " #__N__);

#endif // NNGCPP_EXCEPTIONS_NOT_IMPLEMENTED_H
