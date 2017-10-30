namespace Nng.Sharp {
    using System;

    public partial interface IHaveOptions<TOptions>
        where TOptions : class, IOptions
    {
        TOptions Options { get; }

        TOptions GetOptions();
    }
}
