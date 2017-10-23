#include "dialer.h"
#include "socket.h"
#include "options.h"
#include "exceptions.hpp"

namespace nng {

    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;
    using std::bind;

    // TODO: TBD: ditto "listener" ...
    dialer::dialer() : endpoint(), did(0), _options() {
        configure_options();
    }

    dialer::dialer(const socket* const sp, const std::string& addr)
        : endpoint(), did(0), _options() {

        const auto& op = bind(&::nng_dialer_create, &did, sp->sid, _1);
        const auto errnum = op(addr.c_str());
        THROW_NNG_EXCEPTION_EC(errnum);

        configure_options();
    }

    dialer::~dialer() {
        close();
    }

    void dialer::start(flag_type flags) {
        const auto& op = bind(&::nng_dialer_start, did, _1);
        const auto errnum = op(static_cast<int>(flags));
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void dialer::close() {
        if (did) {
            const auto op = bind(&::nng_dialer_close, did);
            const auto errnum = op();
            THROW_NNG_EXCEPTION_EC(errnum);
            did = 0;
            // TODO: TBD: reconfigure afer close? may not be a bad idea...
        }
    }

    bool dialer::has_one() const {
        return did != 0;
    }

    void dialer::on_dialed() {
        if (did) {
            configure_options();
        }
    }

    void dialer::configure_options() {

        _options.set_getters(
            bind(&::nng_dialer_getopt, did, _1, _2, _3)
            , bind(&::nng_dialer_getopt_int, did, _1, _2)
            , bind(&::nng_dialer_getopt_size, did, _1, _2)
            , bind(&::nng_dialer_getopt_ms, did, _1, _2)
        );

        _options.set_setters(
            bind(&::nng_dialer_setopt, did, _1, _2, _3)
            , bind(&::nng_dialer_setopt_int, did, _1, _2)
            , bind(&::nng_dialer_setopt_size, did, _1, _2)
            , bind(&::nng_dialer_setopt_ms, did, _1, _2)
        );
    }

    options_reader_writer* const dialer::options() {
        return &_options;
    }
}
