#include "socket.h"
#include "dialer.h"
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

    void dialer::set_option_int(const std::string& name, int val) {
        auto ec = ::nng_dialer_setopt_int(did, name.c_str(), val);
    }

    void dialer::set_option_size(const std::string& name, option_size_type val) {
        auto ec = ::nng_dialer_setopt_size(did, name.c_str(), val);
    }

    void dialer::set_option_usec(const std::string& name, option_ulonglong_type val) {
        auto ec = ::nng_dialer_setopt_usec(did, name.c_str(), val);
    }

    void dialer::get_option_int(const std::string& name, int* valp) {
        auto ec = ::nng_dialer_getopt_int(did, name.c_str(), valp);
    }

    void dialer::get_option_size(const std::string& name, option_size_type* valp) {
        auto ec = ::nng_dialer_getopt_size(did, name.c_str(), valp);
    }

    void dialer::get_option_usec(const std::string& name, option_ulonglong_type* valp) {
        auto ec = ::nng_dialer_getopt_usec(did, name.c_str(), valp);
    }
}
