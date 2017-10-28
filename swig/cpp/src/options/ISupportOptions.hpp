#ifndef NNGCPP_OPTIONS_SUPPORT_OPTIONS_HPP
#define NNGCPP_OPTIONS_SUPPORT_OPTIONS_HPP

namespace nng {

    template<class Options_>
    struct ISupportOptions {

        typedef Options_ options_type;

    private:

        options_type _options;

    protected:

        typedef ISupportOptions<Options_> support_options_type;

        ISupportOptions() : _options() {}

    public:

        virtual ~ISupportOptions() {}

        virtual options_type* const GetOptions() {
            return &_options;
        }
    };
}

#endif // NNGCPP_OPTIONS_SUPPORT_OPTIONS_HPP
