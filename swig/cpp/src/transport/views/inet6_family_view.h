#ifndef NNGCPP_TRANSPORT_VIEWS_INET6_FAMILY_VIEW_H
#define NNGCPP_TRANSPORT_VIEWS_INET6_FAMILY_VIEW_H

#include "IFamilyView.h"

namespace nng {

    struct _Inet6FamilyView : public IAddrFamilyView<sockaddr_in6_t> {

        virtual _SockAddrFamilyType get_family() const override;

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

        virtual in6_addr_vector_type get_in6_addr() const override;
        virtual in6_addr16_vector_type get_in6_addr16() const override;
        virtual in6_addr32_vector_type get_in6_addr32() const override;

        virtual void set_in6_addr(in6_addr_vector_type value) override;
        virtual void set_in6_addr16(in6_addr16_vector_type value) override;
        virtual void set_in6_addr32(in6_addr32_vector_type value) override;

        virtual nng::uint16_t get_port() const override;
        virtual void set_port(const nng::uint16_t value) override;
    };
}

#endif // NNGCPP_TRANSPORT_VIEWS_INET6_FAMILY_VIEW_H
