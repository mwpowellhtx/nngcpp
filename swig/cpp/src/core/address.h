#ifndef NNGCPP_ADDRESS_H
#define NNGCPP_ADDRESS_H

#include "types.h"
#include "having_one.hpp"

#include <cstddef>
#include <memory>
#include <vector>
#include <functional>

#define SOCKADDR_FAMILY_TO_STRING(x)    #x

namespace nng {

    /* Some address details. This is in some ways like a traditional sockets sockaddr,
    but we have our own to cope with our unique families, etc. The details of this
    structure are directly exposed to applications. These structures can be obtained
    via property lookups, etc. */
    struct sockaddr_path_t {
        uint16_t    sa_family;
        char        sa_path[NNG_MAXADDRLEN];
    };
    typedef struct sockaddr_path_t sockaddr_path_t;
    typedef struct sockaddr_path_t sockaddr_ipc_t;
    typedef struct sockaddr_path_t sockaddr_inproc_t;

    // TODO: TBD: I am prepared to support this path for accessing the IPv6 address regardless what Garrett does in the NNG code...
    // TODO: TBD have a look at: https://stackoverflow.com/questions/22622013/wrap-existing-memory-with-const-stdvector#answer-22622361
    // for potential approach to exposing the value of the array in a const-vector to a reference wrapper...
    // https://stackoverflow.com/questions/6623387/fix-lock-size-of-stdvector
    // https://stackoverflow.com/questions/2434196/how-to-initialize-stdvector-from-c-style-array
    struct sockaddr_in6_t {
        uint16_t        sa_family;
        uint16_t        sa_port;
        union {
            uint8_t     sa_addr[16];
            uint16_t    sa_addr16[8];
            uint32_t    sa_addr32[4];
        };
    };
    typedef struct sockaddr_in6_t sockaddr_in6_t;
    typedef struct sockaddr_in6_t sockaddr_udp6_t;
    typedef struct sockaddr_in6_t sockaddr_tcp6_t;

    struct sockaddr_in_t {
        uint16_t    sa_family;
        uint16_t    sa_port;
        uint32_t    sa_addr;
    };

    struct sockaddr_zt_t {
        uint16_t    sa_family;
        uint64_t    sa_nwid;
        uint64_t    sa_nodeid;
        uint32_t    sa_port;
    };

    typedef struct sockaddr_in_t sockaddr_in_t;
    typedef struct sockaddr_in_t sockaddr_udp_t;
    typedef struct sockaddr_in_t sockaddr_tcp_t;
    typedef struct sockaddr_zt_t sockaddr_zt_t;

    typedef struct sockaddr_t {
        union {
            uint16_t            s_family;
            sockaddr_path_t     s_path;
            sockaddr_inproc_t   s_inproc;
            sockaddr_in6_t      s_in6;
            sockaddr_in_t       s_in;
            sockaddr_zt_t       s_zt;
        } s_un;
    } sockaddr_t;

    enum sockaddr_family_type : uint16_t {
        af_unspec = ::NNG_AF_UNSPEC,
        af_inproc = ::NNG_AF_INPROC,
        af_ipc = ::NNG_AF_IPC,
        af_inet = ::NNG_AF_INET,
        af_inet6 = ::NNG_AF_INET6,
        af_zt = ::NNG_AF_ZT,  // ZeroTier
    };

    template<sockaddr_family_type Val_>
    struct sockaddr_family_name
        : std::integral_constant<sockaddr_family_type, Val_> {
        static const std::string name;
    };

    template<>
    struct sockaddr_family_name<af_unspec>
        : std::integral_constant<sockaddr_family_type, af_unspec> {
        static const std::string name;
    };

    template<>
    struct sockaddr_family_name<af_inproc>
        : std::integral_constant<sockaddr_family_type, af_inproc> {
        static const std::string name;
    };

    template<>
    struct sockaddr_family_name<af_ipc>
        : std::integral_constant<sockaddr_family_type, af_ipc> {
        static const std::string name;
    };

    template<>
    struct sockaddr_family_name<af_inet>
        : std::integral_constant<sockaddr_family_type, af_inet> {
        static const std::string name;
    };

    template<>
    struct sockaddr_family_name<af_inet6>
        : std::integral_constant<sockaddr_family_type, af_inet6> {
        static const std::string name;
    };

    template<>
    struct sockaddr_family_name<af_zt>
        : std::integral_constant<sockaddr_family_type, af_zt> {
        static const std::string name;
    };

    //typedef ::nng_sockaddr_inproc sockaddr_inproc_view;
    typedef sockaddr_inproc_t sockaddr_inproc_view;
    //typedef ::nng_sockaddr_ipc sockaddr_ipc_view;
    typedef sockaddr_ipc_t sockaddr_ipc_view;
    //typedef ::nng_sockaddr_in sockaddr_in_view;
    typedef sockaddr_in_t sockaddr_in_view;
    //typedef ::nng_sockaddr_in6 sockaddr_in6_view;
    typedef sockaddr_in6_t sockaddr_in6_view;
    //typedef ::nng_sockaddr_zt sockaddr_zt_view;
    typedef sockaddr_zt_t sockaddr_zt_view;

    struct family_view_base : public having_one, public std::equal_to<family_view_base> {

        virtual ~family_view_base();

        virtual sockaddr_family_type get_jewel() const;

        virtual sockaddr_family_type get_family() const = 0;

        virtual bool has_one() const override;

    protected:

        //typedef ::nng_sockaddr sockaddr_type;
        typedef sockaddr_t sockaddr_type;

        sockaddr_type* const _sap;

        family_view_base(sockaddr_type* const sap, sockaddr_family_type jewel);

