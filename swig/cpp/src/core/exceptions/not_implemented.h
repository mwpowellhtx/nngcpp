#ifndef NNGCPP_TRX_NOT_IMPLEMENTED_EXCEPTION_H
#define NNGCPP_TRX_NOT_IMPLEMENTED_EXCEPTION_H

#include <exception>
#include <string>

namespace trx {

    class not_implemented : public std::logic_error {
    private:

        std::string _message;

    public:

        not_implemented();

        not_implemented(const std::string& message);

        virtual ~not_implemented();
    };
}

#endif // NNGCPP_CORE_NOT_IMPLEMENTED_EXCEPTION_H
