/*
  Copyright (c) 2017 Michael W. Powell <mwpowellhtx@gmail.com> All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom
  the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included
  in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
  IN THE SOFTWARE.
*/

%{
#include "cpp/src/transport/views/ipc_family_view.h"
%}

%module(directors="1") Transport;

%include "csharp/src/Core/Types.i"

namespace nng {

%ignore align_view(const _SockAddr&);

%typemap(csimports) _IpcFamilyView %{
using System;
using System.Runtime.InteropServices;
%}

%typemap(csclassmodifiers) _IpcFamilyView "public partial class";
%typemap(csinterfaces) _IpcFamilyView "IHaveOne, IAddressFamilyView, IEquatable<IAddressFamilyView>";
%typemap(csbase) _IpcFamilyView "";

// TODO: TBD: "equatable" may not be something that gets easily exposed after all... at least not via SWIG, and not without a little during the mapping...
%ignore _IpcFamilyView::operator==;
%ignore _IpcFamilyView::operator!=;
%ignore _IpcFamilyView::Equals;

//// TODO: TBD: should not need these modifer changes any longer.
//%csmethodmodifiers _IpcFamilyView::GetIPv4Addr "public override";
//%csmethodmodifiers _IpcFamilyView::SetIPv4Addr "public override";
//%csmethodmodifiers _IpcFamilyView::GetIPv6Addr "public override";
//%csmethodmodifiers _IpcFamilyView::GetIPv6Addr16 "public override";
//%csmethodmodifiers _IpcFamilyView::GetIPv6Addr32 "public override";
//%csmethodmodifiers _IpcFamilyView::SetIPv6Addr "public override";
//%csmethodmodifiers _IpcFamilyView::SetIPv6Addr16 "public override";
//%csmethodmodifiers _IpcFamilyView::SetIPv6Addr32 "public override";
//%csmethodmodifiers _IpcFamilyView::__GetPort "public override";
//%csmethodmodifiers _IpcFamilyView::__SetPort "public override";

// Rename methods which caused conflicts with internally seen included definitions.
%rename("GetPort") _IpcFamilyView::__GetPort;
%rename("SetPort") _IpcFamilyView::__SetPort;

%rename("InterProcessFamilyView") _IpcFamilyView;

}

%include "cpp/src/transport/views/ipc_family_view.h"
