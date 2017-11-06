#ifndef NNGCPP_OPTIONS_HAVE_OPTIONS_HPP
#define NNGCPP_OPTIONS_HAVE_OPTIONS_HPP

namespace nng {

#ifndef NNGCPP_SOCKET_H
    class _Socket;
#endif // NNGCPP_SOCKET_H

#ifndef NNGCPP_BASIC_ASYNC_SERVICE_H
    class _BasicAsyncService;
#endif // NNGCPP_BASIC_ASYNC_SERVICE_H

    template<class Options_>
    struct IHaveOptions {

        typedef Options_ options_type;
        typedef IHaveOptions<Options_> have_options_type;

    private:

        options_type _options;

    protected:

        friend class _Socket;

        IHaveOptions() : _options() {}

    public:

        virtual ~IHaveOptions() {}

        virtual options_type* const GetOptions() {
            return &_options;
        }
    };
}

#endif // NNGCPP_OPTIONS_HAVE_OPTIONS_HPP
