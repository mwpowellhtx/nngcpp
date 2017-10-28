#ifndef NNGCPP_OPTIONS_NAMES_H
#define NNGCPP_OPTIONS_NAMES_H

#include <string>

namespace nng {

    struct _OptionNames {
    private:

        _OptionNames();

    public:

        // TODO: TBD: provide time-units to make the times more obvious: i.e. Seconds? Milliseconds? Microseconds? Nanoseconds?
        static const std::string socket_name;
        static const std::string compat_domain;
        static const std::string raw;
        static const std::string linger_duration;
        static const std::string recv_buf;
        static const std::string send_buf;
        static const std::string recv_fd;
        static const std::string send_fd;
        static const std::string recv_timeout_duration;
        static const std::string send_timeout_duration;
        static const std::string local_addr;
        static const std::string remote_addr;
        static const std::string url;
        static const std::string max_ttl;
        static const std::string protocol;
        static const std::string transport;
        static const std::string max_recv_sz;
        static const std::string min_reconnect_duration;
        static const std::string max_reconnect_duration;
        static const std::string pair1_polyamorous;
        static const std::string sub_subscribe;
        static const std::string sub_unsubscribe;
        static const std::string req_resend_duration;
        static const std::string surveyor_survey_duration;
    };

    typedef _OptionNames option_names;
}

#endif // NNGCPP_OPTIONS_NAMES_H
