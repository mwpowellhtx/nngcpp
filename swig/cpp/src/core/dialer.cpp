#include "dialer.h"
#include "socket.h"
#include "exceptions.hpp"

namespace nng {

    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;
    using std::placeholders::_4;

    // TODO: TBD: ditto "listener" ...
    dialer::dialer()
        : did(0)
        , endpoint()
        , options_reader()
        , options_writer() {
    }

    dialer::dialer(const socket* const sp, const std::string& addr)
        : did(0)
        , endpoint()
        , options_reader()
        , options_writer() {

        const auto errnum = ::nng_dialer_create(&did, sp->sid, addr.c_str());
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    dialer::~dialer() {
        close();
    }

    void dialer::start(flag_type flags) {
        const auto errnum = ::nng_dialer_start(did, static_cast<int>(flags));
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void dialer::close() {
        if (did) {
            const auto op = std::bind(&::nng_dialer_close, _1);
            const auto errnum = op(did);
            THROW_NNG_EXCEPTION_EC(errnum);
            did = 0;
        }
    }

    bool dialer::has_one() const {
        return did != 0;
    }

    void dialer::get_option(const std::string& name, std::string& val) {
        auto sz = val.size();
        const auto& op = std::bind(&::nng_dialer_getopt, _1, _2, _3, _4);
        const auto errnum = op(did, name.c_str(), (void*)&val[0], &sz);
        THROW_NNG_EXCEPTION_EC(errnum);
        // Do this because the C++ buffer does not appear to be honored quite right when working with the C buffers.
        val.resize(sz - 1);
    }

    void dialer::get_option(const std::string& name, std::string& val, size_type& sz) {
        const auto& op = std::bind(&::nng_dialer_getopt, _1, _2, _3, _4);
        const auto errnum = op(did, name.c_str(), (void*)&val[0], &sz);
        THROW_NNG_EXCEPTION_EC(errnum);
        // Do this because the C++ buffer does not appear to be honored quite right when working with the C buffers.
        val.resize(sz - 1);
    }

    void dialer::get_option(const std::string& name, void* val, size_type* szp) {
        const auto& op = std::bind(&::nng_dialer_getopt, _1, _2, _3, _4);
        const auto errnum = op(did, name.c_str(), val, szp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void dialer::get_option_int(const std::string& name, int* valp) {
        const auto& op = std::bind(&::nng_dialer_getopt_int, _1, _2, _3);
        const auto errnum = op(did, name.c_str(), valp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void dialer::get_option_size(const std::string& name, size_type* valp) {
        const auto& op = std::bind(&::nng_dialer_getopt_size, _1, _2, _3);
        const auto errnum = op(did, name.c_str(), valp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void dialer::get_option(const std::string& name, duration_type* valp) {
        duration_rep_type val;
        get_option_ms(name, &val);
        *valp = duration_type(val);
    }

    void dialer::get_option_ms(const std::string& name, duration_rep_type* valp) {
        const auto& op = std::bind(&::nng_dialer_getopt_ms, _1, _2, _3);
        const auto errnum = op(did, name.c_str(), valp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void dialer::set_option(const std::string& name, const std::string& val, size_type sz) {
        const auto& op = std::bind(&::nng_dialer_setopt, _1, _2, _3, _4);
        const auto errnum = op(did, name.c_str(), val.c_str(), sz);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void dialer::set_option(const std::string& name, const std::string& val) {
        const auto& op = std::bind(&::nng_dialer_setopt, _1, _2, _3, _4);
        const auto errnum = op(did, name.c_str(), val.c_str(), val.length());
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void dialer::set_option(const std::string& name, const void* bp, size_type sz) {
        const auto& op = std::bind(&::nng_dialer_setopt, _1, _2, _3, _4);
        const auto errnum = op(did, name.c_str(), bp, sz);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void dialer::set_option_int(const std::string& name, int val) {
        const auto& op = std::bind(&::nng_dialer_setopt_int, _1, _2, _3);
        const auto errnum = op(did, name.c_str(), val);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void dialer::set_option_size(const std::string& name, size_type val) {
        const auto& op = std::bind(&::nng_dialer_setopt_size, _1, _2, _3);
        const auto errnum = op(did, name.c_str(), val);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void dialer::set_option(const std::string& name, duration_type val) {
        set_option_ms(name, val.count());
    }

    void dialer::set_option_ms(const std::string& name, duration_rep_type val) {
        const auto& op = std::bind(&::nng_dialer_setopt_ms, _1, _2, _3);
        const auto errnum = op(did, name.c_str(), val);
        THROW_NNG_EXCEPTION_EC(errnum);
    }
}
