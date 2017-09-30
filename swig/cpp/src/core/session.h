#ifndef NNGCPP_SESSION_H
#define NNGCPP_SESSION_H

// TODO: TBD: considering the NNG fini teardown, it may be appropriate to emit appropriately protocoled sockets from the session as well...
namespace nng {

    class NNGCPP_DECLSPEC session {
        public:

            session();

            virtual ~session();
    };
}

#endif // NNGCPP_SESSION_H
