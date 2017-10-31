#include "dialer.h"
#include "socket.h"
#include "invocation.hpp"

namespace nng {

    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;
    using std::bind;

    // TODO: TBD: ditto "listener" ...
    _Dialer::_Dialer() : _EndPoint(), did(0) {
        configure_options(did);
    }

    _Dialer::_Dialer(const socket& s, const std::string& addr) : _EndPoint(), did(0) {

        const auto& op = bind(&::nng_dialer_create, &did, s.sid, _1);
        invocation::with_default_error_handling(op, addr.c_str());
        configure_options(did);
    }

    _Dialer::~_Dialer() {
        Close();
    }

    void _Dialer::on_dialed() {
        configure_options(did);
    }

    void _Dialer::configure_options(nng_type did) {

        // Configure the EP related bindings.
        configure_endpoint(
            bind(&::nng_dialer_start, did, _1)
            , bind(&::nng_dialer_close, did)
        );

        // Also pick up the Options bindings.
        auto op = GetOptions();

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

    bool _Dialer::HasOne() const {
        return did != 0;
    }

    void _Dialer::Start() {
        Start(flag_none);
    }

    void _Dialer::Start(SocketFlag flags) {
        invocation::with_default_error_handling(__start, flags);
    }

    void _Dialer::Close() {
        if (!HasOne()) { return; }
        invocation::with_default_error_handling(__close);
        configure_options(did = 0);
    }

    _Dialer::options_type* const _Dialer::GetOptions() {
        return ep_type::GetOptions();
    }
}
