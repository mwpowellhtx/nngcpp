#include "socket.h"
#include "dialer.h"
#include "nng_exception.h"

namespace nng {

    // TODO: TBD: ditto "listener" ...
    dialer::dialer()
        : did(0)
        , endpoint()
        , options() {
    }

    dialer::dialer(const socket& s, const std::string& addr)
        : did(0)
        , endpoint()
        , options() {

        const auto errnum = ::nng_dialer_create(&did, s.sid, addr.c_str());
        //if (errnum) {
        //    std::string buffer;
        //    buffer.resize(128);
        //    _itoa(errnum, &buffer[0], 10);
        //    throw std::exception(("errnum: " + buffer).c_str());
        //}
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    dialer::~dialer() {
        close();
    }

    void dialer::start(int flags) {
        const auto errnum = ::nng_dialer_start(did, flags);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void dialer::close() {
        if (did) {
            //std::string buffer;
            //buffer.resize(128);
            //_itoa(did, &buffer[0], 10);
            const auto errnum = ::nng_dialer_close(did);
            THROW_NNG_EXCEPTION_EC(errnum);
            did = 0;
        }
    }

    void dialer::set_option(const std::string& name, const std::string& val, option_size_type sz) {
        const auto errnum = ::nng_dialer_setopt(did, name.c_str(), val.c_str(), sz);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void dialer::set_option(const std::string& name, const std::string& val) {
        const auto errnum = ::nng_dialer_setopt(did, name.c_str(), val.c_str(), val.length());
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void dialer::get_option(const std::string& name, std::string& val, option_size_type& sz) {
        const auto errnum = ::nng_dialer_getopt(did, name.c_str(), (void*)&val[0], &sz);
        THROW_NNG_EXCEPTION_EC(errnum);
        // Do this because the C++ buffer does not appear to be honored quite right when working with the C buffers.
        val.resize(sz - 1);
    }

    void dialer::get_option(const std::string& name, std::string& val) {
        option_size_type sz = val.size();
        const auto errnum = ::nng_dialer_getopt(did, name.c_str(), (void*)&val[0], &sz);
        THROW_NNG_EXCEPTION_EC(errnum);
            // Do this because the C++ buffer does not appear to be honored quite right when working with the C buffers.
        val.resize(sz - 1);
    }

    void dialer::set_option(const std::string& name, const void* bp, option_size_type sz) {
        const auto errnum = ::nng_dialer_setopt(did, name.c_str(), bp, sz);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void dialer::get_option(const std::string& name, void* val, option_size_type* szp) {
        const auto errnum = ::nng_dialer_getopt(did, name.c_str(), val, szp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void dialer::set_option_int(const std::string& name, int val) {
        const auto errnum = ::nng_dialer_setopt_int(did, name.c_str(), val);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void dialer::set_option_size(const std::string& name, option_size_type val) {
        const auto errnum = ::nng_dialer_setopt_size(did, name.c_str(), val);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void dialer::set_option_usec(const std::string& name, option_ulonglong_type val) {
        const auto errnum = ::nng_dialer_setopt_usec(did, name.c_str(), val);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void dialer::get_option_int(const std::string& name, int* valp) {
        const auto errnum = ::nng_dialer_getopt_int(did, name.c_str(), valp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void dialer::get_option_size(const std::string& name, option_size_type* valp) {
        const auto errnum = ::nng_dialer_getopt_size(did, name.c_str(), valp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void dialer::get_option_usec(const std::string& name, option_ulonglong_type* valp) {
        const auto errnum = ::nng_dialer_getopt_usec(did, name.c_str(), valp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }
}
