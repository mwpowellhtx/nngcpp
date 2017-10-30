namespace Nng.Sharp {
    public partial interface IIPv6FamilyView {

        IPv6AddrByteVector GetIPv6Addr();

        IPv6AddrUShortVector GetIPv6Addr16();

        IPv6AddrUIntVector GetIPv6Addr32();

        void SetIPv6Addr(IPv6AddrByteVector value);

        void SetIPv6Addr16(IPv6AddrUShortVector value);

        void SetIPv6Addr32(IPv6AddrUIntVector value);
    }
}
