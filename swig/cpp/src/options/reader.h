#ifndef NNGCPP_OPTIONS_READER_H
#define NNGCPP_OPTIONS_READER_H

#include "../core/types.h"

#include <string>
#include <functional>

namespace nng {

#ifndef NNGCPP_SOCKET_H
    class _Socket;
#endif // NNGCPP_SOCKET_H

#ifndef NNGCPP_ADDRESS_H
    class _SockAddr;
#endif // NNGCPP_ADDRESS_H

#ifndef NNGCPP_LISTENER_H
    class _Listener;
#endif // NNGCPP_LISTENER_H

#ifndef NNGCPP_DIALER_H
    class _Dialer;
#endif // NNGCPP_DIALER_H

#ifndef NNGCPP_DIALER_H
    class message_pipe;
#endif // NNGCPP_DIALER_H

    class _OptionReader;

#ifndef NNGCPP_OPTIONS_READER_WRITER_H
    class _OptionReaderWriter;
#endif // NNGCPP_OPTIONS_READER_WRITER_H

    struct _BasicOptionReader {
    public:

        typedef std::function<int(const char*, void* valp, size_type* szp)> getopt_func;

        typedef std::function<int(const char*, int* valp)> getopt_int_func;
        typedef std::function<int(const char*, size_type* valp)> getopt_sz_func;

        typedef std::function<int(const char*, duration_rep_type* valp)> getopt_duration_func;

    private:

        friend class _OptionReader;
        friend class _OptionReaderWriter;

        const getopt_func _getopt;
        const getopt_int_func _getopt_int;
        const getopt_sz_func _getopt_sz;
        const getopt_duration_func _getopt_duration;

    protected:

        friend class _Socket;
        friend class _Listener;
        friend class _Dialer;
        friend class message_pipe;

        // TODO: TBD: making them public against my better judgment; however friendship web is getting kind of sticky IMHO...
        void set_getters(const getopt_func& getopt
            , const getopt_int_func& getopt_int
            , const getopt_sz_func& getopt_sz
            , const getopt_duration_func& getopt_duration);

    public:

        virtual ~_BasicOptionReader();

        virtual void get(const std::string& name, void* valp, size_type& szp) = 0;

        virtual std::string GetText(const std::string& name) = 0;
        virtual std::string GetText(const std::string& name, size_type& sz) = 0;

        virtual int32_t GetInt32(const std::string& name) = 0;
        virtual size_type GetSize(const std::string& name) = 0;

        virtual duration_type GetDuration(const std::string& name) = 0;
        virtual duration_rep_type GetMilliseconds(const std::string& name) = 0;

        virtual _SockAddr GetSocketAddress(const std::string& name) = 0;

    protected:

        _BasicOptionReader();
    };

#ifndef NNGCPP_OPTIONS_HAVE_OPTIONS_HPP
    template<class Options_> struct IHaveOptions;
#endif // NNGCPP_OPTIONS_HAVE_OPTIONS_HPP

    class _OptionReader : public _BasicOptionReader {
    protected:

        template<class Options_> friend struct IHaveOptions;

        _OptionReader();

    public:

        virtual ~_OptionReader();

        virtual void get(const std::string& name, void* valp, size_type& szp);

        virtual std::string GetText(const std::string& name);
        virtual std::string GetText(const std::string& name, size_type& sz);

        virtual int32_t GetInt32(const std::string& name);
        virtual size_type GetSize(const std::string& name);

        virtual duration_type GetDuration(const std::string& name);
        virtual duration_rep_type GetMilliseconds(const std::string& name);

        virtual _SockAddr GetSocketAddress(const std::string& name);
    };

    typedef _BasicOptionReader basic_options_reader;
    typedef _OptionReader options_reader;
}

#endif // NNGCPP_OPTIONS_READER_H
