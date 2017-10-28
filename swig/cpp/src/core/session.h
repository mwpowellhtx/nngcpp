#ifndef NNGCPP_SESSION_H
#define NNGCPP_SESSION_H

#include "../messaging/messaging.h"
#include "../protocol/protocol.h"

#include "address.h"
#include "listener.h"
#include "dialer.h"
#include "device.h"
#include "../options/options.h"

#include <memory>
#include <vector>

// TODO: TBD: considering the NNG fini teardown, it may be appropriate to emit appropriately protocoled sockets from the session as well...
namespace nng {


    class session {
        private:

            std::vector<std::shared_ptr<dialer>> _dialer_eps;
            std::vector<std::shared_ptr<listener>> _listener_eps;

            std::vector<std::shared_ptr<protocol::latest_bus_socket>> _bus_sockets;

            std::vector<std::shared_ptr<protocol::latest_pair_socket>> _pair_sockets;

            std::vector<std::shared_ptr<protocol::latest_push_socket>> _push_sockets;
            std::vector<std::shared_ptr<protocol::latest_pull_socket>> _pull_sockets;

            std::vector<std::shared_ptr<protocol::latest_req_socket>> _req_sockets;
            std::vector<std::shared_ptr<protocol::latest_rep_socket>> _rep_sockets;

            std::vector<std::shared_ptr<binary_message>> _messages;

            std::vector<std::shared_ptr<device>> _devices;

        public:

            session();

            virtual ~session();

            // TODO: TBD: for now leave it as-is; eventually convert from (const socket&) to (const socket* const) ...
            std::shared_ptr<dialer> create_dialer_ep();
            std::shared_ptr<dialer> create_dialer_ep(const socket& s, const std::string& addr);

            std::shared_ptr<listener> create_listener_ep();
            std::shared_ptr<listener> create_listener_ep(const socket& s, const std::string& addr);

            void remove_dialer_ep(const dialer* const dp);
            void remove_listener_ep(const listener* const lp);

            std::shared_ptr<protocol::latest_bus_socket> create_bus_socket();
            void remove_bus_socket(const protocol::latest_bus_socket* const sp);

            std::shared_ptr<protocol::latest_pair_socket> create_pair_socket();
            void remove_pair_socket(const protocol::latest_pair_socket* const sp);

            std::shared_ptr<protocol::latest_push_socket> create_push_socket();
            std::shared_ptr<protocol::latest_pull_socket> create_pull_socket();

            std::shared_ptr<protocol::latest_req_socket> create_req_socket();
            void remove_req_socket(const protocol::latest_req_socket* const rp);

            std::shared_ptr<protocol::latest_rep_socket> create_rep_socket();
            void remove_rep_socket(const protocol::latest_rep_socket* const rp);

            std::shared_ptr<device> create_device(socket* const asockp, socket* const bsockp, bool shouldCloseSockets);
            void remove_device(const device* const dp);

            std::shared_ptr<binary_message> create_message();
            void remove_message(const binary_message* const mp);
    };
}

#endif // NNGCPP_SESSION_H
