#ifndef NNGCPP_SESSION_H
#define NNGCPP_SESSION_H

#include "../nngcpp_integration.h"
#include "core/options.h"
#include "core/listener.h"
#include "core/dialer.h"
#include "protocol/protocol.h"

#include <memory>
#include <vector>

// TODO: TBD: considering the NNG fini teardown, it may be appropriate to emit appropriately protocoled sockets from the session as well...
namespace nng {


    class session {
        private:

            std::vector<std::shared_ptr<dialer>> _dialer_eps;
            std::vector<std::shared_ptr<listener>> _listener_eps;

            std::vector<std::shared_ptr<protocol::latest_pair_socket>> _pair_sockets;

            std::vector<std::shared_ptr<protocol::latest_push_socket>> _push_sockets;
            std::vector<std::shared_ptr<protocol::latest_pull_socket>> _pull_sockets;

        public:

            session();

            virtual ~session();

            std::shared_ptr<dialer> create_dialer_ep();
            std::shared_ptr<dialer> create_dialer_ep(const socket& s, const std::string& addr);

            std::shared_ptr<listener> create_listener_ep();
            std::shared_ptr<listener> create_listener_ep(const socket& s, const std::string& addr);

            void remove_dialer_ep(const dialer* const dp);
            void remove_listener_ep(const listener* const lp);

            std::shared_ptr<protocol::latest_pair_socket> create_pair_socket();
            void remove_pair_socket(const protocol::latest_pair_socket* const sp);

            std::shared_ptr<protocol::latest_push_socket> create_push_socket();
            std::shared_ptr<protocol::latest_pull_socket> create_pull_socket();
    };
}

#endif // NNGCPP_SESSION_H