    private:

        const sockaddr_family_type _jewel;

    public:

        // TODO: TBD: starting with 8-bit comprehension; but really needs to include 32-bit, and probably 16-bit comprehension is preferred, at least according to the specs.
        typedef std::vector<uint8_t> in6_addr_vector_type;
        typedef std::vector<uint16_t> in6_addr16_vector_type;
        typedef std::vector<uint32_t> in6_addr32_vector_type;

        virtual bool operator==(const family_view_base& other);
        virtual bool operator!=(const family_view_base& other);

        virtual uint32_t get_addr() const;

        virtual void set_addr(const uint32_t value);

        /* TODO: re-consider how the consumer gets/sets these, for instance, using something like inet_pton and/or inet_ntop
        https://msdn.microsoft.com/en-us/library/windows/desktop/cc805844.aspx / http://man7.org/linux/man-pages/man3/inet_pton.3.html */
        virtual in6_addr_vector_type get_in6_addr() const;
        virtual in6_addr16_vector_type get_in6_addr16() const;
        virtual in6_addr32_vector_type get_in6_addr32() const;

        virtual void set_in6_addr(in6_addr_vector_type value);
        virtual void set_in6_addr16(in6_addr16_vector_type value);
        virtual void set_in6_addr32(in6_addr32_vector_type value);

        virtual uint16_t get_port() const;

        virtual void set_port(const uint16_t value);
    };

    template<sockaddr_family_type Type_, class Detail_>
    struct family_view : public family_view_base {

        virtual ~family_view();

        typedef sockaddr_family_type family_type;

        typedef family_view<Type_, Detail_> family_view_type;

        typedef Detail_ detail_type;

    protected:

        virtual bool operator==(const detail_type& other) = 0;
        virtual bool operator!=(const detail_type& other) = 0;

        family_view(sockaddr_type* const sap);

        virtual detail_type* get_detail() const = 0;
    };

    struct unspec_family_view : public family_view_base {

        virtual sockaddr_family_type get_family() const override;

        unspec_family_view(sockaddr_type* const sap);

        virtual ~unspec_family_view();

        virtual bool has_one() const override;
    };

    struct inproc_family_view : public family_view<af_inproc, sockaddr_inproc_t> {

        virtual sockaddr_family_type get_family() const override;

        inproc_family_view(sockaddr_type* const sap);

        virtual ~inproc_family_view();

        virtual bool operator==(const family_view_base& other) override;
        virtual bool operator!=(const family_view_base& other) override;

    protected:

        virtual detail_type* get_detail() const override;

        virtual bool operator==(const detail_type& other) override;
        virtual bool operator!=(const detail_type& other) override;
    };

    struct ipc_family_view : public family_view<af_ipc, sockaddr_ipc_t> {

        virtual sockaddr_family_type get_family() const override;

        ipc_family_view(sockaddr_type* const sap);

        virtual ~ipc_family_view();

        virtual bool operator==(const family_view_base& other) override;
        virtual bool operator!=(const family_view_base& other) override;

    protected:

        virtual detail_type* get_detail() const override;

        virtual bool operator==(const detail_type& other) override;
        virtual bool operator!=(const detail_type& other) override;
    };

    struct inet_family_view : public family_view<af_inet, sockaddr_in_t> {

        virtual sockaddr_family_type get_family() const override;

        inet_family_view(sockaddr_type* const sap);

        virtual ~inet_family_view();

        virtual bool operator==(const family_view_base& other) override;
        virtual bool operator!=(const family_view_base& other) override;

    protected:

        virtual detail_type* get_detail() const override;

        virtual bool operator==(const detail_type& other) override;
        virtual bool operator!=(const detail_type& other) override;

    public:

        virtual uint32_t get_addr() const override;
        virtual void set_addr(const uint32_t value) override;

        virtual uint16_t get_port() const override;
        virtual void set_port(const uint16_t value) override;
    };

    struct inet6_family_view : public family_view<af_inet6, sockaddr_in6_t> {

        virtual sockaddr_family_type get_family() const override;

        inet6_family_view(sockaddr_type* const sap);

        virtual ~inet6_family_view();

        virtual bool operator==(const family_view_base& other) override;
        virtual bool operator!=(const family_view_base& other) override;

    protected:

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

        virtual uint16_t get_port() const override;
        virtual void set_port(const uint16_t value) override;
    };

    struct zt_family_view : public family_view<af_zt, sockaddr_zt_t> {

        virtual sockaddr_family_type get_family() const override;

        zt_family_view(sockaddr_type* const sap);

        virtual ~zt_family_view();

        virtual bool operator==(const family_view_base& other) override;
        virtual bool operator!=(const family_view_base& other) override;

    protected:

        virtual detail_type* get_detail() const override;

        virtual bool operator==(const detail_type& other) override;
        virtual bool operator!=(const detail_type& other) override;
    };

    class address : public having_one, public std::equal_to<address> {
    public:

        //typedef ::nng_sockaddr sockaddr_type;
        typedef sockaddr_t sockaddr_type;

    private:

        sockaddr_type _sa;

        std::unique_ptr<family_view_base> _view;

        friend void align_view(const address& _Address);

    public:

        address();

        address(const address& other);

        virtual ~address();

        size_type get_size() const;

        sockaddr_type* get();

        sockaddr_family_type get_family() const;

        void set_family(const sockaddr_family_type value);

        family_view_base* const view() const;

        virtual bool has_one() const override;

        bool operator==(const address& other);

        bool operator!=(const address& other);

        static address in_loopback();

        static address in6_loopback();

        static std::string name_of(uint16_t value);
    };
}

#endif // NNGCPP_ADDRESS_H
