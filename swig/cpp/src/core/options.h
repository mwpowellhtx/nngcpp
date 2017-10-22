#ifndef NNGCPP_OPTIONS_H
#define NNGCPP_OPTIONS_H

#include "types.h"

#include <string>
#include <functional>

namespace nng {

    class options_writer {
    protected:

        options_writer();

        typedef std::function<int(const char*, const void*, size_type)> set_option_func;

        typedef std::function<int(const char*, int val)> set_option_int_func;
        typedef std::function<int(const char*, size_type val)> set_option_sz_func;

        typedef std::function<int(const char*, duration_rep_type)> set_option_duration_func;

        void set_option(const set_option_func& op, const std::string& name, const void* valp, size_type sz);
        void set_option(const set_option_func& op, const std::string& name, const std::string& val);

        void set_option_int(const set_option_int_func& op, const std::string& name, int val);
        void set_option_sz(const set_option_sz_func& op, const std::string& name, size_type val);

        void set_option_ms(const set_option_duration_func& op, const std::string& name, duration_rep_type val);

    public:

        ~options_writer();

        virtual void set_option(const std::string& name, const void* valp, size_type sz) = 0;
        virtual void set_option(const std::string& name, const std::string& val) = 0;

        virtual void set_option_int(const std::string& name, int val) = 0;
        virtual void set_option_sz(const std::string& name, size_type val) = 0;

        virtual void set_option(const std::string& name, duration_type val) = 0;
        virtual void set_option_ms(const std::string& name, duration_rep_type val) = 0;
    };

    class options_reader {
    protected:

        options_reader();

        typedef std::function<int(const char*, void* valp, size_type* szp)> get_option_func;

        typedef std::function<int(const char*, int* valp)> get_option_int_func;
        typedef std::function<int(const char*, size_type* valp)> get_option_sz_func;

        typedef std::function<int(const char*, duration_rep_type* valp)> get_option_ms_func;

        void get_option(const get_option_func& op, const std::string& name, void* valp, size_type& sz);
        void get_option(const get_option_func& op, const std::string& name, std::string& val);

        void get_option_int(const get_option_int_func& op, const std::string& name, int& val);
        void get_option_sz(const get_option_sz_func& op, const std::string& name, size_type& val);

        void get_option_ms(const get_option_ms_func& op, const std::string& name, duration_rep_type& val);

    public:

        virtual ~options_reader();

        virtual void get_option(const std::string& name, void* valp, size_type& szp) = 0;

        virtual void get_option(const std::string& name, std::string& val, size_type& sz) = 0;
        virtual void get_option(const std::string& name, std::string& val) = 0;

        virtual void get_option_int(const std::string& name, int& val) = 0;
        virtual void get_option_sz(const std::string& name, size_type& val) = 0;

        virtual void get_option(const std::string& name, duration_type& val) = 0;
        virtual void get_option_ms(const std::string& name, duration_rep_type& val) = 0;
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
