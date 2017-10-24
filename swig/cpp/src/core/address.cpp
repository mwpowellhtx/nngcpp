#include "address.h"
#include "exceptions.hpp"

#ifdef _WIN32
#   include <WinSock2.h>
#else
#   include <arpa/inet.h>
#endif

#include <sstream>
#include <cstring>

#define THROW_VIEW_NOT_IMPLEMENTED_EX(name) throw nng::exceptions::not_implemented(#name " view is not implemented for the address family");

namespace nng {

    template<sockaddr_family_type Val_>
    const std::string sockaddr_family_name<Val_>::name = "";
    const std::string sockaddr_family_name<af_unspec>::name = SOCKADDR_FAMILY_TO_STRING(af_unspec);
    const std::string sockaddr_family_name<af_inproc>::name = SOCKADDR_FAMILY_TO_STRING(af_inproc);
    const std::string sockaddr_family_name<af_ipc>::name = SOCKADDR_FAMILY_TO_STRING(af_ipc);
    const std::string sockaddr_family_name<af_inet>::name = SOCKADDR_FAMILY_TO_STRING(af_inet);
    const std::string sockaddr_family_name<af_inet6>::name = SOCKADDR_FAMILY_TO_STRING(af_inet6);
    const std::string sockaddr_family_name<af_zt>::name = SOCKADDR_FAMILY_TO_STRING(af_zt);

    family_view_base::family_view_base(sockaddr_type* const sap, sockaddr_family_type jewel)
        : having_one(), equal_to()
        , _sap(sap), _jewel(jewel) {
    }

    family_view_base::~family_view_base() {
    }

    sockaddr_family_type family_view_base::get_jewel() const {
        return _jewel;
    }

    bool family_view_base::has_one() const {
        return _sap != nullptr && _sap->s_un.s_family == get_jewel();
    }

    bool family_view_base::operator==(const family_view_base& other) {
        // Smoke test both of the Views for equality.
        return get_jewel() == other.get_jewel();
    }

    bool family_view_base::operator!=(const family_view_base& other) {
        return !operator==(other);
    }

    uint32_t family_view_base::get_addr() const {
        THROW_VIEW_NOT_IMPLEMENTED_EX(get_addr);
    }

    void family_view_base::set_addr(const uint32_t value) {
        THROW_VIEW_NOT_IMPLEMENTED_EX(set_addr);
    }

    family_view_base::in6_addr_vector_type family_view_base::get_in6_addr() const {
        THROW_VIEW_NOT_IMPLEMENTED_EX(get_in6_addr);
    }

    family_view_base::in6_addr16_vector_type family_view_base::get_in6_addr16() const {
        THROW_VIEW_NOT_IMPLEMENTED_EX(get_in6_addr16);
    }

    family_view_base::in6_addr32_vector_type family_view_base::get_in6_addr32() const {
        THROW_VIEW_NOT_IMPLEMENTED_EX(get_in6_addr32);
    }

    void family_view_base::set_in6_addr(in6_addr_vector_type value) {
        THROW_VIEW_NOT_IMPLEMENTED_EX(set_in6_addr);
    }

    void family_view_base::set_in6_addr16(in6_addr16_vector_type value) {
        THROW_VIEW_NOT_IMPLEMENTED_EX(set_in6_addr16);
    }

    void family_view_base::set_in6_addr32(in6_addr32_vector_type value) {
        THROW_VIEW_NOT_IMPLEMENTED_EX(set_in6_addr32);
    }

    uint16_t family_view_base::get_port() const {
        THROW_VIEW_NOT_IMPLEMENTED_EX(get_port);
    }

    void family_view_base::set_port(const uint16_t value) {
        THROW_VIEW_NOT_IMPLEMENTED_EX(set_port);
    }

    unspec_family_view::unspec_family_view(sockaddr_type* const sap)
        : family_view_base(sap, af_unspec) {
    }

    unspec_family_view::~unspec_family_view() {
    }

    bool unspec_family_view::has_one() const {
        return false;
    }

    sockaddr_family_type unspec_family_view::get_family() const {
        return static_cast<sockaddr_family_type>(_sap->s_un.s_family);
    }

    template<sockaddr_family_type Type_, class Detail_>
    family_view<Type_, Detail_>::family_view(sockaddr_type* const sap) : family_view_base(sap, Type_) {
    }

    template<sockaddr_family_type Type_, class Detail_>
    family_view<Type_, Detail_>::~family_view() {
    }

    inproc_family_view::inproc_family_view(sockaddr_type* const sap) : family_view(sap) {
    }

    inproc_family_view::~inproc_family_view() {
    }

    inproc_family_view::detail_type* inproc_family_view::get_detail() const {
        return &_sap->s_un.s_inproc;
    }

