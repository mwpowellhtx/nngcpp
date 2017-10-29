#include "IFamilyView.h"
#include "../../core/exceptions.hpp"

#define THROW_VIEW_NOT_IMPLEMENTED_EX(name) \
    throw nng::exceptions::not_implemented(#name \
        " view is not implemented for the address family");

namespace nng {

    IAddrFamilyViewBase::IAddrFamilyViewBase(sockaddr_type* const sap, SocketAddressFamily jewel)
        : IHaveOne(), IEquatable(), equal_to()
        , _sap(sap), _jewel(jewel) {
    }

    IAddrFamilyViewBase::~IAddrFamilyViewBase() {
    }

    uint16_t IAddrFamilyViewBase::GetJewel() const {
        /* Family related details are dealt with in raw types instead of enumerated types.
        This helps the mapping during SWIG processing flow more naturally, especially without
        having to deal with as many proxied wrappers. */
        return _jewel;
    }

    uint16_t IAddrFamilyViewBase::GetFamily() const {
        /* We can do it this way because the field is Unionized across the breadth of the
        transports. Which has the added benefit of the specific views just focusing on their
        individual view responsibilities, and letting the base class sort out their identities. */
        return _sap->s_un.s_family;
    }

    bool IAddrFamilyViewBase::HasOne() const {
        // We have one when we have an SA* and our Famly Jewel is aligned.
        return _sap != nullptr && GetFamily() == GetJewel();
    }

    bool IAddrFamilyViewBase::Equals(const IAddrFamilyViewBase& other) const {
        // Smoke test both of the Views for equality.
        return GetJewel() == other.GetJewel();
    }

    bool IAddrFamilyViewBase::operator==(const IAddrFamilyViewBase& other) {
        return Equals(other);
    }

    bool IAddrFamilyViewBase::operator!=(const IAddrFamilyViewBase& other) {
        return !Equals(other);
    }

    nng::uint32_t IAddrFamilyViewBase::GetIPv4Addr() const {
        THROW_VIEW_NOT_IMPLEMENTED_EX(GetIPv4Addr);
    }

    void IAddrFamilyViewBase::SetIPv4Addr(const nng::uint32_t value) {
        THROW_VIEW_NOT_IMPLEMENTED_EX(SetIPv4Addr);
    }

    IPv6AddrVector IAddrFamilyViewBase::GetIPv6Addr() const {
        THROW_VIEW_NOT_IMPLEMENTED_EX(GetIPv6Addr);
    }

    IPv6AddrUInt16Vector IAddrFamilyViewBase::GetIPv6Addr16() const {
        THROW_VIEW_NOT_IMPLEMENTED_EX(GetIPv6Addr16);
    }

    IPv6AddrUInt32Vector IAddrFamilyViewBase::GetIPv6Addr32() const {
        THROW_VIEW_NOT_IMPLEMENTED_EX(GetIPv6Addr32);
    }

    void IAddrFamilyViewBase::SetIPv6Addr(IPv6AddrVector value) {
        THROW_VIEW_NOT_IMPLEMENTED_EX(SetIPv6Addr);
    }

    void IAddrFamilyViewBase::SetIPv6Addr16(IPv6AddrUInt16Vector value) {
        THROW_VIEW_NOT_IMPLEMENTED_EX(SetIPv6Addr16);
    }

    void IAddrFamilyViewBase::SetIPv6Addr32(IPv6AddrUInt32Vector value) {
        THROW_VIEW_NOT_IMPLEMENTED_EX(SetIPv6Addr32);
    }

    nng::uint16_t IAddrFamilyViewBase::__GetPort() const {
        THROW_VIEW_NOT_IMPLEMENTED_EX(__GetPort);
    }

    void IAddrFamilyViewBase::__SetPort(const nng::uint16_t value) {
        THROW_VIEW_NOT_IMPLEMENTED_EX(__SetPort);
    }
}
