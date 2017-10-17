#ifndef NNGCPP_OPTIONS_H
#define NNGCPP_OPTIONS_H

#include "../nngcpp_integration.h"

#include <cstddef>
#include <cstdint>
#include <string>

namespace nng {

    class options {
        protected:

            typedef std::size_t size_type;
            typedef uint64_t option_ulonglong_type;

            options();

        public:

            //// TODO: TBD: I do not thinkg there is ever a good reason to expose these to the externally facing consumer... I could be wrong about that, but for now will leave them unspecified.
            virtual void set_option(const std::string& name, const std::string& val, size_type sz) = 0;
            virtual void set_option(const std::string& name, const std::string& val) = 0;

            virtual void get_option(const std::string& name, std::string& val) = 0;
            virtual void get_option(const std::string& name, std::string& val, size_type& sz) = 0;

            virtual void set_option(const std::string& name, const void* valp, size_type sz) = 0;
            virtual void get_option(const std::string& name, void* val, size_type* szp) = 0;

            virtual void set_option_int(const std::string& name, int val) = 0;
            virtual void set_option_size(const std::string& name, size_type val) = 0;
            virtual void set_option_usec(const std::string& name, option_ulonglong_type val) = 0;

            virtual void get_option_int(const std::string& name, int* valp) = 0;
            virtual void get_option_size(const std::string& name, size_type* valp) = 0;
            virtual void get_option_usec(const std::string& name, option_ulonglong_type* valp) = 0;

            // TODO: TBD: add API for get/set uint64 support
            // TODO: TBD: also consider separating get from set slices...
            // TODO: TBD: look into how possibly to "disable" functionality, perhaps via template?
    };

    struct option_names {
        private:

            option_names();

        public:

            // TODO: TBD: provide time-units to make the times more obvious: i.e. Seconds? Milliseconds? Microseconds? Nanoseconds?
            static const std::string socket_name;
            static const std::string compat_domain;
            static const std::string raw;
            static const std::string linger_usec;
            static const std::string receive_buffer;
            static const std::string send_buffer;
            static const std::string receive_file_descriptor;
            static const std::string send_file_descriptor;
            static const std::string receive_timeout_usec;
            static const std::string send_timeout_usec;
            static const std::string local_address;
            static const std::string remote_address;
            static const std::string url;
            static const std::string max_ttl;
            static const std::string protocol;
            static const std::string transport;
            static const std::string max_receive_size;
            static const std::string min_reconnect_time_usec;
            static const std::string max_reconnect_time_usec;
            static const std::string pair1_polyamorous;
            static const std::string sub_subscribe;
            static const std::string sub_unsubscribe;
            static const std::string req_resend_time_usec;
            static const std::string surveyor_survey_time_usec;
    };
}

#endif // NNGCPP_OPTIONS_H
