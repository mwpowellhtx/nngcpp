#include "message_pipe.h"
#include "message_base.h"
#include "../core/invocation.hpp"

namespace nng {

    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;
    using std::bind;

    message_pipe::message_pipe(msg_type* const msgp)
        : having_one(), can_close()
        , pid(0), _msgp(msgp), _options()
        , __getter(bind(&::nng_msg_get_pipe, _msgp))
        , __setter(), __closer() {

        invocation::with_result(__getter, &pid);
        configure(pid, _msgp);
    }

    message_pipe::message_pipe(message_base* const mbp)
        : message_pipe(mbp->get_msgp()) {
    }

    // TODO: TBD: this is looking very similar to socket. perhaps it is...
    message_pipe::~message_pipe() {
        close();
    }

    void message_pipe::close() {
        if (!has_one()) { return; }
        // TODO: TBD: should be fine trapping non-ec_enone RV's only; but consider whether ec_enoent was appropriate...
        invocation::with_error_handling_if_not_one_of(__closer, { ec_enone, ec_enoent });
        configure(pid = 0);
    }

    bool message_pipe::has_one() const {
        return pid > 0;
    }

    void message_pipe::reset() {
        // Which means that the Message Pipe is now (re-)connected with the Message. And that's it.
        invocation::with_void_return_value(__setter);
    }

    void message_pipe::set(msg_type* const msgp) {
        // Re-bind the getter to keep aligned with the new message.
        __getter = bind(&::nng_msg_get_pipe, _msgp = msgp);
        // Except in this case we want to preserve the current PID.
        configure(pid, _msgp);
        // And in which case we simply want to pass the Message Pipe along to the caller.
        invocation::with_void_return_value(__setter);
    }

    void message_pipe::set(message_base* const mbp) {
        set(mbp->get_msgp());
    }

    void message_pipe::configure(nng_type pid, msg_type* msgp) {

        __setter = bind(&::nng_msg_set_pipe, msgp, pid);
        __closer = bind(&::nng_pipe_close, pid);

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
