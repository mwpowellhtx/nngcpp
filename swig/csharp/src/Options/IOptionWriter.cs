namespace Nng.Sharp {
    using System;

    public partial interface IOptionWriter : IOptions
    {
        void SetText(string name, string s);

        void GetInt32(string name, int value);

        void SetSize(string name, ulong sz);

        void SetMilliseconds(string name, int value);
    }

    public static class OptionWriterExtensionMethods {

        public static void SetDuration(this IOptionWriter writer, string name, TimeSpan value) {
            writer.SetMilliseconds((int)value.TotalMilliseconds);
        }
    }
}
