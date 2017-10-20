#include "listener.h"
#include "socket.h"
#include "exceptions.hpp"

namespace nng {

    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;
    using std::placeholders::_4;

    // TODO: TBD: is "listener" its own thing? or simply another kind of "socket"? i.e. perhaps a receive-only socket, as the name would suggest
    listener::listener()
        : lid(0)
        , endpoint()
        , options_reader()
        , options_writer() {
    }

    listener::listener(const socket* const sp, const std::string& addr)
        : lid(0)
        , endpoint()
        , options_reader()
        , options_writer() {

        const auto op = std::bind(&::nng_listener_create, _1, _2, _3);
        const auto errnum = op(&lid, sp->sid, addr.c_str());
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    listener::~listener() {
        close();
    }

    void listener::start(flag_type flags) {
        const auto op = std::bind(&::nng_listener_start, _1, _2);
        const auto errnum = op(lid, static_cast<int>(flags));
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void listener::close() {
        if (lid) {
            const auto op = std::bind(&::nng_listener_close, _1);
            const auto errnum = op(lid);
            THROW_NNG_EXCEPTION_EC(errnum);
            lid = 0;
        }
    }

    bool listener::has_one() const {
        return lid != 0;
    }

    void listener::get_option(const std::string& name, std::string& val, size_type& sz) {
        const auto op = std::bind(&::nng_listener_getopt, _1, _2, _3, _4);
        const auto errnum = op(lid, name.c_str(), (void*)&val[0], &sz);
        THROW_NNG_EXCEPTION_EC(errnum);
        // Do this because the C++ buffer does not appear to be honored quite right when working with the C buffers.
        val.resize(sz - 1);
    }

    void listener::get_option(const std::string& name, std::string& val) {
        auto sz = val.size();
        const auto op = std::bind(&::nng_listener_getopt, _1, _2, _3, _4);
        const auto errnum = op(lid, name.c_str(), (void*)&val[0], &sz);
        THROW_NNG_EXCEPTION_EC(errnum);
        // Do this because the C++ buffer does not appear to be honored quite right when working with the C buffers.
        val.resize(sz - 1);
    }

    void listener::get_option(const std::string& name, void* val, size_type* szp) {
        const auto op = std::bind(&::nng_listener_getopt, _1, _2, _3, _4);
        const auto errnum = op(lid, name.c_str(), val, szp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void listener::get_option_int(const std::string& name, int* valp) {
        const auto op = std::bind(&::nng_listener_getopt_int, _1, _2, _3);
        const auto errnum = op(lid, name.c_str(), valp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void listener::get_option_size(const std::string& name, size_type* valp) {
        const auto op = std::bind(&::nng_listener_getopt_size, _1, _2, _3);
        const auto errnum = op(lid, name.c_str(), valp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void listener::get_option(const std::string& name, duration_type* valp) {
        duration_type::rep val;
        get_option_ms(name, &val);
        *valp = duration_type(val);
    }

    void listener::get_option_ms(const std::string& name, duration_rep_type* valp) {
        const auto op = std::bind(&::nng_listener_getopt_ms, _1, _2, _3);
        const auto errnum = op(lid, name.c_str(), valp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void listener::set_option(const std::string& name, const std::string& val, size_type sz) {
        const auto op = std::bind(&::nng_listener_setopt, _1, _2, _3, _4);
        const auto errnum = op(lid, name.c_str(), val.c_str(), sz);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void listener::set_option(const std::string& name, const std::string& val) {
        const auto op = std::bind(&::nng_listener_setopt, _1, _2, _3, _4);
        const auto errnum = op(lid, name.c_str(), val.c_str(), val.length());
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void listener::set_option(const std::string& name, const void* bp, size_type sz) {
        const auto op = std::bind(&::nng_listener_setopt, _1, _2, _3, _4);
        const auto errnum = op(lid, name.c_str(), bp, sz);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void listener::set_option_int(const std::string& name, int val) {
        const auto op = std::bind(&::nng_listener_setopt_int, _1, _2, _3);
        const auto errnum = op(lid, name.c_str(), val);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void listener::set_option_size(const std::string& name, size_type val) {
        const auto op = std::bind(&::nng_listener_setopt_size, _1, _2, _3);
        const auto errnum = op(lid, name.c_str(), val);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void listener::set_option(const std::string& name, duration_type val) {
        set_option_ms(name, val.count());
    }

    void listener::set_option_ms(const std::string& name, duration_rep_type val) {
        const auto op = std::bind(&::nng_listener_setopt_ms, _1, _2, _3);
        const auto errnum = op(lid, name.c_str(), val);
        THROW_NNG_EXCEPTION_EC(errnum);
    }
}
