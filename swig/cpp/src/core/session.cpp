#include "session.h"

#include <algorithm>

namespace nng {

    session::session()
        : _dialer_eps()
        , _listener_eps()
        , _bus_sockets()
        , _pair_sockets()
        , _push_sockets()
        , _pull_sockets()
        , _req_sockets()
        , _rep_sockets()
        , _devices() {
    }

    template<class Type_>
    void __release_all(std::vector<std::shared_ptr<Type_>>& values) {
        for (auto & x : values) {
            x.reset();
        }
        values.clear();
    }

    session::~session() {
        __release_all(_dialer_eps);
        __release_all(_listener_eps);
        __release_all(_bus_sockets);
        __release_all(_pair_sockets);
        __release_all(_push_sockets);
        __release_all(_pull_sockets);
        __release_all(_req_sockets);
        __release_all(_rep_sockets);
        __release_all(_devices);
        ::nng_fini();
    }

    template<class Type_, typename ...Args_>
    std::shared_ptr<Type_> __create(std::vector<std::shared_ptr<Type_>>& values, Args_ &&...args) {
        auto sp = std::make_shared<Type_>(args...);
        values.push_back(sp);
        return sp;
    }

    template<class Type_>
    void __remove(std::vector<std::shared_ptr<Type_>>& values, const Type_* const valuep) {
        std::remove_if(values.begin(), values.end(), [&](const std::shared_ptr<Type_>& sp) {
            return sp.get() == valuep;
        });
    }

    std::shared_ptr<dialer> session::create_dialer_ep() {
        return __create(_dialer_eps);
    }

    std::shared_ptr<dialer> session::create_dialer_ep(const socket& s, const std::string& addr) {
        return __create(_dialer_eps, &s, addr);
    }

    std::shared_ptr<listener> session::create_listener_ep() {
        return __create(_listener_eps);
    }

    std::shared_ptr<listener> session::create_listener_ep(const socket& s, const std::string& addr) {
        return __create(_listener_eps, &s, addr);
    }

    void session::remove_dialer_ep(const dialer* const dp) {
        // Yes, in fact, we do not need the full smart pointer in this instance.
        __remove(_dialer_eps, dp);
    }

    void session::remove_listener_ep(const listener* const lp) {
        // Ditto smart pointers.
        __remove(_listener_eps, lp);
    }

    std::shared_ptr<protocol::latest_bus_socket> session::create_bus_socket() {
        return __create(_bus_sockets);
    }

    void session::remove_bus_socket(const protocol::latest_bus_socket* const sp) {
        __remove(_bus_sockets, sp);
    }

    std::shared_ptr<protocol::latest_pair_socket> session::create_pair_socket() {
        return __create(_pair_sockets);
    }

    void session::remove_pair_socket(const protocol::latest_pair_socket* const sp) {
        __remove(_pair_sockets, sp);
    }

    std::shared_ptr<protocol::latest_push_socket> session::create_push_socket() {
        return __create(_push_sockets);
    }

    std::shared_ptr<protocol::latest_pull_socket> session::create_pull_socket() {
        return __create(_pull_sockets);
    }

    std::shared_ptr<protocol::latest_req_socket> session::create_req_socket() {
        return __create(_req_sockets);
    }

    void session::remove_req_socket(const protocol::latest_req_socket* const rp) {
        __remove(_req_sockets, rp);
    }

    void session::remove_rep_socket(const protocol::latest_rep_socket* const rp) {
        __remove(_rep_sockets, rp);
    }

    std::shared_ptr<protocol::latest_rep_socket> session::create_rep_socket() {
        return __create(_rep_sockets);
    }

    std::shared_ptr<device> session::create_device(socket* const asockp, socket* const bsockp, bool shouldCloseSockets) {
        return __create(_devices, asockp, bsockp, shouldCloseSockets);
    }

    void session::remove_device(const device* const dp) {
        __remove(_devices, dp);
    }

    // TODO: TBD: string based? or vector based?
    std::shared_ptr<binary_message> session::create_message() {
        return __create(_messages);
    }

    void session::remove_message(const binary_message* const mp) {
        __remove(_messages, mp);
    }
}
