#ifndef NNGCPP_DIALER_H
#define NNGCPP_DIALER_H

#include "enums.h"
#include "endpoint.h"
#include "options.h"

namespace nng {

#ifndef NNGCPP_SOCKET_H
    class socket;
#endif // NNGCPP_SOCKET_H

    class dialer : public endpoint, public options_reader, public options_writer {
    public:

        typedef ::nng_dialer nng_type;

        private:

            friend class socket;

            nng_type did;

        public:

            dialer();

            dialer(const socket* const sp, const std::string& addr);

            virtual ~dialer();

            virtual void start(flag_type flags = flag_none);
            virtual void close();

            virtual bool has_one() const override;

            virtual void get_option(const std::string& name, std::string& val);
            virtual void get_option(const std::string& name, std::string& val, size_type& sz);

            virtual void set_option(const std::string& name, const void* v, size_type sz);
            virtual void get_option(const std::string& name, void* val, size_type* szp);

            virtual void get_option_int(const std::string& name, int* valp);
            virtual void get_option_size(const std::string& name, size_type* valp);
            virtual void get_option_ms(const std::string& name, duration_type* valp);

            virtual void set_option(const std::string& name, const std::string& val, size_type sz);
            virtual void set_option(const std::string& name, const std::string& val);

            virtual void set_option_int(const std::string& name, int val);
            virtual void set_option_size(const std::string& name, size_type val);
            virtual void set_option_ms(const std::string& name, duration_type val);
    };
}

#endif // NNGCPP_DIALER_H
