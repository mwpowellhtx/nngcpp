#ifndef NNGCPP_ASYNC_OPTIONS_WRITER_H
#define NNGCPP_ASYNC_OPTIONS_WRITER_H

#include "../types.h"

#include <functional>

namespace nng {

#ifndef NNGCPP_BASIC_ASYNC_SERVICE_H
    class _BasicAsyncService;
#endif // NNGCPP_BASIC_ASYNC_SERVICE_H

#ifndef NNGCPP_OPTIONS_HAVE_OPTIONS_HPP
    template<class Options_> struct IHaveOptions;
#endif // NNGCPP_OPTIONS_HAVE_OPTIONS_HPP

    struct _AsyncOptionWriter {
    private:

        typedef std::function<void(duration_rep_type)> setopt_duration_func;

        const setopt_duration_func _setopt_duration;

    protected:

        template<class Options_> friend struct IHaveOptions;

        friend class _BasicAsyncService;

        // TODO: TBD: ditto sticky friendship web...
        void set_setters(const setopt_duration_func& setopt_duration);

        _AsyncOptionWriter();

    public:

        virtual ~_AsyncOptionWriter();

        virtual void SetTimeoutDuration(const duration_type& val);
        virtual void SetTimeoutMilliseconds(duration_rep_type val);
    };

    typedef _AsyncOptionWriter async_options_writer;
}

#endif // NNGCPP_ASYNC_OPTIONS_WRITER_H
