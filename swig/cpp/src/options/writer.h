#ifndef NNGCPP_OPTIONS_WRITER_H
#define NNGCPP_OPTIONS_WRITER_H

#include "../core/types.h"

#include <string>
#include <functional>

namespace nng {

#ifndef NNGCPP_SOCKET_H
    class _Socket;
#endif // NNGCPP_SOCKET_H

#ifndef NNGCPP_LISTENER_H
    class _Listener;
#endif // NNGCPP_LISTENER_H

#ifndef NNGCPP_DIALER_H
    class _Dialer;
#endif // NNGCPP_DIALER_H

    class _OptionWriter;

#ifndef NNGCPP_OPTIONS_READER_WRITER_H
    class _OptionReaderWriter;
#endif // NNGCPP_OPTIONS_READER_WRITER_H

    struct _BasicOptionWriter {
    public:

        typedef std::function<int(const char*, const void*, size_type)> setopt_func;

        typedef std::function<int(const char*, int val)> setopt_int_func;
        typedef std::function<int(const char*, size_type val)> setopt_sz_func;

        typedef std::function<int(const char*, duration_rep_type)> setopt_duration_func;

    private:

        friend class _OptionWriter;
        friend class _OptionReaderWriter;

        const setopt_func _setopt;
        const setopt_int_func _setopt_int;
        const setopt_sz_func _setopt_sz;
        const setopt_duration_func _setopt_duration;

    protected:

        friend class _Socket;
        friend class _Listener;
        friend class _Dialer;

        // TODO: TBD: ditto sticky friendship web...
        void set_setters(const setopt_func& setopt
            , const setopt_int_func& setopt_int
            , const setopt_sz_func& setopt_sz
            , const setopt_duration_func& setopt_duration);

    public:

        virtual ~_BasicOptionWriter();

        virtual void set(const std::string& name, const void* valp, size_type sz) = 0;
        virtual void SetString(const std::string& name, const std::string& s) = 0;

        virtual void SetInt32(const std::string& name, int32_t val) = 0;
        virtual void SetSize(const std::string& name, size_type val) = 0;

        virtual void SetDuration(const std::string& name, const duration_type& val) = 0;
        virtual void SetMilliseconds(const std::string& name, duration_rep_type val) = 0;

    protected:

        _BasicOptionWriter();
    };

#ifndef NNGCPP_OPTIONS_HAVE_OPTIONS_HPP
    template<class Options_> struct IHaveOptions;
#endif // NNGCPP_OPTIONS_HAVE_OPTIONS_HPP

    class _OptionWriter : public _BasicOptionWriter {
    protected:

        template<class Options_> friend struct IHaveOptions;

        _OptionWriter();

    public:

        virtual ~_OptionWriter();

        virtual void set(const std::string& name, const void* valp, size_type sz);
        virtual void SetString(const std::string& name, const std::string& s);

        virtual void SetInt32(const std::string& name, int32_t val);
        virtual void SetSize(const std::string& name, size_type val);

        virtual void SetDuration(const std::string& name, const duration_type& val);
        virtual void SetMilliseconds(const std::string& name, duration_rep_type val);
    };

    typedef _BasicOptionWriter basic_options_writer;
    typedef _OptionWriter options_writer;
}

#endif // NNGCPP_OPTIONS_WRITER_H
