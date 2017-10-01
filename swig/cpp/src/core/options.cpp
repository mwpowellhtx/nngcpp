#include "options.h"

namespace nng {

    options::options() {
    }

    option_names::option_names() {
    }

    const std::string option_names::raw = NNG_OPT_RAW;
    const std::string option_names::linger = NNG_OPT_LINGER;
    const std::string option_names::receive_buffer = NNG_OPT_RECVBUF;
    const std::string option_names::send_buffer = NNG_OPT_SENDBUF;
    const std::string option_names::receive_file_descriptor = NNG_OPT_RECVFD;
    const std::string option_names::send_file_descriptor = NNG_OPT_SENDFD;
    const std::string option_names::receive_timeout = NNG_OPT_RECVTIMEO;
    const std::string option_names::send_timeout = NNG_OPT_SENDTIMEO;
    const std::string option_names::local_address = NNG_OPT_LOCADDR;
    const std::string option_names::remote_address = NNG_OPT_REMADDR;
    const std::string option_names::url = NNG_OPT_URL;
    const std::string option_names::max_ttl = NNG_OPT_MAXTTL;
    const std::string option_names::protocol = NNG_OPT_PROTOCOL;
    const std::string option_names::transport = NNG_OPT_TRANSPORT;
    const std::string option_names::receive_max_size = NNG_OPT_RECVMAXSZ;
    const std::string option_names::min_reconnect_time = NNG_OPT_RECONNMINT;
    const std::string option_names::sub_subscribe = NNG_OPT_SUB_SUBSCRIBE;
    const std::string option_names::surveyor_survey_time = NNG_OPT_SURVEYOR_SURVEYTIME;
}
