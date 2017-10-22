#include "options.h"
#include "exceptions.hpp"

#include "algorithms/string_algo.hpp"

namespace nng {

    template<typename Op_, typename... Args_>
    void invoke_op(const Op_& op, Args_... args) {
        const auto errnum = op(args...);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    options_writer::options_writer() {
    }

    options_writer::~options_writer() {
    }

    void options_writer::set_option(const set_option_func& op, const std::string& name, const void* valp, size_type sz) {
        invoke_op(op, name.c_str(), valp, sz);
    }

    void options_writer::set_option(const set_option_func& op, const std::string& name, const std::string& val) {
        invoke_op(op, name.c_str(), val.c_str(), val.length());
    }

    void options_writer::set_option_int(const set_option_int_func& op, const std::string& name, int val) {
        invoke_op(op, name.c_str(), val);
    }

    void options_writer::set_option_sz(const set_option_sz_func& op, const std::string& name, size_type val) {
        invoke_op(op, name.c_str(), val);
    }

    void options_writer::set_option_ms(const set_option_duration_func& op, const std::string& name, duration_rep_type val) {
        invoke_op(op, name.c_str(), val);
    }

    options_reader::options_reader() {
    }

    options_reader::~options_reader() {
    }

    void options_reader::get_option(const get_option_func& op, const std::string& name, void* valp, size_type& sz) {
        invoke_op(op, name.c_str(), valp, &sz);
    }

    void options_reader::get_option(const get_option_func& op, const std::string& name, std::string& val) {
        auto sz = val.size();
        invoke_op(op, name.c_str(), &val[0], &sz);
        /* So we do use the string trimming algorithms after all...
        Which the in-place is sufficient, no need to use the copying version. */
        trx::trim(val);
    }

    void options_reader::get_option_int(const get_option_int_func& op, const std::string& name, int& val) {
        invoke_op(op, name.c_str(), &val);
    }

    void options_reader::get_option_sz(const get_option_sz_func& op, const std::string& name, size_type& val) {
        invoke_op(op, name.c_str(), &val);
    }

    void options_reader::get_option_ms(const get_option_ms_func& op, const std::string& name, duration_rep_type& val) {
        invoke_op(op, name.c_str(), &val);
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
