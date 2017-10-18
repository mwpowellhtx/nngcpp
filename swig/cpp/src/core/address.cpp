#include "address.h"
#include "exceptions.hpp"

#define THROW_VIEW_NOT_IMPLEMENTED_EX(name) throw trx::not_implemented(#name " view is not implemented for the address family");

namespace nng {

    family_view_base::family_view_base(::nng_sockaddr* const sap, sockaddr_family_type jewel)
        : _sap(sap)
        , _jewel(jewel) {
    }

    family_view_base::~family_view_base() {
    }

    sockaddr_family_type family_view_base::get_jewel() const {
        return _jewel;
    }

    bool family_view_base::has_one() const {
        return _sap != nullptr&&_sap->s_un.s_family == get_jewel();
    }

    uint32_t family_view_base::get_addr() const {
        THROW_VIEW_NOT_IMPLEMENTED_EX(get_addr);
    }

    family_view_base::in6_addr_vector_type family_view_base::get_in6_addr() const {
        THROW_VIEW_NOT_IMPLEMENTED_EX(get_addr_vector);
    }

    uint16_t family_view_base::get_port() const {
        THROW_VIEW_NOT_IMPLEMENTED_EX(get_port);
    }

    unspec_family_view::unspec_family_view(::nng_sockaddr* const sap)
        : family_view_base(sap, af_unspec) {
    }

    unspec_family_view::~unspec_family_view() {
    }

    template<sockaddr_family_type Type_, class Detail_>
    family_view<Type_, Detail_>::family_view(::nng_sockaddr* const sap) : family_view_base(sap, Type_) {
    }

    template<sockaddr_family_type Type_, class Detail_>
    family_view<Type_, Detail_>::~family_view() {
    }

    template<sockaddr_family_type Type_, class Detail_>
    sockaddr_family_type family_view<Type_, Detail_>::get_type() const {
        return Type_;
    }

    ipc_family_view::ipc_family_view(::nng_sockaddr* const sap) : family_view(sap) {
    }

    ipc_family_view::~ipc_family_view() {
    }

    ::nng_sockaddr_ipc* ipc_family_view::get_detail() const {
        return &_sap->s_un.s_path;
    }

    inproc_family_view::inproc_family_view(::nng_sockaddr* const sap) : family_view(sap) {
    }

    inproc_family_view::~inproc_family_view() {
    }

    ::nng_sockaddr_inproc* inproc_family_view::get_detail() const {
        return &_sap->s_un.s_inproc;
    }

    inet_family_view::inet_family_view(::nng_sockaddr* const sap)
        : family_view(sap) {
    }

    inet_family_view::~inet_family_view() {
    }

    ::nng_sockaddr_in* inet_family_view::get_detail() const {
        return &_sap->s_un.s_in;
    }

    uint32_t inet_family_view::get_addr() const {
        return get_detail()->sa_addr;
    }

    uint16_t inet_family_view::get_port() const {
        return get_detail()->sa_port;
    }

    inet6_family_view::inet6_family_view(::nng_sockaddr* const sap)
        : family_view(sap) {
    }

    inet6_family_view::~inet6_family_view() {
    }

    ::nng_sockaddr_in6* inet6_family_view::get_detail() const {
        return &_sap->s_un.s_in6;
    }

    inet6_family_view::in6_addr_vector_type inet6_family_view::get_in6_addr() const {
        const auto sz = sizeof(::nng_sockaddr_in6::sa_addr);
        const auto raw = get_detail()->sa_addr;
        return in6_addr_vector_type(raw, raw + sz);
    }

    uint16_t inet6_family_view::get_port() const {
        return get_detail()->sa_port;
    }

    zt_family_view::zt_family_view(::nng_sockaddr* const sap) : family_view(sap) {
    }

    zt_family_view::~zt_family_view() {
    }

    ::nng_sockaddr_zt* zt_family_view::get_detail() const {
        return &_sap->s_un.s_zt;
    }

    address::address() : _sa(), _view() {
        // TODO: TBD: this is probably as safe an assumption as possible...
        std::memset(&_sa, 0, sizeof(_sa));
    }

    address::~address() {
    }

    void address::align_view() {

        // Should be safe to do this regardless of the state of the union.
        const auto actual_jewel = _sa.s_un.s_family;

        if (_view != nullptr && _view->get_jewel() == actual_jewel) {
            return;
        }

        // For use regardless of the intended view.
        const auto sap = get();

        // Trusting that Jewel is not somehow otherwise accidentally correct!
        switch (actual_jewel) {
        case af_unspec:
            _view.reset(new unspec_family_view(sap));
            break;
        case af_inproc:
            _view.reset(new inproc_family_view(sap));
            break;
        case af_ipc:
            _view.reset(new ipc_family_view(sap));
            break;
        case af_inet:
            _view.reset(new inet_family_view(sap));
            break;
        case af_inet6:
            _view.reset(new inet6_family_view(sap));
            break;
        case af_zt:
            _view.reset(new zt_family_view(sap));
            break;
        }
    }

    bool address::has_one() {
        align_view();
        return view() != nullptr;
    }

    address::size_type address::get_size() const {
        return sizeof(_sa);
    }

    address::sockaddr_type* address::get() {
        return &_sa;
    }

    family_view_base* const address::view() {
        align_view();
        return _view.get();
    }

    sockaddr_family_type address::get_family() const {
        return static_cast<sockaddr_family_type>(_sa.s_un.s_family);
    }
}
