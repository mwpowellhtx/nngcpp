#ifndef NNGCPP_SESSION_H
#define NNGCPP_SESSION_H

#include "../nngcpp_integration.h"
#include "core/options.h"
#include "protocol/protocol.h"

#include <vector>
#include <memory>

// TODO: TBD: considering the NNG fini teardown, it may be appropriate to emit appropriately protocoled sockets from the session as well...
namespace nng {

    class listener;

    class session {
        private:

            std::vector<std::shared_ptr<protocol::latest_push_socket>> _push_sockets;
            std::vector<std::shared_ptr<protocol::latest_pull_socket>> _pull_sockets;

            std::vector<std::shared_ptr<listener>> _listeners;

        public:

            session();

            virtual ~session();

            std::shared_ptr<protocol::latest_push_socket> create_push_socket();
            std::shared_ptr<protocol::latest_pull_socket> create_pull_socket();

            std::shared_ptr<listener> create_listener();
            void remove_listener(const listener* const lp);
    };
}

#endif // NNGCPP_SESSION_H
