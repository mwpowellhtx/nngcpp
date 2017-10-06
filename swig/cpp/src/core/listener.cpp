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

        const auto errnum = ::nng_listener_create(&lid, s.sid, addr.c_str());
    }

    listener::~listener() {
        close();
    }

    void listener::start(int flags) {
        const auto errnum = ::nng_listener_start(lid, flags);
    }

    void listener::close() {
        if (lid) {
            const auto errnum = ::nng_listener_close(lid);
            lid = 0;
        }
    }

    void listener::set_option(const std::string& name, const std::string& val, option_size_type sz) {
        const auto errnum = ::nng_listener_setopt(lid, name.c_str(), val.c_str(), sz);
    }

    void listener::set_option(const std::string& name, const std::string& val) {
        const auto errnum = ::nng_listener_setopt(lid, name.c_str(), val.c_str(), val.length());
    }

    void listener::get_option(const std::string& name, std::string& val, option_size_type& sz) {
        const auto errnum = ::nng_listener_getopt(lid, name.c_str(), (void*)&val[0], &sz);
        // Do this because the C++ buffer does not appear to be honored quite right when working with the C buffers.
        val.resize(sz - 1);
    }

    void listener::get_option(const std::string& name, std::string& val) {
        option_size_type sz = val.size();
        const auto errnum = ::nng_listener_getopt(lid, name.c_str(), (void*)&val[0], &sz);
        // Do this because the C++ buffer does not appear to be honored quite right when working with the C buffers.
        val.resize(sz - 1);
    }

    void listener::set_option(const std::string& name, const void* bp, option_size_type sz) {
        const auto errnum = ::nng_listener_setopt(lid, name.c_str(), bp, sz);
    }

    void listener::get_option(const std::string& name, void* val, option_size_type* szp) {
        const auto errnum = ::nng_listener_getopt(lid, name.c_str(), val, szp);
    }

    void listener::set_option_int(const std::string& name, int val) {
        const auto errnum = ::nng_listener_setopt_int(lid, name.c_str(), val);
    }

    void listener::set_option_size(const std::string& name, option_size_type val) {
        const auto errnum = ::nng_listener_setopt_size(lid, name.c_str(), val);
    }

    void listener::set_option_usec(const std::string& name, option_ulonglong_type val) {
        const auto errnum = ::nng_listener_setopt_usec(lid, name.c_str(), val);
    }

    void listener::get_option_int(const std::string& name, int* valp) {
        const auto errnum = ::nng_listener_getopt_int(lid, name.c_str(), valp);
    }

    void listener::get_option_size(const std::string& name, option_size_type* valp) {
        const auto errnum = ::nng_listener_getopt_size(lid, name.c_str(), valp);
    }

    void listener::get_option_usec(const std::string& name, option_ulonglong_type* valp) {
        const auto errnum = ::nng_listener_getopt_usec(lid, name.c_str(), valp);
    }
}