    bool inproc_family_view::operator==(const detail_type& other) {
        const auto detailp = get_detail();
        return std::memcmp(&(detailp->sa_path), &other.sa_path, NNG_MAXADDRLEN) == 0;
    }

    bool inproc_family_view::operator!=(const detail_type& other) {
        return !operator==(other);
    }

    bool inproc_family_view::operator==(const family_view_base& other) {

        if (!family_view_base::operator==(other)) { return false; }

        // We are expecting Views to be Truly Aligned.
        auto op = dynamic_cast<inproc_family_view*>(
            const_cast<family_view_base*>(&other)
            );

        if (!op) { return false; }

        return operator==(*op->get_detail());
    }

    bool inproc_family_view::operator!=(const family_view_base& other) {
        return !operator==(other);
    }

    sockaddr_family_type inproc_family_view::get_family() const {
        return static_cast<sockaddr_family_type>(get_detail()->sa_family);
    }

    ipc_family_view::ipc_family_view(sockaddr_type* const sap) : family_view(sap) {
    }

    ipc_family_view::~ipc_family_view() {
    }

    ipc_family_view::detail_type* ipc_family_view::get_detail() const {
        return &_sap->s_un.s_path;
    }

    bool ipc_family_view::operator==(const detail_type& other) {
        return std::memcmp(&(get_detail()->sa_path), &other.sa_path, NNG_MAXADDRLEN) == 0;
    }

    bool ipc_family_view::operator!=(const detail_type& other) {
        return !operator==(other);
    }

    bool ipc_family_view::operator==(const family_view_base& other) {

        if (!family_view_base::operator==(other)) { return false; }

        // We are expecting Views to be Truly Aligned.
        auto op = dynamic_cast<ipc_family_view*>(
            const_cast<family_view_base*>(&other)
            );

        if (!op) { return false; }

        return operator==(*op->get_detail());
    }

    bool ipc_family_view::operator!=(const family_view_base& other) {
        return !operator==(other);
    }

    sockaddr_family_type ipc_family_view::get_family() const {
        return static_cast<sockaddr_family_type>(get_detail()->sa_family);
    }

    inet_family_view::inet_family_view(sockaddr_type* const sap)
        : family_view(sap) {
    }

    inet_family_view::~inet_family_view() {
    }

    inet_family_view::detail_type* inet_family_view::get_detail() const {
        return &_sap->s_un.s_in;
    }

    bool inet_family_view::operator==(const detail_type& other) {
        const auto detailp = get_detail();
        return detailp->sa_addr == other.sa_family
            && detailp->sa_port == other.sa_port;
    }

    bool inet_family_view::operator!=(const detail_type& other) {
        return !operator==(other);
    }

    bool inet_family_view::operator==(const family_view_base& other) {

        if (!family_view_base::operator==(other)) { return false; }

        // We are expecting Views to be Truly Aligned.
        auto op = dynamic_cast<inet_family_view*>(
            const_cast<family_view_base*>(&other)
            );

        if (!op) { return false; }

        return operator==(*op->get_detail());
    }

    bool inet_family_view::operator!=(const family_view_base& other) {
        return !operator==(other);
    }

    sockaddr_family_type inet_family_view::get_family() const {
        return static_cast<sockaddr_family_type>(get_detail()->sa_family);
    }

    uint32_t inet_family_view::get_addr() const {
        return ::ntohl(get_detail()->sa_addr);
    }

    void inet_family_view::set_addr(const uint32_t value) {
        get_detail()->sa_addr = ::htonl(value);
    }

    uint16_t inet_family_view::get_port() const {
        return ::ntohs(get_detail()->sa_port);
    }

    void inet_family_view::set_port(const uint16_t value) {
        get_detail()->sa_port = ::htons(value);
    }

    inet6_family_view::inet6_family_view(sockaddr_type* const sap)
        : family_view(sap) {
    }

    inet6_family_view::~inet6_family_view() {
    }

    inet6_family_view::detail_type* inet6_family_view::get_detail() const {
        return &_sap->s_un.s_in6;
    }

    bool inet6_family_view::operator==(const detail_type& other) {
        const auto detailp = get_detail();
        return std::memcmp(&(detailp->sa_addr), &(other.sa_addr), 16) == 0
            && detailp->sa_port == other.sa_port;
    }

    bool inet6_family_view::operator!=(const detail_type& other) {
        return !operator==(other);
    }

    bool inet6_family_view::operator==(const family_view_base& other) {

        if (!family_view_base::operator==(other)) { return false; }

        // We are expecting Views to be Truly Aligned.
        auto op = dynamic_cast<inet6_family_view*>(
            const_cast<family_view_base*>(&other)
            );

        if (!op) { return false; }

        return operator==(*op->get_detail());
    }

