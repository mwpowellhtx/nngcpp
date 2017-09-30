#ifndef NNGCPP_OPTIONS_H
#define NNGCPP_OPTIONS_H

#include <cstddef>
#include <cstdint>

namespace nng {

    class socket;

    class NNGCPP_DECLSPEC options {
        protected:

            typedef std::size_t option_size_type;
            typedef uint64_t option_ulonglong_type;

            options();

        public:

            virtual void set_option(int opt, const void* v, option_size_type sz) = 0;
            virtual void set_option_int(int opt, int val) = 0;
            virtual void set_option_size(int opt, option_size_type val) = 0;
            virtual void set_option_usec(int opt, option_ulonglong_type val) = 0;

            virtual void get_option(int opt, void* val, option_size_type* szp) = 0;
            virtual void get_option_int(int opt, int* valp) = 0;
            virtual void get_option_size(int opt, option_size_type* valp) = 0;
            virtual void get_option_usec(int opt, option_ulonglong_type* valp) = 0;
    };
}

#endif // NNGCPP_OPTIONS_H
