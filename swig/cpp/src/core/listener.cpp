#include "listener.h"
#include "socket.h"
#include "nng_exception.h"

namespace nng {

    // TODO: TBD: is "listener" its own thing? or simply another kind of "socket"? i.e. perhaps a receive-only socket, as the name would suggest
    listener::listener() {
    }

    listener::listener(const socket& s, const std::string& addr)
        : endpoint()
            , options() {

        auto ec = ::nng_listener_create(&lid, s.sid, addr.c_str());
    }

    listener::~listener() {
        close();
    }

    void listener::start(int flags) {
        auto ec = ::nng_listener_start(lid, flags);
    }

    void listener::close() {
        if (lid) {
            auto ec = ::nng_listener_close(lid);
            lid = 0;
        }
    }

    void listener::set_option(int opt, const void* v, option_size_type sz) {
        auto ec = ::nng_listener_setopt(lid, opt, v, sz);
    }

    void listener::set_option_int(int opt, int val) {
        auto ec = ::nng_listener_setopt_int(lid, opt, val);
    }

    void listener::set_option_size(int opt, option_size_type val) {
        auto ec = ::nng_listener_setopt_size(lid, opt, val);
    }

    void listener::set_option_usec(int opt, option_ulonglong_type val) {
        auto ec = ::nng_listener_setopt_usec(lid, opt, val);
    }

    void listener::get_option(int opt, void* val, option_size_type* szp) {
        auto ec = ::nng_listener_getopt(lid, opt, val, szp);
    }

    void listener::get_option_int(int opt, int* valp) {
        auto ec = ::nng_listener_getopt_int(lid, opt, valp);
    }

    void listener::get_option_size(int opt, option_size_type* valp) {
        auto ec = ::nng_listener_getopt_size(lid, opt, valp);
    }

    void listener::get_option_usec(int opt, option_ulonglong_type* valp) {
        auto ec = ::nng_listener_getopt_usec(lid, opt, valp);
    }
}
