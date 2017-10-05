#include "socket.h"
#include "listener.h"
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


    void listener::set_option_int(const std::string& name, int val) {
        auto ec = ::nng_listener_setopt_int(lid, name.c_str(), val);
    }

    void listener::set_option_size(const std::string& name, option_size_type val) {
        auto ec = ::nng_listener_setopt_size(lid, name.c_str(), val);
    }

    void listener::set_option_usec(const std::string& name, option_ulonglong_type val) {
        auto ec = ::nng_listener_setopt_usec(lid, name.c_str(), val);
    }

    void listener::get_option_int(const std::string& name, int* valp) {
        auto ec = ::nng_listener_getopt_int(lid, name.c_str(), valp);
    }

    void listener::get_option_size(const std::string& name, option_size_type* valp) {
        auto ec = ::nng_listener_getopt_size(lid, name.c_str(), valp);
    }

    void listener::get_option_usec(const std::string& name, option_ulonglong_type* valp) {
        auto ec = ::nng_listener_getopt_usec(lid, name.c_str(), valp);
    }
}
