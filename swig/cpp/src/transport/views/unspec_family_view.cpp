#include "unspec_family_view.h"

namespace nng {

    _UnspecFamilyView::_UnspecFamilyView(sockaddr_type* const sap)
        : IAddrFamilyView(sap, af_unspec) {
    }

    _UnspecFamilyView::~_UnspecFamilyView() {
    }

    bool _UnspecFamilyView::HasOne() const {
        return false;
    }

    _UnspecFamilyView::detail_type* _UnspecFamilyView::get_detail() const {
        return &_sap->s_un.s_unspec;
    }

    // Fill in the gaps, even calling to base functionality, in order for SWIG to see them properly.
    bool _UnspecFamilyView::Equals(const IAddrFamilyViewBase& other) const {
        return __family_view_base::Equals(other);
    }

    bool _UnspecFamilyView::operator==(const IAddrFamilyViewBase& other) {
        return __family_view_base::operator==(other);
    }

    bool _UnspecFamilyView::operator!=(const IAddrFamilyViewBase& other) {
        return __family_view_base::operator!=(other);
    }

    nng::uint32_t _UnspecFamilyView::GetIPv4Addr() const {
        return __family_view_base::GetIPv4Addr();
    }

    void _UnspecFamilyView::SetIPv4Addr(const nng::uint32_t value) {
        __family_view_base::SetIPv4Addr(value);
    }

    IPv6AddrVector _UnspecFamilyView::GetIPv6Addr() const {
        return __family_view_base::GetIPv6Addr();
    }

    IPv6AddrUInt16Vector _UnspecFamilyView::GetIPv6Addr16() const {
        return __family_view_base::GetIPv6Addr16();
    }

    IPv6AddrUInt32Vector _UnspecFamilyView::GetIPv6Addr32() const {
        return __family_view_base::GetIPv6Addr32();
    }

    void _UnspecFamilyView::SetIPv6Addr(IPv6AddrVector value) {
        __family_view_base::SetIPv6Addr(value);
    }

    void _UnspecFamilyView::SetIPv6Addr16(IPv6AddrUInt16Vector value) {
        __family_view_base::SetIPv6Addr16(value);
    }

    void _UnspecFamilyView::SetIPv6Addr32(IPv6AddrUInt32Vector value) {
        __family_view_base::SetIPv6Addr32(value);
    }

    nng::uint16_t _UnspecFamilyView::__GetPort() const {
        return __family_view_base::__GetPort();
    }

    void _UnspecFamilyView::__SetPort(const nng::uint16_t value) {
        __family_view_base::__SetPort(value);
    }
}
