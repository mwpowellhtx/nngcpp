#include "invalid_operation.h"

namespace trx {

    invalid_operation::invalid_operation() : logic_error("the operation is invalid") {
    }

    invalid_operation::invalid_operation(const std::string& message) : logic_error(message) {
    }

    invalid_operation::~invalid_operation() {
    }
}
