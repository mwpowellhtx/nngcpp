#ifndef NNGCPP_TRANSPORT_FAMILY_VIEW_BASE_H
#define NNGCPP_TRANSPORT_FAMILY_VIEW_BASE_H

#include "../sockaddr.h"

#include "../../core/having_one.hpp"

#include <cstddef>
#include <vector>

namespace nng {

    enum _SockAddrFamilyType : nng::uint16_t {
        af_unspec = ::NNG_AF_UNSPEC,
        af_inproc = ::NNG_AF_INPROC,
        af_ipc = ::NNG_AF_IPC,
        af_inet = ::NNG_AF_INET,
        af_inet6 = ::NNG_AF_INET6,
        af_zt = ::NNG_AF_ZT, // ZeroTier
    };

    struct IAddrFamilyViewBase
        : public IHaveOne
        , public std::equal_to<IAddrFamilyViewBase> {

        virtual ~IAddrFamilyViewBase();

        virtual _SockAddrFamilyType get_jewel() const;

        virtual _SockAddrFamilyType get_family() const = 0;

        virtual bool HasOne() const override;

    protected:

        //typedef ::nng_sockaddr sockaddr_type;
        typedef sockaddr_t sockaddr_type;

        sockaddr_type* const _sap;

        IAddrFamilyViewBase(sockaddr_type* const sap, const _SockAddrFamilyType jewel);

    private:

        const _SockAddrFamilyType _jewel;

    public:

        // TODO: TBD: starting with 8-bit comprehension; but really needs to include 32-bit, and probably 16-bit comprehension is preferred, at least according to the specs.
        typedef std::vector<nng::uint8_t> in6_addr_vector_type;
        typedef std::vector<nng::uint16_t> in6_addr16_vector_type;
        typedef std::vector<nng::uint32_t> in6_addr32_vector_type;

        virtual bool operator==(const IAddrFamilyViewBase& other);
        virtual bool operator!=(const IAddrFamilyViewBase& other);

        virtual nng::uint32_t get_addr() const;

        virtual void set_addr(const nng::uint32_t value);

        /* TODO: re-consider how the consumer gets/sets these, for instance, using something like inet_pton and/or inet_ntop
        https://msdn.microsoft.com/en-us/library/windows/desktop/cc805844.aspx / http://man7.org/linux/man-pages/man3/inet_pton.3.html */
        virtual in6_addr_vector_type get_in6_addr() const;
        virtual in6_addr16_vector_type get_in6_addr16() const;
        virtual in6_addr32_vector_type get_in6_addr32() const;

        virtual void set_in6_addr(in6_addr_vector_type value);
        virtual void set_in6_addr16(in6_addr16_vector_type value);
        virtual void set_in6_addr32(in6_addr32_vector_type value);

        virtual nng::uint16_t get_port() const;

        virtual void set_port(const nng::uint16_t value);
    };

    template<class Detail_>
    struct IAddrFamilyView
        : public IAddrFamilyViewBase
        , public std::equal_to<Detail_> {

        typedef Detail_ detail_type;

        typedef IAddrFamilyView<Detail_> family_view_type;

        typedef _SockAddrFamilyType family_type;

        virtual ~IAddrFamilyView() {}

    protected:

        IAddrFamilyView(sockaddr_type* const sap, const _SockAddrFamilyType jewel)
            : IAddrFamilyViewBase(sap, jewel) {
        }

        virtual detail_type* get_detail() const = 0;

        virtual bool operator==(const detail_type& other) = 0;
        virtual bool operator!=(const detail_type& other) = 0;
    };

#ifndef NNGCPP_ADDRESS_H
    class _SockAddr;
#endif // NNGCPP_ADDRESS_H

}

#endif // NNGCPP_TRANSPORT_FAMILY_VIEW_BASE_H
