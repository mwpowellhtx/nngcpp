#include "options.h"

namespace nng {

    options_writer::options_writer() {
    }

    options_writer::~options_writer() {
    }

    options_reader::options_reader() {
    }

    options_reader::~options_reader() {
    }

    option_names::option_names() {
    }

    const std::string option_names::socket_name = NNG_OPT_SOCKNAME;
    const std::string option_names::compat_domain = NNG_OPT_DOMAIN;
    const std::string option_names::raw = NNG_OPT_RAW;
    const std::string option_names::linger_duration = NNG_OPT_LINGER;
    const std::string option_names::receive_buffer = NNG_OPT_RECVBUF;
    const std::string option_names::send_buffer = NNG_OPT_SENDBUF;
    const std::string option_names::receive_file_descriptor = NNG_OPT_RECVFD;
    const std::string option_names::send_file_descriptor = NNG_OPT_SENDFD;
    const std::string option_names::receive_timeout_duration = NNG_OPT_RECVTIMEO;
    const std::string option_names::send_timeout_duration = NNG_OPT_SENDTIMEO;
    const std::string option_names::local_address = NNG_OPT_LOCADDR;
    const std::string option_names::remote_address = NNG_OPT_REMADDR;
    const std::string option_names::url = NNG_OPT_URL;
    const std::string option_names::max_ttl = NNG_OPT_MAXTTL;
    const std::string option_names::protocol = NNG_OPT_PROTOCOL;
    const std::string option_names::transport = NNG_OPT_TRANSPORT;
    const std::string option_names::max_receive_size = NNG_OPT_RECVMAXSZ;
    const std::string option_names::min_reconnect_time_duration = NNG_OPT_RECONNMINT;
    const std::string option_names::max_reconnect_time_duration = NNG_OPT_RECONNMAXT;
    const std::string option_names::pair1_polyamorous = NNG_OPT_PAIR1_POLY;
    const std::string option_names::req_resend_time_duration = NNG_OPT_REQ_RESENDTIME;
    const std::string option_names::sub_subscribe = NNG_OPT_SUB_SUBSCRIBE;
    const std::string option_names::sub_unsubscribe = NNG_OPT_SUB_UNSUBSCRIBE;
    const std::string option_names::surveyor_survey_time_duration = NNG_OPT_SURVEYOR_SURVEYTIME;
}
