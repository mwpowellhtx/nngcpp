#ifndef NNGCPP_DIALER_H
#define NNGCPP_DIALER_H

#include "../nngcpp_integration.h"

#include "endpoint.h"
#include "options.h"

namespace nng {

#ifndef NNGCPP_SOCKET_H
    class socket;
#endif // NNGCPP_SOCKET_H

    class dialer : public endpoint, public options {
    public:

        typedef ::nng_dialer nng_type;

        private:

            friend class socket;

            nng_type did;

        public:

            dialer();

            dialer(const socket& s, const std::string& addr);

            virtual ~dialer();

            void start(int flags);
            void close();

            virtual void set_option_int(const std::string* const name, int val);
            virtual void set_option_size(const std::string* const name, option_size_type val);
            virtual void set_option_usec(const std::string* const name, option_ulonglong_type val);

            virtual void get_option_int(const std::string* const name, int* valp);
            virtual void get_option_size(const std::string* const name, option_size_type* valp);
            virtual void get_option_usec(const std::string* const name, option_ulonglong_type* valp);
    };
}

#endif // NNGCPP_DIALER_H
