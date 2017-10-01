#ifndef NNGCPP_SESSION_H
#define NNGCPP_SESSION_H

#include "../nngcpp_integration.h"

// TODO: TBD: considering the NNG fini teardown, it may be appropriate to emit appropriately protocoled sockets from the session as well...
namespace nng {

    class session {
        public:

            session();

            virtual ~session();
    };
}

#endif // NNGCPP_SESSION_H
