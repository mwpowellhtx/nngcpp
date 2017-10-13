#include "message_pipe.h"
#include "message_base.h"
#include "../core/exceptions.hpp"

namespace nng {

    namespace messaging {

        message_pipe::message_pipe(const message_base* const mbp)
            : options()
            , pid(::nng_msg_get_pipe(mbp->_msgp)) {

            // TODO: TBD: throw an exception upon irregular pipe value...
        }

        // TODO: TBD: this is looking very similar to socket. perhaps it is...
        message_pipe::~message_pipe() {
            close();
        }

        void message_pipe::close() {
            if (!is_valid()) { return; }
            const auto errnum = ::nng_pipe_close(pid);
            THROW_NNG_EXCEPTION_EC(errnum);
            pid = 0;
        }

        bool message_pipe::is_valid() const {
            return pid > 0;
        }

        bool message_pipe::operator==(const message_pipe& rhs) {
            return pid == rhs.pid;
        }

        bool message_pipe::operator!=(const message_pipe& rhs) {
            return pid != rhs.pid;
        }

        void message_pipe::set_option(const std::string& name, const std::string& val, option_size_type sz) {
            const auto errnum = ::nng_setopt(pid, name.c_str(), val.c_str(), sz);
            THROW_NNG_EXCEPTION_EC(errnum);
        }

        void message_pipe::set_option(const std::string& name, const std::string& val) {
            const auto errnum = ::nng_setopt(pid, name.c_str(), val.c_str(), val.length());
            THROW_NNG_EXCEPTION_EC(errnum);
        }

        void message_pipe::get_option(const std::string& name, std::string& val) {
            option_size_type sz = val.size();
            const auto errnum = ::nng_getopt(pid, name.c_str(), (void*)&val[0], &sz);
            THROW_NNG_EXCEPTION_EC(errnum);
            // Do this because the C++ buffer does not appear to be honored quite right when working with the C buffers.
            val.resize(sz - 1);
        }

        void message_pipe::get_option(const std::string& name, std::string& val, option_size_type& sz) {
            const auto errnum = ::nng_getopt(pid, name.c_str(), (void*)&val[0], &sz);
            THROW_NNG_EXCEPTION_EC(errnum);
            // Do this because the C++ buffer does not appear to be honored quite right when working with the C buffers.
            val.resize(sz - 1);
        }

        void message_pipe::set_option(const std::string& name, const void* valp, option_size_type sz) {
            const auto errnum = ::nng_setopt(pid, name.c_str(), valp, sz);
            THROW_NNG_EXCEPTION_EC(errnum);
        }

        void message_pipe::get_option(const std::string& name, void* val, option_size_type* szp) {
            const auto errnum = ::nng_getopt(pid, name.c_str(), val, szp);
            THROW_NNG_EXCEPTION_EC(errnum);
        }

        void message_pipe::set_option_int(const std::string& name, int val) {
            const auto errnum = ::nng_setopt_int(pid, name.c_str(), val);
            THROW_NNG_EXCEPTION_EC(errnum);
        }

        void message_pipe::set_option_size(const std::string& name, option_size_type val) {
            const auto errnum = ::nng_setopt_size(pid, name.c_str(), val);
            THROW_NNG_EXCEPTION_EC(errnum);
        }

        void message_pipe::set_option_usec(const std::string& name, option_ulonglong_type val) {
            const auto errnum = ::nng_setopt_usec(pid, name.c_str(), val);
            THROW_NNG_EXCEPTION_EC(errnum);
        }

        void message_pipe::get_option_int(const std::string& name, int* valp) {
            const auto errnum = ::nng_getopt_int(pid, name.c_str(), valp);
            THROW_NNG_EXCEPTION_EC(errnum);
        }

        void message_pipe::get_option_size(const std::string& name, option_size_type* valp) {
            const auto errnum = ::nng_getopt_size(pid, name.c_str(), valp);
            THROW_NNG_EXCEPTION_EC(errnum);
        }

        void message_pipe::get_option_usec(const std::string& name, option_ulonglong_type* valp) {
            const auto errnum = ::nng_getopt_usec(pid, name.c_str(), valp);
            THROW_NNG_EXCEPTION_EC(errnum);
        }
    }
}
