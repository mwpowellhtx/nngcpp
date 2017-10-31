#include "survey.h"

namespace nng {
    namespace protocol {
        namespace v0 {

            using std::placeholders::_1;

            // While we could use nng_survey_open, I think it is sufficient to just use the versioned symbol.
            survey_socket::survey_socket() : _Socket(std::bind(&(::nng_surveyor0_open), _1)) {
            }

            survey_socket::~survey_socket() {
            }
        }
    }
}
