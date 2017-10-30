namespace Nng.Sharp {
    public partial interface ISocketAddress {

        ushort Family { get; set; }

        IAddressFamilyView View { get; }

        ulong GetSize();
    }
}
