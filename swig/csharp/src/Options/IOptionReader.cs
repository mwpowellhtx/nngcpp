namespace Nng.Sharp {
    using System;

    public partial interface IOptionReader : IOptions
    {
        string GetText(string name);

        string GetText(string name, ref ulong szp);

        int GetInt32(string name);

        ulong GetSize(string name);

        int GetMilliseconds(string name);
    }

    public static class OptionReaderExtensionMethods {

        public static TimeSpan GetDuration(this IOptionReader reader, string name) {
            var value = reader.GetMilliseconds(name);
            // TODO: TBD: the "get" direction is fine. But we should be careful of the "set" direction because TimeSpan works in Double, whereas NNG works in Int32.
            return TimeSpan.FromMilliseconds(value);
        }
    }
}
