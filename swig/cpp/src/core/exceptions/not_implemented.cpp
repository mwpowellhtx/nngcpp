#include "not_implemented.h"

namespace trx {

    not_implemented::not_implemented() : logic_error("function is not implemented") {
    }

    not_implemented::not_implemented(const std::string& message) : logic_error(message) {
    }

    not_implemented::~not_implemented() {
    }
}
