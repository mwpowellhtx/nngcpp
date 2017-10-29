#ifndef NNGCPP_OPTIONS_SUPPORT_OPTIONS_HPP
#define NNGCPP_OPTIONS_SUPPORT_OPTIONS_HPP

namespace nng {

#ifndef NNGCPP_OPTIONS_READER_H
    class _OptionReader;
#endif //NNGCPP_OPTIONS_READER_H

#ifndef NNGCPP_OPTIONS_WRITER_H
    class _OptionWriter;
#endif //NNGCPP_OPTIONS_WRITER_H

#ifndef NNGCPP_OPTIONS_READER_WRITER_H
    class _OptionReaderWriter;
#endif //NNGCPP_OPTIONS_READER_WRITER_H

#ifndef NNGCPP_SOCKET_H
    class socket;
#endif // NNGCPP_SOCKET_H

    template<class Options_>
    struct ISupportOptions {

        typedef Options_ options_type;
        typedef ISupportOptions<Options_> interface_type;

    private:

        options_type _options;

    protected:

        friend class socket;
        friend class _OptionReader;
        friend class _OptionWriter;
        friend class _OptionReaderWriter;

        ISupportOptions() : _options() {}

    public:

        virtual ~ISupportOptions() {}

        virtual options_type* const GetOptions() {
            return &_options;
        }
    };
}

#endif // NNGCPP_OPTIONS_SUPPORT_OPTIONS_HPP
