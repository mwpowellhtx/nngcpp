#ifndef NNGCPP_TRANSPORT_VIEWS_INET6_FAMILY_VIEW_H
#define NNGCPP_TRANSPORT_VIEWS_INET6_FAMILY_VIEW_H

#include "IFamilyView.h"

namespace nng {

    struct _Inet6FamilyView : public IAddrFamilyView<sockaddr_in6_t> {

        virtual ~_Inet6FamilyView();

        virtual bool operator==(const IAddrFamilyViewBase& other) override;
        virtual bool operator!=(const IAddrFamilyViewBase& other) override;

    protected:

        friend void align_view(const _SockAddr&);

        _Inet6FamilyView(sockaddr_type* const sap);

        virtual detail_type* get_detail() const override;

        virtual bool operator==(const detail_type& other) override;
        virtual bool operator!=(const detail_type& other) override;

    public:

        virtual IPv6AddrVector GetIPv6Addr() const override;
        virtual IPv6AddrUInt16Vector GetIPv6Addr16() const override;
        virtual IPv6AddrUInt32Vector GetIPv6Addr32() const override;

        virtual void SetIPv6Addr(IPv6AddrVector value) override;
        virtual void SetIPv6Addr16(IPv6AddrUInt16Vector value) override;
        virtual void SetIPv6Addr32(IPv6AddrUInt32Vector value) override;

        virtual nng::uint16_t __GetPort() const override;
        virtual void __SetPort(const nng::uint16_t value) override;
    };
}

#endif // NNGCPP_TRANSPORT_VIEWS_INET6_FAMILY_VIEW_H
