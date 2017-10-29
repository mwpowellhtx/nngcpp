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

#ifndef NNGCPP_OPTIONS_HAVE_OPTIONS_HPP
    template<class Options_> struct IHaveOptions;
#endif // NNGCPP_OPTIONS_HAVE_OPTIONS_HPP

    class _OptionReader {
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

        template<class Options_> friend struct IHaveOptions;

        // TODO: TBD: making them public against my better judgment; however friendship web is getting kind of sticky IMHO...
        void set_getters(const getopt_func& getopt
            , const getopt_int_func& getopt_int
            , const getopt_sz_func& getopt_sz
            , const getopt_duration_func& getopt_duration);

        _OptionReader();

    public:

        virtual ~_OptionReader();

        virtual void get(const std::string& name, void* valp, size_type& szp);

        virtual std::string GetText(const std::string& name);
        virtual std::string GetText(const std::string& name, size_type* const szp);

        virtual void get(const std::string& name, address& val);

        virtual int32_t GetInt32(const std::string& name);
        virtual size_type GetSize(const std::string& name);

        virtual duration_type GetDuration(const std::string& name);
        virtual duration_rep_type GetMilliseconds(const std::string& name);
    };

    typedef _OptionReader options_reader;
}

#endif // NNGCPP_OPTIONS_READER_H
