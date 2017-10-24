#include "../exceptions.hpp"

namespace nng {
    namespace exceptions {

        not_implemented::not_implemented() : logic_error("function is not implemented") {
        }

        not_implemented::not_implemented(const char* _Message) : logic_error(_Message) {
        }

        not_implemented::not_implemented(const std::string& _Message) : logic_error(_Message) {
        }

        not_implemented::~not_implemented() {
        }
    }
}
