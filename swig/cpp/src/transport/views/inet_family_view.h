#ifndef NNGCPP_TRANSPORT_VIEWS_INET_FAMILY_VIEW_H
#define NNGCPP_TRANSPORT_VIEWS_INET_FAMILY_VIEW_H

#include "IFamilyView.h"

namespace nng {

    struct _InetFamilyView : public IAddrFamilyView<sockaddr_in_t> {

        virtual ~_InetFamilyView();

        virtual bool operator==(const IAddrFamilyViewBase& other) override;
        virtual bool operator!=(const IAddrFamilyViewBase& other) override;

    protected:

        friend void align_view(const _SockAddr&);

        _InetFamilyView(sockaddr_type* const sap);

        virtual detail_type* get_detail() const override;

        virtual bool operator==(const detail_type& other) override;
        virtual bool operator!=(const detail_type& other) override;

    public:

        virtual nng::uint32_t GetIPv4Addr() const override;
        virtual void SetIPv4Addr(const nng::uint32_t value) override;

        virtual nng::uint16_t __GetPort() const override;
        virtual void __SetPort(const nng::uint16_t value) override;
    };
}

#endif // NNGCPP_TRANSPORT_VIEWS_INET_FAMILY_VIEW_H
