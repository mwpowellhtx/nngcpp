#ifndef NNGCPP_OPTIONS_H
#define NNGCPP_OPTIONS_H

#include "types.h"

#include <string>
#include <functional>

namespace nng {

#ifndef NNGCPP_ADDRESS_H
    class address;
#endif // NNGCPP_ADDRESS_H

#ifndef NNGCPP_SOCKET_H
    class socket;
#endif // NNGCPP_SOCKET_H
//
#ifndef NNGCPP_LISTENER_H
    class listener;
#endif //NNGCPP_LISTENER_H

#ifndef NNGCPP_DIALER_H
    class dialer;
#endif // NNGCPP_DIALER_H

#ifndef NNGCPP_MESSAGE_PIPE_H
    class message_pipe;
#endif //NNGCPP_MESSAGE_PIPE_H

    class options_writer {
    public:

        typedef std::function<int(const char*, const void*, size_type)> setopt_func;

        typedef std::function<int(const char*, int val)> setopt_int_func;
        typedef std::function<int(const char*, size_type val)> setopt_sz_func;

        typedef std::function<int(const char*, duration_rep_type)> setopt_duration_func;

    private:

        const setopt_func _setopt;
        const setopt_int_func _setopt_int;
        const setopt_sz_func _setopt_sz;
        const setopt_duration_func _setopt_duration;

    protected:

        options_writer();

        // TODO: TBD: ditto sticky friendship web...
        void set_setters(const setopt_func& setopt
            , const setopt_int_func& setopt_int
            , const setopt_sz_func& setopt_sz
            , const setopt_duration_func& setopt_duration);

    public:

        virtual ~options_writer();

        virtual void set(const std::string& name, const void* valp, size_type sz);
        virtual void set(const std::string& name, const std::string& val);

        virtual void set_int(const std::string& name, int val);
        virtual void set_sz(const std::string& name, size_type val);

        virtual void set(const std::string& name, const duration_type& val);
        virtual void set_milliseconds(const std::string& name, duration_rep_type val);
    };

    class options_reader {
    public:

        typedef std::function<int(const char*, void* valp, size_type* szp)> getopt_func;

        typedef std::function<int(const char*, int* valp)> getopt_int_func;
        typedef std::function<int(const char*, size_type* valp)> getopt_sz_func;

        typedef std::function<int(const char*, duration_rep_type* valp)> getopt_duration_func;

    private:

        const getopt_func _getopt;
        const getopt_int_func _getopt_int;
        const getopt_sz_func _getopt_sz;
        const getopt_duration_func _getopt_duration;

    protected:

        friend class message_pipe;

        options_reader();

        // TODO: TBD: making them public against my better judgment; however friendship web is getting kind of sticky IMHO...
        void set_getters(const getopt_func& getopt
            , const getopt_int_func& getopt_int
            , const getopt_sz_func& getopt_sz
            , const getopt_duration_func& getopt_duration);

    public:

        virtual ~options_reader();

        virtual void get(const std::string& name, void* valp, size_type& szp);

        virtual void get(const std::string& name, std::string& val, size_type& sz);
        virtual void get(const std::string& name, std::string& val);
        virtual void get(const std::string& name, address& val);

        virtual void get_int(const std::string& name, int& val);
        virtual void get_sz(const std::string& name, size_type& val);

        virtual void get(const std::string& name, duration_type& val);
        virtual void get_milliseconds(const std::string& name, duration_rep_type& val);
    };

    class options_reader_writer : public options_reader, public options_writer {
    protected:

        friend class socket;
        friend class dialer;
        friend class listener;

        options_reader_writer();

        virtual ~options_reader_writer();
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
