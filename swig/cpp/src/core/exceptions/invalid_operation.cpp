#include "../exceptions.hpp"

namespace nng {
    namespace exceptions {

        invalid_operation::invalid_operation() : logic_error("the operation is invalid") {
        }

        invalid_operation::invalid_operation(const char* _Message) : logic_error(_Message) {
        }

        invalid_operation::invalid_operation(const std::string& _Message) : logic_error(_Message) {
        }

        invalid_operation::~invalid_operation() {
        }
    }
}