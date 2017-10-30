#include "inet_family_view.h"

#ifdef _WIN32
#   include <WinSock2.h>
#else
#   include <arpa/inet.h>
#endif

namespace nng {

    _InetFamilyView::_InetFamilyView(sockaddr_type* const sap)
        : IAddrFamilyView(sap, af_inet) {
    }

    _InetFamilyView::~_InetFamilyView() {
    }

    bool _InetFamilyView::HasOne() const {
        return __family_view_base::HasOne();
    }

    _InetFamilyView::detail_type* _InetFamilyView::get_detail() const {
        return &_sap->s_un.s_in;
    }

    bool _InetFamilyView::Equals(detail_type* const ap, detail_type* const bp) const {
        return ap == bp || (
            ap->sa_family == bp->sa_family
            && ap->sa_addr == bp->sa_addr
            && ap->sa_port == bp->sa_port
            );
    }

    bool _InetFamilyView::Equals(const IAddrFamilyViewBase& other) const {
        if (!__family_view_base::Equals(other)) { return false; }

        // We are expecting Views to be Truly Aligned.
        auto op = dynamic_cast<_InetFamilyView*>(
            const_cast<IAddrFamilyViewBase*>(&other)
            );

        if (!op) { return false; }

        return Equals(get_detail(), op->get_detail());
    }

    bool _InetFamilyView::operator==(const IAddrFamilyViewBase& other) {
        return Equals(other);
    }

    bool _InetFamilyView::operator!=(const IAddrFamilyViewBase& other) {
        return !Equals(other);
    }

    bool _InetFamilyView::operator==(const IAddrFamilyViewBase& other) {

    }

    bool _InetFamilyView::operator!=(const IAddrFamilyViewBase& other) {
        return !operator==(other);
    }

    uint32_t _InetFamilyView::GetIPv4Addr() const {
        return ::ntohl(get_detail()->sa_addr);
    }

    void _InetFamilyView::SetIPv4Addr(const uint32_t value) {
        get_detail()->sa_addr = ::htonl(value);
    }

    uint16_t _InetFamilyView::__GetPort() const {
        return ::ntohs(get_detail()->sa_port);
    }

    void _InetFamilyView::__SetPort(const uint16_t value) {
        get_detail()->sa_port = ::htons(value);
    }

    IPv6AddrVector _InetFamilyView::GetIPv6Addr() const {
        return __family_view_base::GetIPv6Addr();
    }

    IPv6AddrUInt16Vector _InetFamilyView::GetIPv6Addr16() const {
        return __family_view_base::GetIPv6Addr16();
    }

    IPv6AddrUInt32Vector _InetFamilyView::GetIPv6Addr32() const {
        return __family_view_base::GetIPv6Addr32();
    }

    void _InetFamilyView::SetIPv6Addr(IPv6AddrVector value) {
        __family_view_base::SetIPv6Addr(value);
    }

    void _InetFamilyView::SetIPv6Addr16(IPv6AddrUInt16Vector value) {
        __family_view_base::SetIPv6Addr16(value);
    }

    void _InetFamilyView::SetIPv6Addr32(IPv6AddrUInt32Vector value) {
        __family_view_base::SetIPv6Addr32(value);
    }
}
