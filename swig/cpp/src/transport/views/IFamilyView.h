#ifndef NNGCPP_TRANSPORT_FAMILY_VIEW_BASE_H
#define NNGCPP_TRANSPORT_FAMILY_VIEW_BASE_H

#include "../sockaddr.h"

#include "../../core/having_one.hpp"
#include "../../core/equatable.hpp"

#include <cstddef>
#include <vector>

namespace nng {

    /* TODO: TBD: starting with 8-bit comprehension; but really needs to include 32-bit,
    and probably 16-bit comprehension is preferred, at least according to the specs.
    Possibly even extending into 64-bit support, 2 of those. */
    typedef std::vector<nng::uint8_t> IPv6AddrVector;
    typedef std::vector<nng::uint16_t> IPv6AddrUInt16Vector;
    typedef std::vector<nng::uint32_t> IPv6AddrUInt32Vector;

    /* Unfortunately, we do not seem able to rename the enum items, so we are forced to
    declare them inline during the SWIG mapping. See Github issue for further details.
    https://github.com/swig/swig/issues/1138 */

    enum SocketAddressFamily : nng::uint16_t {
        af_unspec = 0, // ::NNG_AF_UNSPEC,
        af_inproc, // ::NNG_AF_INPROC,
        af_ipc, // ::NNG_AF_IPC,
        af_inet, // ::NNG_AF_INET,
        af_inet6, // ::NNG_AF_INET6,
        af_zt, // ::NNG_AF_ZT (ZeroTier)
    };

    struct IAddrFamilyViewBase
        : public IHaveOne
        , public IEquatable<IAddrFamilyViewBase>
        , public std::equal_to<IAddrFamilyViewBase> {

        virtual ~IAddrFamilyViewBase();

        virtual uint16_t GetJewel() const;
        virtual uint16_t GetFamily() const;

        virtual bool HasOne() const override;

    protected:

        typedef sockaddr_t sockaddr_type;

        sockaddr_type* const _sap;

        IAddrFamilyViewBase(sockaddr_type* const sap, const SocketAddressFamily jewel);

    private:

        const SocketAddressFamily _jewel;

    public:

        virtual bool Equals(const IAddrFamilyViewBase& other) const override;

        virtual bool operator==(const IAddrFamilyViewBase& other);
        virtual bool operator!=(const IAddrFamilyViewBase& other);

        virtual nng::uint32_t GetIPv4Addr() const;

        virtual void SetIPv4Addr(const nng::uint32_t value);

        /* TODO: re-consider how the consumer gets/sets these, for instance, using something like inet_pton and/or inet_ntop
        https://msdn.microsoft.com/en-us/library/windows/desktop/cc805844.aspx / http://man7.org/linux/man-pages/man3/inet_pton.3.html */
        virtual IPv6AddrVector GetIPv6Addr() const;
        virtual IPv6AddrUInt16Vector GetIPv6Addr16() const;
        virtual IPv6AddrUInt32Vector GetIPv6Addr32() const;

        virtual void SetIPv6Addr(IPv6AddrVector value);
        virtual void SetIPv6Addr16(IPv6AddrUInt16Vector value);
        virtual void SetIPv6Addr32(IPv6AddrUInt32Vector value);

        /* Had to rename these to funkier names in order to avoid conflicts
        with Internet related definitions, like "SetPort". Go figure... */
        virtual nng::uint16_t __GetPort() const;
        virtual void __SetPort(const nng::uint16_t value);
    };

    template<class Detail_>
    struct IAddrFamilyView
        : public IAddrFamilyViewBase
        , public std::equal_to<Detail_> {

        typedef Detail_ detail_type;

        typedef IAddrFamilyView<Detail_> family_view_type;

        typedef SocketAddressFamily family_type;

        virtual ~IAddrFamilyView() {}

    protected:

        IAddrFamilyView(sockaddr_type* const sap, const SocketAddressFamily jewel)
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
