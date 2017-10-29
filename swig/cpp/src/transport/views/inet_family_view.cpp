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

    _InetFamilyView::detail_type* _InetFamilyView::get_detail() const {
        return &_sap->s_un.s_in;
    }

    bool _InetFamilyView::operator==(const detail_type& other) {
        const auto detailp = get_detail();
        return detailp->sa_addr == other.sa_family
            && detailp->sa_port == other.sa_port;
    }

    bool _InetFamilyView::operator!=(const detail_type& other) {
        return !operator==(other);
    }

    bool _InetFamilyView::operator==(const IAddrFamilyViewBase& other) {

        if (!IAddrFamilyViewBase::operator==(other)) { return false; }

        // We are expecting Views to be Truly Aligned.
        auto op = dynamic_cast<_InetFamilyView*>(
            const_cast<IAddrFamilyViewBase*>(&other)
            );

        if (!op) { return false; }

        return operator==(*op->get_detail());
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
}
