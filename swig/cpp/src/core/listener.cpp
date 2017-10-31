#include "listener.h"
#include "socket.h"
#include "invocation.hpp"

namespace nng {

    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;
    using std::bind;

    // TODO: TBD: is "listener" its own thing? or simply another kind of "socket"? i.e. perhaps a receive-only socket, as the name would suggest
    _Listener::_Listener() : _EndPoint(), lid(0) {
        configure_options(lid);
    }

    _Listener::_Listener(const _Socket& s, const std::string& addr) : _EndPoint(), lid(0) {

        const auto op = bind(&::nng_listener_create, &lid, s.sid, _1);
        invocation::with_default_error_handling(op, addr.c_str());
        configure_options(lid);
    }

    _Listener::~_Listener() {
        Close();
    }

    void _Listener::on_listened() {
        configure_options(lid);
    }

    void _Listener::configure_options(nng_type lid) {

        // Configure the EP related bindings.
        configure_endpoint(
            bind(&::nng_listener_start, lid, _1)
            , bind(&::nng_listener_close, lid)
        );

        // Also convey the Options bindings.
        auto op = GetOptions();

        op->set_getters(
            bind(&::nng_listener_getopt, lid, _1, _2, _3)
            , bind(&::nng_listener_getopt_int, lid, _1, _2)
            , bind(&::nng_listener_getopt_size, lid, _1, _2)
            , bind(&::nng_listener_getopt_ms, lid, _1, _2)
        );

        op->set_setters(
            bind(&::nng_listener_setopt, lid, _1, _2, _3)
            , bind(&::nng_listener_setopt_int, lid, _1, _2)
            , bind(&::nng_listener_setopt_size, lid, _1, _2)
            , bind(&::nng_listener_setopt_ms, lid, _1, _2)
        );
    }

    bool _Listener::HasOne() const {
        return lid != 0;
    }

    void _Listener::Start() {
        Start(flag_none);
    }

    void _Listener::Start(SocketFlag flags) {
        invocation::with_default_error_handling(__start, flags);
    }

    void _Listener::Close() {
        if (!HasOne()) { return; }
        invocation::with_default_error_handling(__close);
        configure_options(lid = 0);
    }

    _Listener::options_type* const _Listener::GetOptions() {
        return have_options_type::GetOptions();
    }
}
