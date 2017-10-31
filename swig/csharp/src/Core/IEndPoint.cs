namespace Nng.Sharp {

using System;
using System.Runtime.InteropServices;

public partial interface IEndPoint {

    void Start();

    void Start(SocketFlag flags);
}

}
