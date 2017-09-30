#include "nng.h"
#include "session.h"

namespace nng {

    session::session() {
    }

    session::~session() {
        ::nng_fini();
    }
}
