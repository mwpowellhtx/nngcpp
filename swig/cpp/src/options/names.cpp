#define NNG_ONLY
#include<nngcpp.h>

#include "names.h"

namespace nng {

    using std::string;
    using O = OptionNames;

    const string O::socket_name = NNG_OPT_SOCKNAME;
    const string O::compat_domain = NNG_OPT_DOMAIN;
    const string O::raw = NNG_OPT_RAW;
    const string O::linger_duration = NNG_OPT_LINGER;
    const string O::recv_buf = NNG_OPT_RECVBUF;
    const string O::send_buf = NNG_OPT_SENDBUF;
    const string O::recv_fd = NNG_OPT_RECVFD;
    const string O::send_fd = NNG_OPT_SENDFD;
    const string O::recv_timeout_duration = NNG_OPT_RECVTIMEO;
    const string O::send_timeout_duration = NNG_OPT_SENDTIMEO;
    const string O::local_addr = NNG_OPT_LOCADDR;
    const string O::remote_addr = NNG_OPT_REMADDR;
    const string O::url = NNG_OPT_URL;
    const string O::max_ttl = NNG_OPT_MAXTTL;
    const string O::protocol = NNG_OPT_PROTOCOL;
    const string O::transport = NNG_OPT_TRANSPORT;
    const string O::max_recv_sz = NNG_OPT_RECVMAXSZ;
    const string O::min_reconnect_time_duration = NNG_OPT_RECONNMINT;
    const string O::max_reconnect_time_duration = NNG_OPT_RECONNMAXT;
    const string O::pair1_polyamorous = NNG_OPT_PAIR1_POLY;
    const string O::req_resend_time_duration = NNG_OPT_REQ_RESENDTIME;
    const string O::sub_subscribe = NNG_OPT_SUB_SUBSCRIBE;
    const string O::sub_unsubscribe = NNG_OPT_SUB_UNSUBSCRIBE;
    const string O::surveyor_survey_time_duration = NNG_OPT_SURVEYOR_SURVEYTIME;
}
