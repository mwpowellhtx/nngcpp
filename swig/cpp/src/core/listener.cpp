#include "listener.h"
#include "socket.h"
#include "invocation.hpp"

namespace nng {

    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;
    using std::bind;

    // TODO: TBD: is "listener" its own thing? or simply another kind of "socket"? i.e. perhaps a receive-only socket, as the name would suggest
    listener::listener() : _EndPoint(), lid(0) {
        configure_options(lid);
    }

    listener::listener(const socket* const sp, const std::string& addr) : _EndPoint(), lid(0) {

        const auto op = bind(&::nng_listener_create, &lid, sp->sid, _1);
        invocation::with_default_error_handling(op, addr.c_str());
        configure_options(lid);
    }

    listener::~listener() {
        Close();
    }

    listener::options_type* const listener::GetOptions() {
        return _EndPoint::GetOptions();
    }

    void listener::start(flag_type flags) {
        const auto op = bind(&::nng_listener_start, lid, _1);
        invocation::with_default_error_handling(op, static_cast<int>(flags));
    }

    void listener::Close() {
        if (!HasOne()) { return; }
        const auto op = bind(&::nng_listener_close, lid);
        invocation::with_default_error_handling(op);
        configure_options(lid = 0);
    }

    bool listener::HasOne() const {
        return lid != 0;
    }

    void listener::on_listened() {
        configure_options(lid);
    }

    void listener::configure_options(nng_type lid) {

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
}