    bool inet6_family_view::operator!=(const family_view_base& other) {
        return !operator==(other);
    }

    sockaddr_family_type inet6_family_view::get_family() const {
        return static_cast<sockaddr_family_type>(get_detail()->sa_family);
    }

    inet6_family_view::in6_addr_vector_type inet6_family_view::get_in6_addr() const {
        const auto sz = sizeof(::nng_sockaddr_in6::sa_addr);
        auto raw = get_detail()->sa_addr;
        const auto raw_end = raw + sz;
        while (!*raw && raw < raw_end) { ++raw; }
        return in6_addr_vector_type(raw, raw_end);
    }

    inet6_family_view::in6_addr16_vector_type inet6_family_view::get_in6_addr16() const {
        // TODO: TBD: fill in the blanks here... personally I think that could/should be a unionized view in the NNG API, but will need to be ready to do some work here if necessary.
        return family_view_base::get_in6_addr16();
        //const auto sz = sizeof(::nng_sockaddr_in6::sa_addr);
        //const auto raw = get_detail()->sa_addr;
        //return in6_addr16_vector_type(raw, raw + sz);
    }

    inet6_family_view::in6_addr32_vector_type inet6_family_view::get_in6_addr32() const {
        // TODO: TBD: fill in the blanks here... personally I think that could/should be a unionized view in the NNG API, but will need to be ready to do some work here if necessary.
        return family_view_base::get_in6_addr32();
        //const auto sz = sizeof(::nng_sockaddr_in6::sa_addr);
        //const auto raw = get_detail()->sa_addr;
        //return in6_addr32_vector_type(raw, raw + sz);
    }

    // TODO: TBD: decide whether this should go in the header; would allow for testing/vetting of the expected sizes themselves.
    // TODO: TBD: otherwise would need to support testing indirectly via the setters...
    template<typename Vector_>
    struct in6_addr_expected_size
        : std::integral_constant<std::size_t, 0> {
    };

    template<>
    struct in6_addr_expected_size<family_view_base::in6_addr_vector_type>
        : std::integral_constant<std::size_t, 16> {
    };

    template<>
    struct in6_addr_expected_size<family_view_base::in6_addr16_vector_type>
        : std::integral_constant<std::size_t, 8> {
    };

    template<>
    struct in6_addr_expected_size<family_view_base::in6_addr32_vector_type>
        : std::integral_constant<std::size_t, 4> {
    };

    void inet6_family_view::set_in6_addr(in6_addr_vector_type value) {
        if (value.size() <= in6_addr_expected_size<in6_addr_vector_type>::value) {
            return;
        }
        // TODO: TBD: see notes around address unit testing re: IPv6 representation and NNG's representation
        auto raw = get_detail()->sa_addr;
        auto _value_begin = value.begin();
        auto _value_end = value.end();
        for (auto it = _value_begin; it != _value_end; ++it) {
            raw[it - _value_begin] = *it;
        }
    }

    void inet6_family_view::set_in6_addr16(in6_addr16_vector_type value) {
        family_view_base::set_in6_addr16(value);
        // TODO: TBD: ditto address view NNG struct versus wrapper work...
        if (value.size() <= in6_addr_expected_size<in6_addr16_vector_type>::value) {
            return;
        }
        //// TODO: TBD: see notes around address unit testing re: IPv6 representation and NNG's representation
        //auto raw = get_detail()->sa_addr;
        //auto _value_begin = value.begin();
        //auto _value_end = value.end();
        //for (auto it = _value_begin; it != _value_end; ++it) {
        //    raw[it - _value_begin] = *it;
        //}
    }

    void inet6_family_view::set_in6_addr32(in6_addr32_vector_type value) {
        family_view_base::set_in6_addr32(value);
        // TODO: TBD: ditto address view NNG struct versus wrapper work...
        if (value.size() <= in6_addr_expected_size<in6_addr32_vector_type>::value) {
            return;
        }
        std::is_abstract<bool>::value;
        //// TODO: TBD: see notes around address unit testing re: IPv6 representation and NNG's representation
        //auto raw = get_detail()->sa_addr;
        //auto _value_begin = value.begin();
        //auto _value_end = value.end();
        //for (auto it = _value_begin; it != _value_end; ++it) {
        //    raw[it - _value_begin] = *it;
        //}
    }

    uint16_t inet6_family_view::get_port() const {
        return ::ntohs(get_detail()->sa_port);
    }

    void inet6_family_view::set_port(const uint16_t value) {
        get_detail()->sa_port = ::htons(value);
    }

