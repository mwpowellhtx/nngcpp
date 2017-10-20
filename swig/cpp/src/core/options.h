#ifndef NNGCPP_OPTIONS_H
#define NNGCPP_OPTIONS_H

#include "types.h"

#include <string>

namespace nng {

    class options_writer {
    protected:

        options_writer();

    public:

        ~options_writer();

        virtual void set_option(const std::string& name, const void* valp, size_type sz) = 0;

        virtual void set_option(const std::string& name, const std::string& val, size_type sz) = 0;
        virtual void set_option(const std::string& name, const std::string& val) = 0;

        virtual void set_option_int(const std::string& name, int val) = 0;
        virtual void set_option_size(const std::string& name, size_type val) = 0;
        virtual void set_option_ms(const std::string& name, duration_type val) = 0;
    };

    class options_reader {
    protected:

            options_reader();

        public:

            virtual ~options_reader();

            virtual void get_option(const std::string& name, void* val, size_type* szp) = 0;

            virtual void get_option(const std::string& name, std::string& val) = 0;
            virtual void get_option(const std::string& name, std::string& val, size_type& sz) = 0;

            virtual void get_option_int(const std::string& name, int* valp) = 0;
            virtual void get_option_size(const std::string& name, size_type* valp) = 0;
            virtual void get_option_ms(const std::string& name, duration_type* valp) = 0;
    };

    struct option_names {
        private:

            option_names();

        public:

            // TODO: TBD: provide time-units to make the times more obvious: i.e. Seconds? Milliseconds? Microseconds? Nanoseconds?
            static const std::string socket_name;
            static const std::string compat_domain;
            static const std::string raw;
            static const std::string linger_duration;
            static const std::string receive_buffer;
            static const std::string send_buffer;
            static const std::string receive_file_descriptor;
            static const std::string send_file_descriptor;
            static const std::string receive_timeout_duration;
            static const std::string send_timeout_duration;
            static const std::string local_address;
            static const std::string remote_address;
            static const std::string url;
            static const std::string max_ttl;
            static const std::string protocol;
            static const std::string transport;
            static const std::string max_receive_size;
            static const std::string min_reconnect_time_duration;
            static const std::string max_reconnect_time_duration;
            static const std::string pair1_polyamorous;
            static const std::string sub_subscribe;
            static const std::string sub_unsubscribe;
            static const std::string req_resend_time_duration;
            static const std::string surveyor_survey_time_duration;
    };
}

#endif // NNGCPP_OPTIONS_H
