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
    struct in6_addr_expected_size<IAddrFamilyViewBase::in6_addr_vector_type>
        : std::integral_constant<std::size_t, 16> {
    };

    template<>
    struct in6_addr_expected_size<IAddrFamilyViewBase::in6_addr16_vector_type>
        : std::integral_constant<std::size_t, 8> {
    };

    template<>
    struct in6_addr_expected_size<IAddrFamilyViewBase::in6_addr32_vector_type>
        : std::integral_constant<std::size_t, 4> {
    };

    _Inet6FamilyView::_Inet6FamilyView(sockaddr_type* const sap)
        : IAddrFamilyView(sap, af_inet6) {
    }

    _Inet6FamilyView::~_Inet6FamilyView() {
    }

    _SockAddrFamilyType _Inet6FamilyView::get_family() const {
        return static_cast<_SockAddrFamilyType>(get_detail()->sa_family);
    }

    _Inet6FamilyView::detail_type* _Inet6FamilyView::get_detail() const {
        return &_sap->s_un.s_in6;
    }

    bool _Inet6FamilyView::operator==(const detail_type& other) {
        const auto detailp = get_detail();
        return std::memcmp(&detailp->sa_addr, &other.sa_addr, 16) == 0
            && detailp->sa_port == other.sa_port;
    }

    bool _Inet6FamilyView::operator!=(const detail_type& other) {
        return !operator==(other);
    }

    bool _Inet6FamilyView::operator==(const IAddrFamilyViewBase& other) {

        if (!IAddrFamilyViewBase::operator==(other)) { return false; }

        // We are expecting Views to be Truly Aligned.
        auto op = dynamic_cast<_Inet6FamilyView*>(
            const_cast<IAddrFamilyViewBase*>(&other)
            );

        if (!op) { return false; }

        return operator==(*op->get_detail());
    }

    bool _Inet6FamilyView::operator!=(const IAddrFamilyViewBase& other) {
        return !operator==(other);
    }

    _Inet6FamilyView::in6_addr_vector_type _Inet6FamilyView::get_in6_addr() const {
        return IAddrFamilyViewBase::get_in6_addr();
    }

    _Inet6FamilyView::in6_addr16_vector_type _Inet6FamilyView::get_in6_addr16() const {
        return IAddrFamilyViewBase::get_in6_addr16();
    }

    _Inet6FamilyView::in6_addr32_vector_type _Inet6FamilyView::get_in6_addr32() const {
        return IAddrFamilyViewBase::get_in6_addr32();
    }

    void _Inet6FamilyView::set_in6_addr(in6_addr_vector_type value) {
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

    void _Inet6FamilyView::set_in6_addr16(in6_addr16_vector_type value) {
        IAddrFamilyViewBase::set_in6_addr16(value);
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

    void _Inet6FamilyView::set_in6_addr32(in6_addr32_vector_type value) {
        IAddrFamilyViewBase::set_in6_addr32(value);
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

    uint16_t _Inet6FamilyView::get_port() const {
        return ::ntohs(get_detail()->sa_port);
    }

    void _Inet6FamilyView::set_port(const uint16_t value) {
        get_detail()->sa_port = ::htons(value);
    }
}
