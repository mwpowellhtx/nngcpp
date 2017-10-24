#include "options.h"
#include "address.h"
#include "invocation.hpp"
#include "algorithms/string_algo.hpp"

namespace nng {

    options_writer::options_writer()
        : _setopt()
        , _setopt_int()
        , _setopt_sz()
        , _setopt_duration() {
    }

    options_writer::~options_writer() {
    }

    void options_writer::set_setters(const setopt_func& setopt
        , const setopt_int_func& setopt_int
        , const setopt_sz_func& setopt_sz
        , const setopt_duration_func& setopt_duration) {

        // Cast the constness away from the setters long enough to install the new ones.
        const_cast<setopt_func&>(_setopt) = setopt;
        const_cast<setopt_int_func&>(_setopt_int) = setopt_int;
        const_cast<setopt_sz_func&>(_setopt_sz) = setopt_sz;
        const_cast<setopt_duration_func&>(_setopt_duration) = setopt_duration;
    }

    void options_writer::set(const std::string& name, const void* valp, size_type sz) {
        invocation::with_default_error_handling(_setopt, name.c_str(), valp, sz);
    }

    void options_writer::set(const std::string& name, const std::string& val) {
        invocation::with_default_error_handling(_setopt, name.c_str(), val.c_str(), val.length());
    }

    void options_writer::set_int(const std::string& name, int val) {
        invocation::with_default_error_handling(_setopt_int, name.c_str(), val);
    }

    void options_writer::set_sz(const std::string& name, size_type val) {
        invocation::with_default_error_handling(_setopt_sz, name.c_str(), val);
    }

    void options_writer::set(const std::string& name, const duration_type& val) {
        set_milliseconds(name, val.count());
    }

    void options_writer::set_milliseconds(const std::string& name, duration_rep_type val) {
        invocation::with_default_error_handling(_setopt_duration, name.c_str(), val);
    }

    options_reader::options_reader()
        : _getopt()
        , _getopt_int()
        , _getopt_sz()
        , _getopt_duration() {
    }

    void options_reader::set_getters(const getopt_func& getopt
        , const getopt_int_func& getopt_int
        , const getopt_sz_func& getopt_sz
        , const getopt_duration_func& getopt_duration) {

        // Ditto casting constness away...
        const_cast<getopt_func&>(_getopt) = getopt;
        const_cast<getopt_int_func&>(_getopt_int) = getopt_int;
        const_cast<getopt_sz_func&>(_getopt_sz) = getopt_sz;
        const_cast<getopt_duration_func&>(_getopt_duration) = getopt_duration;
    }

    options_reader::~options_reader() {
    }

    void options_reader::get(const std::string& name, void* valp, size_type& sz) {
        invocation::with_default_error_handling(_getopt, name.c_str(), valp, &sz);
    }

    void options_reader::get(const std::string& name, std::string& val, size_type& sz) {
        val.resize(sz);
        get(name, val);
    }

    void options_reader::get(const std::string& name, std::string& val) {
        auto sz = val.size();
        invocation::with_default_error_handling(_getopt, name.c_str(), &val[0], &sz);
        /* So we do use the string trimming algorithms after all...
        Which the in-place is sufficient, no need to use the copying version. */
        trx::trim(val);
    }

    void options_reader::get(const std::string& name, address& val) {
        auto sz = val.get_size();
        invocation::with_default_error_handling(_getopt, name.c_str(), val.get(), &sz);
    }

    void options_reader::get_int(const std::string& name, int& val) {
        invocation::with_default_error_handling(_getopt_int, name.c_str(), &val);
    }

    void options_reader::get_sz(const std::string& name, size_type& val) {
        invocation::with_default_error_handling(_getopt_sz, name.c_str(), &val);
    }

    void options_reader::get(const std::string& name, duration_type& val) {
        duration_rep_type x;
        get_milliseconds(name, x);
        val = duration_type(x);
    }

    void options_reader::get_milliseconds(const std::string& name, duration_rep_type& val) {
        invocation::with_default_error_handling(_getopt_duration, name.c_str(), &val);
    }

    options_reader_writer::options_reader_writer() : options_reader(), options_writer() {
    }

    options_reader_writer::~options_reader_writer() {
    }

    option_names::option_names() {
    }

    const std::string option_names::socket_name = NNG_OPT_SOCKNAME;
    const std::string option_names::compat_domain = NNG_OPT_DOMAIN;
    const std::string option_names::raw = NNG_OPT_RAW;
    const std::string option_names::linger_duration = NNG_OPT_LINGER;
    const std::string option_names::recv_buf = NNG_OPT_RECVBUF;
    const std::string option_names::send_buf = NNG_OPT_SENDBUF;
    const std::string option_names::recv_fd = NNG_OPT_RECVFD;
    const std::string option_names::send_fd = NNG_OPT_SENDFD;
    const std::string option_names::recv_timeout_duration = NNG_OPT_RECVTIMEO;
    const std::string option_names::send_timeout_duration = NNG_OPT_SENDTIMEO;
    const std::string option_names::local_addr = NNG_OPT_LOCADDR;
    const std::string option_names::remote_addr = NNG_OPT_REMADDR;
    const std::string option_names::url = NNG_OPT_URL;
    const std::string option_names::max_ttl = NNG_OPT_MAXTTL;
    const std::string option_names::protocol = NNG_OPT_PROTOCOL;
    const std::string option_names::transport = NNG_OPT_TRANSPORT;
    const std::string option_names::max_recv_sz = NNG_OPT_RECVMAXSZ;
    const std::string option_names::min_reconnect_time_duration = NNG_OPT_RECONNMINT;
    const std::string option_names::max_reconnect_time_duration = NNG_OPT_RECONNMAXT;
    const std::string option_names::pair1_polyamorous = NNG_OPT_PAIR1_POLY;
    const std::string option_names::req_resend_time_duration = NNG_OPT_REQ_RESENDTIME;
    const std::string option_names::sub_subscribe = NNG_OPT_SUB_SUBSCRIBE;
    const std::string option_names::sub_unsubscribe = NNG_OPT_SUB_UNSUBSCRIBE;
    const std::string option_names::surveyor_survey_time_duration = NNG_OPT_SURVEYOR_SURVEYTIME;
}
