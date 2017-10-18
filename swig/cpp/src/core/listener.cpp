#include "listener.h"
#include "socket.h"
#include "exceptions.hpp"

namespace nng {

    // TODO: TBD: is "listener" its own thing? or simply another kind of "socket"? i.e. perhaps a receive-only socket, as the name would suggest
    listener::listener()
        : lid(0)
        , endpoint()
        , options() {
    }

    listener::listener(const socket* const sp, const std::string& addr)
        : lid(0)
        , endpoint()
        , options() {

        const auto errnum = ::nng_listener_create(&lid, sp->sid, addr.c_str());
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    listener::~listener() {
        close();
    }

    void listener::start(flag_type flags) {
        const auto errnum = ::nng_listener_start(lid, static_cast<int>(flags));
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void listener::close() {
        if (lid) {
            const auto errnum = ::nng_listener_close(lid);
            THROW_NNG_EXCEPTION_EC(errnum);
            lid = 0;
        }
    }

    bool listener::has_one() const {
        return lid != 0;
    }

    void listener::set_option(const std::string& name, const std::string& val, size_type sz) {
        const auto errnum = ::nng_listener_setopt(lid, name.c_str(), val.c_str(), sz);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void listener::set_option(const std::string& name, const std::string& val) {
        const auto errnum = ::nng_listener_setopt(lid, name.c_str(), val.c_str(), val.length());
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void listener::get_option(const std::string& name, std::string& val, size_type& sz) {
        const auto errnum = ::nng_listener_getopt(lid, name.c_str(), (void*)&val[0], &sz);
        THROW_NNG_EXCEPTION_EC(errnum);
        // Do this because the C++ buffer does not appear to be honored quite right when working with the C buffers.
        val.resize(sz - 1);
    }

    void listener::get_option(const std::string& name, std::string& val) {
        size_type sz = val.size();
        const auto errnum = ::nng_listener_getopt(lid, name.c_str(), (void*)&val[0], &sz);
        THROW_NNG_EXCEPTION_EC(errnum);
        // Do this because the C++ buffer does not appear to be honored quite right when working with the C buffers.
        val.resize(sz - 1);
    }

    void listener::set_option(const std::string& name, const void* bp, size_type sz) {
        const auto errnum = ::nng_listener_setopt(lid, name.c_str(), bp, sz);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void listener::get_option(const std::string& name, void* val, size_type* szp) {
        const auto errnum = ::nng_listener_getopt(lid, name.c_str(), val, szp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void listener::set_option_int(const std::string& name, int val) {
        const auto errnum = ::nng_listener_setopt_int(lid, name.c_str(), val);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void listener::set_option_size(const std::string& name, size_type val) {
        const auto errnum = ::nng_listener_setopt_size(lid, name.c_str(), val);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void listener::set_option_usec(const std::string& name, option_ulonglong_type val) {
        const auto errnum = ::nng_listener_setopt_usec(lid, name.c_str(), val);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void listener::get_option_int(const std::string& name, int* valp) {
        const auto errnum = ::nng_listener_getopt_int(lid, name.c_str(), valp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void listener::get_option_size(const std::string& name, size_type* valp) {
        const auto errnum = ::nng_listener_getopt_size(lid, name.c_str(), valp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void listener::get_option_usec(const std::string& name, option_ulonglong_type* valp) {
        const auto errnum = ::nng_listener_getopt_usec(lid, name.c_str(), valp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }
}
