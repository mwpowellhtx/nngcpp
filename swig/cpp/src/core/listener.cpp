#include "listener.h"
#include "socket.h"
#include "invocation.hpp"

namespace nng {

    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;
    using std::bind;

    // TODO: TBD: is "listener" its own thing? or simply another kind of "socket"? i.e. perhaps a receive-only socket, as the name would suggest
    listener::listener() : endpoint(), lid(0), _options() {
        configure_options(lid);
    }

    listener::listener(const socket* const sp, const std::string& addr)
        : endpoint(), lid(0), _options() {

        const auto op = bind(&::nng_listener_create, &lid, sp->sid, _1);
        invocation::with_default_error_handling(op, addr.c_str());
        configure_options(lid);
    }

    listener::~listener() {
        close();
    }

    void listener::start(flag_type flags) {
        const auto op = bind(&::nng_listener_start, lid, _1);
        invocation::with_default_error_handling(op, static_cast<int>(flags));
    }

    void listener::close() {
        if (!has_one()) { return; }
        const auto op = bind(&::nng_listener_close, lid);
        invocation::with_default_error_handling(op);
        configure_options(lid = 0);
    }

    bool listener::has_one() const {
        return lid != 0;
    }

    void listener::on_listened() {
        configure_options(lid);
    }

    void listener::configure_options(nng_type lid) {

        _options.set_getters(
            bind(&::nng_listener_getopt, lid, _1, _2, _3)
            , bind(&::nng_listener_getopt_int, lid, _1, _2)
            , bind(::nng_listener_getopt_size, lid, _1, _2)
            , bind(::nng_listener_getopt_ms, lid, _1, _2)
        );

        _options.set_setters(
            bind(&::nng_listener_setopt, lid, _1, _2, _3)
            , bind(&::nng_listener_setopt_int, lid, _1, _2)
            , bind(&::nng_listener_setopt_size, lid, _1, _2)
            , bind(&::nng_listener_setopt_ms, lid, _1, _2)
        );
    }

    options_reader_writer* const listener::options() {
        return &_options;
    }
}
