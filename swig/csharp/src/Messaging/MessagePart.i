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

// Include the file we want to wrap a first time.
%{
#include "cpp/src/messaging/message_part.h"
%}

%module(directors="1") Messaging;

%include "csharp/src/Core/Types.i"

namespace nng {

%typemap(csimports) _MessagePart %{
using System;
using System.Runtime.InteropServices;
%}

%rename("MessagePart") _MessagePart;

%typemap(csclassmodifiers) _MessagePart "public abstract partial class"
%typemap(csinterfaces) _MessagePart "IMessagePart"

%ignore _MessagePart::get_message() const;

%apply uint32_t* OUTPUT { uint32_t* resultp };

%typemap(cscode) _MessagePart %{
  public abstract bool TryGet(ByteVector resultp);

  public abstract ByteVector Get();

  public abstract ulong GetSize();

  public abstract bool HasOne();

  public abstract void Clear();

  public abstract void Append(ByteVector buf);

  public abstract void Append(string s);

  public abstract void Append(uint val);

  public abstract void Prepend(ByteVector buf);

  public abstract void Prepend(string s);

  public abstract void Prepend(uint val);

  public abstract void TrimLeft(ulong sz);

  public abstract void TrimLeft(out uint resultp);

  public abstract void TrimRight(ulong sz);

  public abstract void TrimRight(out uint resultp);
%}

}

%include "cpp/src/messaging/message_part.h"
