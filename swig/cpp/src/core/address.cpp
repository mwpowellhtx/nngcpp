#include "address.h"

namespace nng {

    address::address() : _sa() {
    }

    address::~address() {
    }

    bool address::has_one() const {
        return true;
    }

    address::size_type address::get_size() const {
        return sizeof(_sa);
    }

    address::sockaddr_type* address::get() {
        return &_sa;
    }

    sockaddr_family_type address::get_s_family() const {
        return static_cast<sockaddr_family_type>(_sa.s_un.s_family);
    }

    uint32_t address::get_addr() const {
        // TODO: TBD: ditto port re: family... i.e. .s_in because family was af_inet ???
        return _sa.s_un.s_in.sa_addr;
    }

    uint16_t address::get_port() const {
        // TODO: TBD: may depend upon the family as to which port we want here...
        return _sa.s_un.s_in.sa_port;
    }
}
