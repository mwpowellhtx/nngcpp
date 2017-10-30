namespace Nng.Sharp {
    public partial interface ICanTrimLeft
    {
        void TrimLeft(ulong count);

        void TrimLeft(out uint result);
    }

    public partial interface ICanTrimRight
    {
        void TrimRight(ulong count);

        void TrimRight(out uint result);
    }
}
