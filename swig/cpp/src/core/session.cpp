#include "session.h"
#include "core/listener.h"

#include <algorithm>

namespace nng {

    session::session()
        : _push_sockets()
        , _pull_sockets()
        , _listeners() {
    }

    template<class Type_>
    void __release_all(std::vector<std::shared_ptr<Type_>>& values) {
        for (auto & x : values) {
            x.reset();
        }
        values.clear();
    }

    session::~session() {
        __release_all(_push_sockets);
        __release_all(_pull_sockets);
        __release_all(_listeners);
        ::nng_fini();
    }

    template<class Type_>
    std::shared_ptr<Type_> __create(std::vector<std::shared_ptr<Type_>>& values) {
        auto sp = std::make_shared<Type_>();
        values.push_back(sp);
        return sp;
    }

    template<class Type_>
    void __remove(std::vector<std::shared_ptr<Type_>>& values, const Type_* const valuep) {
        const auto __where= [&](std::shared_ptr<Type_> x) {
            return x.get() == valuep;
        };
        std::remove_if(values.begin(), values.end(), __where);
    }

    std::shared_ptr<protocol::latest_push_socket> session::create_push_socket() {
        return __create(_push_sockets);
    }

    std::shared_ptr<protocol::latest_pull_socket> session::create_pull_socket() {
        return __create(_pull_sockets);
    }

    std::shared_ptr<listener> session::create_listener() {
        return __create(_listeners);
    }

    void session::remove_listener(const listener* const lp) {
        // Yes, in fact, we do not need the full smart pointer in this instance.
        __remove(_listeners, lp);
    }
}
