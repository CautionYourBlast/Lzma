/*
 *   Copyright (c) 2015 - 2017 Kulykov Oleh <info@resident.name>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *   THE SOFTWARE.
 */


#include "clzma_string.h"
#include "clzma_common.h"
#include <wchar.h>

namespace CLzma {
    
    void String::setFromWChars(const wchar_t * wcs, const size_t len) {
        const size_t maxOutBytes = (len + 1) * sizeof(wchar_t);
        char * out = GetBuf((unsigned int)maxOutBytes);
        const int l = CLzma::wideToChars(wcs, len, out);
        ReleaseBuf_SetEnd(l);
    }
    
    UString String::ustring() const {
        UString s;
        const size_t len = Len();
        if (len > 0) {
            wchar_t * out = s.GetBuf((unsigned int)(len + 1));
            const int l = CLzma::charsToWide(Ptr(), len, out);
            s.ReleaseBuf_SetEnd(l);
        }
        return s;
    }
    
    String::String() : AString() { }
    String::String(const AString & s) : AString(s) { }
    String::String(const char * s) : AString(s) { }
    
    String::String(const UString & s) : AString() {
        const size_t len = s.Len();
        if (len > 0) {
            setFromWChars(s.Ptr(), len);
        }
    }
    
    String::String(const wchar_t * wcs) : AString() {
        const size_t len = wcs ? wcslen(wcs) : 0;
        if (len > 0) {
            setFromWChars(wcs, len);
        }
    }
}
