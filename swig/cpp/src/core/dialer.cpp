#include "dialer.h"
#include "socket.h"
#include "nng_exception.h"

namespace nng {

    // TODO: TBD: ditto "listener" ...
    dialer::dialer()
        : endpoint()
            , options() {
    }

    dialer::dialer(const socket& s, const std::string& addr)
        : endpoint()
        , options() {

        auto ec = ::nng_dialer_create(&did, s.sid, addr.c_str());
    }

    dialer::~dialer() {
        close();
    }

    void dialer::start(int flags) {
        auto ec = ::nng_dialer_start(did, flags);
    }

    void dialer::close() {
        if (did) {
            auto ec = ::nng_dialer_close(did);
            did = 0;
        }
    }

    void dialer::set_option(int opt, const void* v, option_size_type sz) {
        auto ec = ::nng_dialer_setopt(did, opt, v, sz);
    }

    void dialer::set_option_int(int opt, int val) {
        auto ec = ::nng_dialer_setopt_int(did, opt, val);
    }

    void dialer::set_option_size(int opt, option_size_type val) {
        auto ec = ::nng_dialer_setopt_size(did, opt, val);
    }

    void dialer::set_option_usec(int opt, option_ulonglong_type val) {
        auto ec = ::nng_dialer_setopt_usec(did, opt, val);
    }

    void dialer::get_option(int opt, void* val, option_size_type* szp) {
        auto ec = ::nng_dialer_getopt(did, opt, val, szp);
    }

    void dialer::get_option_int(int opt, int* valp) {
        auto ec = ::nng_dialer_getopt_int(did, opt, valp);
    }

    void dialer::get_option_size(int opt, option_size_type* valp) {
        auto ec = ::nng_dialer_getopt_size(did, opt, valp);
    }

    void dialer::get_option_usec(int opt, option_ulonglong_type* valp) {
        auto ec = ::nng_dialer_getopt_usec(did, opt, valp);
    }
}
