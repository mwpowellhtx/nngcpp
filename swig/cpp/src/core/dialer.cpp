#include "dialer.h"
#include "socket.h"
#include "invocation.hpp"

namespace nng {

    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;
    using std::bind;

    // TODO: TBD: ditto "listener" ...
    dialer::dialer() : endpoint(), did(0) {
        configure_options(did);
    }

    dialer::dialer(const socket* const sp, const std::string& addr) : endpoint(), did(0) {

        const auto& op = bind(&::nng_dialer_create, &did, sp->sid, _1);
        invocation::with_default_error_handling(op, addr.c_str());
        configure_options(did);
    }

    dialer::~dialer() {
        close();
    }

    void dialer::start(flag_type flags) {
        const auto& op = bind(&::nng_dialer_start, did, _1);
        invocation::with_default_error_handling(op, static_cast<int>(flags));
    }

    void dialer::close() {
        if (!has_one()) { return; }
        const auto op = bind(&::nng_dialer_close, did);
        invocation::with_default_error_handling(op);
        configure_options(did = 0);
    }

    bool dialer::has_one() const {
        return did != 0;
    }

    void dialer::on_dialed() {
        configure_options(did);
    }

    void dialer::configure_options(nng_type did) {

        auto op = options();

        op->set_getters(
            bind(&::nng_dialer_getopt, did, _1, _2, _3)
            , bind(&::nng_dialer_getopt_int, did, _1, _2)
            , bind(&::nng_dialer_getopt_size, did, _1, _2)
            , bind(&::nng_dialer_getopt_ms, did, _1, _2)
        );

        op->set_setters(
            bind(&::nng_dialer_setopt, did, _1, _2, _3)
            , bind(&::nng_dialer_setopt_int, did, _1, _2)
            , bind(&::nng_dialer_setopt_size, did, _1, _2)
            , bind(&::nng_dialer_setopt_ms, did, _1, _2)
        );
    }
}