    zt_family_view::zt_family_view(sockaddr_type* const sap) : family_view(sap) {
    }

    zt_family_view::~zt_family_view() {
    }

    zt_family_view::detail_type* zt_family_view::get_detail() const {
        return &_sap->s_un.s_zt;
    }

    bool zt_family_view::operator==(const detail_type& other) {
        const auto detailp = get_detail();
        return detailp->sa_nodeid == other.sa_nodeid
            && detailp->sa_nwid == other.sa_nwid
            && detailp->sa_port == other.sa_port;
    }

    bool zt_family_view::operator!=(const detail_type& other) {
        return !operator==(other);
    }

    bool zt_family_view::operator==(const family_view_base& other) {

        if (!family_view_base::operator==(other)) { return false; }

        // We are expecting Views to be Truly Aligned.
        auto op = dynamic_cast<zt_family_view*>(
            const_cast<family_view_base*>(&other)
            );

        if (!op) { return false; }

        return operator==(*op->get_detail());
    }

    bool zt_family_view::operator!=(const family_view_base& other) {
        return !operator==(other);
    }

    sockaddr_family_type zt_family_view::get_family() const {
        return static_cast<sockaddr_family_type>(get_detail()->sa_family);
    }

    address::address() : _sa(), _view() {
        // TODO: TBD: this is probably as safe an assumption as possible...
        std::memset(&_sa, 0, sizeof(_sa));
    }

    address::address(const address& other) : _sa(), _view() {
        std::memcpy(&_sa, &other._sa, sizeof(_sa));
    }

    address::~address() {
    }

    std::string address::name_of(uint16_t value) {
        switch (value) {
        case af_inproc: return sockaddr_family_name<af_inproc>::name;
        case af_ipc: return sockaddr_family_name<af_ipc>::name;
        case af_inet: return sockaddr_family_name<af_inet>::name;
        case af_inet6: return sockaddr_family_name<af_inet6>::name;
        case af_zt: return sockaddr_family_name<af_zt>::name;
        case af_unspec: return sockaddr_family_name<af_unspec>::name;
        }

        std::ostringstream os;
        os << "unknown family type (" << value << ")";
        return os.str();
    }

    void align_view(const address& _Address) {

        // Do the const cast dance to thread the needle.
        auto& a = const_cast<address&>(_Address);

        auto& _sa = a._sa;
        auto& _view = a._view;

        // Should be safe to do this regardless of the state of the union.
        const auto actual_jewel = _sa.s_un.s_family;

        if (_view != nullptr && _view->get_jewel() == actual_jewel) {
            return;
        }

        // For use regardless of the intended view.
        const auto sap = a.get();

        // Trusting that Jewel is not somehow otherwise accidentally correct!
        switch (actual_jewel) {
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
        case af_unspec:
        default:
            _view.reset(new unspec_family_view(sap));
            break;
        }
    }

    family_view_base* const address::view() const {
        align_view(*this);
        return _view.get();
    }

    bool address::has_one() const {
        return view() != nullptr;
    }

    size_type address::get_size() const {
        return sizeof(_sa);
    }

    address::sockaddr_type* address::get() {
        return &_sa;
    }

    sockaddr_family_type address::get_family() const {
        return static_cast<sockaddr_family_type>(_sa.s_un.s_family);
    }

    void address::set_family(const sockaddr_family_type value) {
        _sa.s_un.s_family = value;
    }

    bool address::operator==(const address& other) {

        // Return early when the Families are different.
        if (get_family() != other.get_family()) { return false; }

        // We must cast the constness out of the other for this operation.
        auto otherp = const_cast<address*>(&other);

        // We must take this Long Way Round on account of NNG treating the misaligned data as indeterminate.
        return view()->operator==(*(otherp->view()));
    }

    bool address::operator!=(const address& other) {
        return !operator==(other);
    }

    address address::in_loopback() {
        address addr;
        addr.set_family(af_inet);
        auto vp = addr.view();
        vp->set_addr(INADDR_LOOPBACK);
        return addr;
    }

    address address::in6_loopback() {
        address addr;
        addr.set_family(af_inet6);
        auto vp = addr.view();
        // IPv6 loopback is quite simple: 0000:0000:0000:0000:0000:0000:0000:0001, or simply ::1.
        family_view_base::in6_addr_vector_type value = { 1 };
        for (family_view_base::in6_addr_vector_type::const_iterator it = value.cbegin(); it != value.cend(); ++it) {
            const auto delta = it - value.cbegin();
            const auto i = sizeof(addr._sa.s_un.s_in6.sa_addr) - delta - 1;
            addr._sa.s_un.s_in6.sa_addr[i] = *it;
        }
        return addr;
    }
}
