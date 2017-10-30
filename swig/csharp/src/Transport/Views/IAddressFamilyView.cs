namespace Nng.Sharp {
    // This is not a mistake. We must roll up the views in order to properly support the SWIG mapping, awkwardly so.
    public partial interface IAddressFamilyView
        : IUnspecifiedFamilyView
            , IInProcessFamilyView
            , IInterProcessFamilyView
            , IIPv4FamilyView
            , IIPv6FamilyView
            , ICanViewPort
            , IZeroTierFamilyView {
    }
}
