#include "message_pipe.h"
#include "message_base.h"
#include "../core/exceptions.hpp"

#define THROW_MSG_PIPE_SETOPT_INV_OP() throw trx::invalid_operation("message pipes cannot set options")

namespace nng {

    namespace messaging {

        using std::placeholders::_1;
        using std::placeholders::_2;
        using std::placeholders::_3;
        using std::placeholders::_4;

        message_pipe::message_pipe(message_base* const mbp)
            : options()
            , pid(::nng_msg_get_pipe(mbp->get_msgp())) {

            // TODO: TBD: throw an exception upon irregular pipe value...
        }

        // TODO: TBD: this is looking very similar to socket. perhaps it is...
        message_pipe::~message_pipe() {
            close();
        }

        void message_pipe::close() {
            if (!has_one()) { return; }
            using std::placeholders::_1;
            const auto op = std::bind(::nng_pipe_close, _1);
            const auto errnum = op(pid);
            THROW_NNG_EXCEPTION_IF_NOT_ONEOF(errnum, ec_eunknown, ec_enone, ec_enoent);
            pid = 0;
        }

        bool message_pipe::has_one() const {
            return pid > 0;
        }

        bool message_pipe::operator==(const message_pipe& rhs) {
            return pid == rhs.pid;
        }

        bool message_pipe::operator!=(const message_pipe& rhs) {
            return pid != rhs.pid;
        }

        void message_pipe::get_option(const std::string& name, std::string& val) {
            size_type sz = val.size();
            const auto& op = std::bind(::nng_pipe_getopt, _1, _2, _3, _4);
            const auto errnum = op(pid, name.c_str(), (void*)val.data(), &sz);
            THROW_NNG_EXCEPTION_EC(errnum);
            //// Do this because the C++ buffer does not appear to be honored quite right when working with the C buffers.
            //val.resize(sz - 1);
        }

        void message_pipe::get_option(const std::string& name, std::string& val, size_type& sz) {
            const auto& op =  std::bind(::nng_pipe_getopt, _1, _2, _3, _4);
            const auto errnum = op(pid, name.c_str(), (void*)&val[0], &sz);
            THROW_NNG_EXCEPTION_EC(errnum);
            //// Do this because the C++ buffer does not appear to be honored quite right when working with the C buffers.
            //val.resize(sz - 1);
        }

        void message_pipe::get_option(const std::string& name, void* val, size_type* szp) {
            const auto& op = std::bind(::nng_pipe_getopt, _1, _2, _3, _4);
            const auto errnum = op(pid, name.c_str(), val, szp);
            THROW_NNG_EXCEPTION_EC(errnum);
        }

        void message_pipe::get_option_int(const std::string& name, int* valp) {
            const auto& op = std::bind(::nng_pipe_getopt_int, _1, _2, _3);
            const auto errnum = op(pid, name.c_str(), valp);
            THROW_NNG_EXCEPTION_EC(errnum);
        }

        void message_pipe::get_option_size(const std::string& name, size_type* valp) {
            const auto& op = std::bind(::nng_pipe_getopt_size, _1, _2, _3);
            const auto errnum = ::nng_getopt_size(pid, name.c_str(), valp);
            THROW_NNG_EXCEPTION_EC(errnum);
        }

        void message_pipe::get_option_usec(const std::string& name, option_ulonglong_type* valp) {
            throw trx::not_implemented("::nng_pipe_getopt_usec missing from the nng source");
            //const auto& op = std::bind(::nng_pipe_getopt_usec, _1, _2, _3);
            //const auto errnum = op(pid, name.c_str(), valp);
            //THROW_NNG_EXCEPTION_EC(errnum);
        }

        // TODO: TBD: here are some additional eligible candidates for unit testing of message pipe alone...
        void message_pipe::set_option(const std::string& name, const std::string& val, size_type sz) {
            THROW_MSG_PIPE_SETOPT_INV_OP();
        }

        void message_pipe::set_option(const std::string& name, const std::string& val) {
            THROW_MSG_PIPE_SETOPT_INV_OP();
        }

        void message_pipe::set_option(const std::string& name, const void* valp, size_type sz) {
            THROW_MSG_PIPE_SETOPT_INV_OP();
        }


        void message_pipe::set_option_int(const std::string& name, int val) {
            THROW_MSG_PIPE_SETOPT_INV_OP();
        }

        void message_pipe::set_option_size(const std::string& name, size_type val) {
            THROW_MSG_PIPE_SETOPT_INV_OP();
        }

        void message_pipe::set_option_usec(const std::string& name, option_ulonglong_type val) {
            THROW_MSG_PIPE_SETOPT_INV_OP();
        }
    }
}
