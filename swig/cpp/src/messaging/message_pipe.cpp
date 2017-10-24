#include "message_pipe.h"
#include "message_base.h"
#include "../core/invocation.hpp"

namespace nng {

    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;
    using std::bind;

    message_pipe::nng_type get_id(const message_pipe& mp) {
        return mp.pid;
    }

    message_pipe::message_pipe(message_base* const mbp)
        : having_one(), can_close()
        , pid(0), _options() {

        // TODO: TBD: throw an exception upon irregular pipe value...
        const auto& op = bind(&::nng_msg_get_pipe, get_msgp(mbp));
        invocation::with_result(op, &pid);
        configure_options(pid);
    }

    // TODO: TBD: this is looking very similar to socket. perhaps it is...
    message_pipe::~message_pipe() {
        close();
    }

    void message_pipe::close() {
        if (!has_one()) { return; }
        const auto op = bind(&::nng_pipe_close, pid);
        // TODO: TBD: should be fine trapping non-ec_enone RV's only; but consider whether ec_enoent was appropriate...
        invocation::with_error_handling_if_not_one_of(op, { ec_enone, ec_enoent });
        configure_options(pid = 0);
    }

    bool message_pipe::has_one() const {
        return pid > 0;
    }

    void message_pipe::configure_options(nng_type pid) {

        _options.set_getters(
            bind(&::nng_pipe_getopt, pid, _1, _2, _3)
            , bind(&::nng_pipe_getopt_int, pid, _1, _2)
            , bind(&::nng_pipe_getopt_size, pid, _1, _2)
            , bind(&::nng_pipe_getopt_ms, pid, _1, _2)
        );
    }

    options_reader* const message_pipe::options() {
        return &_options;
    }

    bool message_pipe::operator==(const message_pipe& rhs) {
        return pid == rhs.pid;
    }

    bool message_pipe::operator!=(const message_pipe& rhs) {
        return pid != rhs.pid;
    }
}
