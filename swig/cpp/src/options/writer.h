#ifndef NNGCPP_OPTIONS_WRITER_H
#define NNGCPP_OPTIONS_WRITER_H

#include "../core/types.h"

#include <string>
#include <functional>

namespace nng {

#ifndef NNGCPP_SOCKET_H
    class socket;
#endif // NNGCPP_SOCKET_H

#ifndef NNGCPP_LISTENER_H
    class listener;
#endif // NNGCPP_LISTENER_H

#ifndef NNGCPP_DIALER_H
    class dialer;
#endif // NNGCPP_DIALER_H

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

        friend class socket;
        friend class listener;
        friend class dialer;

        // TODO: TBD: ditto sticky friendship web...
        void set_setters(const setopt_func& setopt
            , const setopt_int_func& setopt_int
            , const setopt_sz_func& setopt_sz
            , const setopt_duration_func& setopt_duration);

    public:

        options_writer();

        virtual ~options_writer();

        virtual void set(const std::string& name, const void* valp, size_type sz);
        virtual void set(const std::string& name, const std::string& val);

        virtual void set_int(const std::string& name, int val);
        virtual void set_sz(const std::string& name, size_type val);

        virtual void set(const std::string& name, const duration_type& val);
        virtual void set_milliseconds(const std::string& name, duration_rep_type val);
    };
}

#endif // NNGCPP_OPTIONS_WRITER_H
