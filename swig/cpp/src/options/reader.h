#ifndef NNGCPP_OPTIONS_READER_H
#define NNGCPP_OPTIONS_READER_H

#include "../core/types.h"

#include <string>
#include <functional>

namespace nng {

#ifndef NNGCPP_ADDRESS_H
    class address;
#endif // NNGCPP_ADDRESS_H

#ifndef NNGCPP_SOCKET_H
    class socket;
#endif // NNGCPP_SOCKET_H

#ifndef NNGCPP_LISTENER_H
    class listener;
#endif // NNGCPP_LISTENER_H

#ifndef NNGCPP_DIALER_H
    class dialer;
#endif // NNGCPP_DIALER_H

#ifndef NNGCPP_DIALER_H
    class message_pipe;
#endif // NNGCPP_DIALER_H

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

        friend class socket;
        friend class listener;
        friend class dialer;
        friend class message_pipe;

        // TODO: TBD: making them public against my better judgment; however friendship web is getting kind of sticky IMHO...
        void set_getters(const getopt_func& getopt
            , const getopt_int_func& getopt_int
            , const getopt_sz_func& getopt_sz
            , const getopt_duration_func& getopt_duration);

    public:

        options_reader();

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
}

#endif // NNGCPP_OPTIONS_READER_H
