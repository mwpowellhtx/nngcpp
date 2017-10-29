#include "IFamilyView.h"
#include "../../core/exceptions.hpp"

#define THROW_VIEW_NOT_IMPLEMENTED_EX(name) \
    throw nng::exceptions::not_implemented(#name \
        " view is not implemented for the address family");

namespace nng {

    IAddrFamilyViewBase::IAddrFamilyViewBase(sockaddr_type* const sap, _SockAddrFamilyType jewel)
        : IHaveOne(), equal_to()
        , _sap(sap), _jewel(jewel) {
    }

    IAddrFamilyViewBase::~IAddrFamilyViewBase() {
    }

    _SockAddrFamilyType IAddrFamilyViewBase::get_jewel() const {
        return _jewel;
    }

    bool IAddrFamilyViewBase::HasOne() const {
        return _sap != nullptr && _sap->s_un.s_family == get_jewel();
    }

    bool IAddrFamilyViewBase::operator==(const IAddrFamilyViewBase& other) {
        // Smoke test both of the Views for equality.
        return get_jewel() == other.get_jewel();
    }

    bool IAddrFamilyViewBase::operator!=(const IAddrFamilyViewBase& other) {
        return !operator==(other);
    }

    nng::uint32_t IAddrFamilyViewBase::get_addr() const {
        THROW_VIEW_NOT_IMPLEMENTED_EX(get_addr);
    }

    void IAddrFamilyViewBase::set_addr(const nng::uint32_t value) {
        THROW_VIEW_NOT_IMPLEMENTED_EX(set_addr);
    }

    IAddrFamilyViewBase::in6_addr_vector_type IAddrFamilyViewBase::get_in6_addr() const {
        THROW_VIEW_NOT_IMPLEMENTED_EX(get_in6_addr);
    }

    IAddrFamilyViewBase::in6_addr16_vector_type IAddrFamilyViewBase::get_in6_addr16() const {
        THROW_VIEW_NOT_IMPLEMENTED_EX(get_in6_addr16);
    }

    IAddrFamilyViewBase::in6_addr32_vector_type IAddrFamilyViewBase::get_in6_addr32() const {
        THROW_VIEW_NOT_IMPLEMENTED_EX(get_in6_addr32);
    }

    void IAddrFamilyViewBase::set_in6_addr(in6_addr_vector_type value) {
        THROW_VIEW_NOT_IMPLEMENTED_EX(set_in6_addr);
    }

    void IAddrFamilyViewBase::set_in6_addr16(in6_addr16_vector_type value) {
        THROW_VIEW_NOT_IMPLEMENTED_EX(set_in6_addr16);
    }

    void IAddrFamilyViewBase::set_in6_addr32(in6_addr32_vector_type value) {
        THROW_VIEW_NOT_IMPLEMENTED_EX(set_in6_addr32);
    }

    nng::uint16_t IAddrFamilyViewBase::get_port() const {
        THROW_VIEW_NOT_IMPLEMENTED_EX(get_port);
    }

    void IAddrFamilyViewBase::set_port(const nng::uint16_t value) {
        THROW_VIEW_NOT_IMPLEMENTED_EX(set_port);
    }
}
