#include "listener.h"
#include "socket.h"
#include "exceptions.hpp"

namespace nng {

    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;

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

    void listener::get_option(const std::string& name, void* valp, size_type& sz) {
        const auto& op = std::bind(&::nng_listener_getopt, lid, _1, _2, _3);
        options_reader::get_option(op, name, valp, sz);
    }

    void listener::get_option(const std::string& name, std::string& val, size_type& sz) {
        val.resize(sz);
        get_option(name, val);
    }

    void listener::get_option(const std::string& name, std::string& val) {
        const auto& op = std::bind(::nng_listener_getopt, lid, _1, _2, _3);
        options_reader::get_option(op, name, val);
    }

    void listener::get_option_int(const std::string& name, int& val) {
        const auto op = std::bind(&::nng_listener_getopt_int, lid, _1, _2);
        options_reader::get_option_int(op, name, val);
    }

    void listener::get_option_sz(const std::string& name, size_type& val) {
        const auto& op = std::bind(::nng_listener_getopt_size, lid, _1, _2);
        options_reader::get_option_sz(op, name, val);
    }

    void listener::get_option(const std::string& name, duration_type& val) {
        duration_type::rep x;
        get_option_ms(name, x);
        val = duration_type(x);
    }

    void listener::get_option_ms(const std::string& name, duration_rep_type& val) {
        const auto& op = std::bind(::nng_listener_getopt_ms, lid, _1, _2);
        options_reader::get_option_ms(op, name.c_str(), val);
    }

    void listener::set_option(const std::string& name, const void* valp, size_type sz) {
        const auto op = std::bind(&::nng_listener_setopt, lid, _1, _2, _3);
        options_writer::set_option(op, name, valp, sz);
    }

    void listener::set_option(const std::string& name, const std::string& val) {
        const auto op = std::bind(&::nng_listener_setopt, lid, _1, _2, _3);
        options_writer::set_option(op, name, val);
    }

    void listener::set_option_int(const std::string& name, int val) {
        const auto& op = std::bind(&::nng_listener_setopt_int, lid, _1, _2);
        options_writer::set_option_int(op, name, val);
    }

    void listener::set_option_sz(const std::string& name, size_type val) {
        const auto& op = std::bind(&::nng_listener_setopt_size, lid, _1, _2);
        options_writer::set_option_sz(op, name, val);
    }

    void listener::set_option(const std::string& name, const duration_type& val) {
        set_option_ms(name, val.count());
    }

    void listener::set_option_ms(const std::string& name, duration_rep_type val) {
        const auto op = std::bind(&::nng_listener_setopt_ms, lid, _1, _2);
        options_writer::set_option_ms(op, name, val);
    }
}
