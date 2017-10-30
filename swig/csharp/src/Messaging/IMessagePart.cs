namespace Nng.Sharp {
    public partial interface IMessagePart : IHaveOne, IClearable, ICanGet
        , ICanAppend<ByteVector>, ICanAppend<string>, ICanAppend<uint>
        , ICanPrepend<ByteVector>, ICanPrepend<string>, ICanPrepend<uint>
        , ICanTrimLeft, ICanTrimRight
    {
    }
}
