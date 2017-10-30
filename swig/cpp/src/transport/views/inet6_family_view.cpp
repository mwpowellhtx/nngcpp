#include "inet6_family_view.h"

#ifdef _WIN32
#   include <WinSock2.h>
#else
#   include <arpa/inet.h>
#endif

namespace nng {

    // TODO: TBD: decide whether this should go in the header; would allow for testing/vetting of the expected sizes themselves.
    // TODO: TBD: otherwise would need to support testing indirectly via the setters...
    template<typename Vector_>
    struct in6_addr_expected_size
        : std::integral_constant<std::size_t, 0> {
    };

    template<>
    struct in6_addr_expected_size<IPv6AddrVector>
        : std::integral_constant<std::size_t, 16> {
    };

    template<>
    struct in6_addr_expected_size<IPv6AddrUInt16Vector>
        : std::integral_constant<std::size_t, 8> {
    };

    template<>
    struct in6_addr_expected_size<IPv6AddrUInt32Vector>
        : std::integral_constant<std::size_t, 4> {
    };

    _Inet6FamilyView::_Inet6FamilyView(sockaddr_type* const sap)
        : IAddrFamilyView(sap, af_inet6) {
    }

    _Inet6FamilyView::~_Inet6FamilyView() {
    }

    bool _Inet6FamilyView::HasOne() const {
        return __family_view_base::HasOne();
    }

    _Inet6FamilyView::detail_type* _Inet6FamilyView::get_detail() const {
        return &_sap->s_un.s_in6;
    }

    bool _Inet6FamilyView::Equals(detail_type* const ap, detail_type* const bp) const {
        return ap == bp || (
            std::memcmp(&ap->sa_addr, &bp->sa_addr, 16) == 0
            && ap->sa_port == bp->sa_port
            );
    }

    bool _Inet6FamilyView::Equals(const IAddrFamilyViewBase& other) const {
        if (!__family_view_base::Equals(other)) { return false; }

        // We are expecting Views to be Truly Aligned.
        auto op = dynamic_cast<_Inet6FamilyView*>(
            const_cast<IAddrFamilyViewBase*>(&other)
            );

        if (!op) { return false; }

        return Equals(get_detail(), op->get_detail());
    }

    bool _Inet6FamilyView::operator==(const IAddrFamilyViewBase& other) {
        return Equals(other);
    }

    bool _Inet6FamilyView::operator!=(const IAddrFamilyViewBase& other) {
        return !Equals(other);
    }

    IPv6AddrVector _Inet6FamilyView::GetIPv6Addr() const {
        return IAddrFamilyViewBase::GetIPv6Addr();
    }

    IPv6AddrUInt16Vector _Inet6FamilyView::GetIPv6Addr16() const {
        return IAddrFamilyViewBase::GetIPv6Addr16();
    }

    IPv6AddrUInt32Vector _Inet6FamilyView::GetIPv6Addr32() const {
        return IAddrFamilyViewBase::GetIPv6Addr32();
    }

    void _Inet6FamilyView::SetIPv6Addr(IPv6AddrVector value) {
        if (value.size() <= in6_addr_expected_size<IPv6AddrVector>::value) {
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

    void _Inet6FamilyView::SetIPv6Addr16(IPv6AddrUInt16Vector value) {
        IAddrFamilyViewBase::SetIPv6Addr16(value);
        // TODO: TBD: ditto address view NNG struct versus wrapper work...
        if (value.size() <= in6_addr_expected_size<IPv6AddrUInt16Vector>::value) {
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

    void _Inet6FamilyView::SetIPv6Addr32(IPv6AddrUInt32Vector value) {
        IAddrFamilyViewBase::SetIPv6Addr32(value);
        // TODO: TBD: ditto address view NNG struct versus wrapper work...
        if (value.size() <= in6_addr_expected_size<IPv6AddrUInt32Vector>::value) {
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

    uint16_t _Inet6FamilyView::__GetPort() const {
        return ::ntohs(get_detail()->sa_port);
    }

    void _Inet6FamilyView::__SetPort(const uint16_t value) {
        get_detail()->sa_port = ::htons(value);
    }

    nng::uint32_t _Inet6FamilyView::GetIPv4Addr() const {
        return __family_view_base::GetIPv4Addr();
    }

    void _Inet6FamilyView::SetIPv4Addr(const nng::uint32_t value) {
        __family_view_base::SetIPv4Addr(value);
    }
}
