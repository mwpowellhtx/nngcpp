namespace Nng.Sharp {
    public partial interface ICanGet
    {
        bool TryGet(ByteVector resultp);

        ByteVector Get();

        ulong GetSize();
    }
}
