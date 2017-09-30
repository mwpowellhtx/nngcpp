#ifndef NNGCPP_DIALER_H
#define NNGCPP_DIALER_H

// nng should be in the include path.
#include <string>

#include "nng.h"
#include "endpoint.h"
#include "options.h"

namespace nng {

    class socket;

    class NNGCPP_DECLSPEC dialer : public endpoint, public options {
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

            virtual void set_option(int opt, const void* v, option_size_type sz);
            virtual void set_option_int(int opt, int val);
            virtual void set_option_size(int opt, option_size_type val);
            virtual void set_option_usec(int opt, option_ulonglong_type val);

            virtual void get_option(int opt, void* val, option_size_type* szp);
            virtual void get_option_int(int opt, int* valp);
            virtual void get_option_size(int opt, option_size_type* valp);
            virtual void get_option_usec(int opt, option_ulonglong_type* valp);
    };
}

#endif // NNGCPP_DIALER_H
