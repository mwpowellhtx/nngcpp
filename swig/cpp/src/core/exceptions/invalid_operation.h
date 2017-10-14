#ifndef NNGCPP_TRX_INVALID_OPERATION_EXCEPTION_H
#define NNGCPP_TRX_INVALID_OPERATION_EXCEPTION_H

#include <exception>
#include <string>

namespace trx {

    class invalid_operation : public std::logic_error {
    private:

        std::string _message;

    public:

        invalid_operation();

        invalid_operation(const std::string& message);

        virtual ~invalid_operation();
    };
}

#endif // NNGCPP_TRX_INVALID_OPERATION_EXCEPTION_H
