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
        const int l = String::wideToChars(wcs, len, out);
        ReleaseBuf_SetEnd(l);
    }
    
    UString String::ustring() const {
        UString s;
        const size_t len = Len();
        if (len > 0) {
            wchar_t * out = s.GetBuf((unsigned int)(len + 1));
            const int l = String::charsToWide(Ptr(), len, out);
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
    
    int String::wideToChars(const wchar_t * wideString, int wideStringLength, char * charsString) {
        int stringLength = 0;
        if (wideStringLength > 0) {
            int l = wideStringLength;
            unsigned char * cursor = (unsigned char *)charsString;
            const wchar_t * ch = wideString;
            for (int i = 0; i < l; i++) {
                wchar_t u = *ch;
                if (u < (unsigned char)0x80) {
                    *cursor++ = (unsigned char)u;
                } else {
                    if (u < 0x0800) {
                        *cursor++ = (unsigned char)0xc0 | ((unsigned char) (u >> 6));
                    } else {
                        if ( (u >= 0xd800) && (u < 0xdc00) && (i < l-1)) {
                            unsigned short low = ch[1];
                            if ((low >= 0xdc00) && (low < 0xe000)) {
                                ++ch;
                                ++i;
                                u = ((u - 0xd800)*0x400) + (low - 0xdc00) + 0x10000;
                            }
                        }
                        if (u > 0xffff) {
                            // if people are working in utf8, but strings are encoded in eg. latin1, the resulting
                            // name might be invalid utf8. This and the corresponding code in fromUtf8 takes care
                            // we can handle this without loosing information. This can happen with latin filenames
                            // and a utf8 locale under Unix.
                            if ( (u > 0x10fe00) && (u < 0x10ff00) ) {
                                *cursor++ = (u - 0x10fe00);
                                ++ch;
                                continue;
                            } else {
                                *cursor++ = (unsigned char)0xf0 | ((unsigned char) (u >> 18));
                                *cursor++ = (unsigned char)0x80 | (((unsigned char) (u >> 12)) & (unsigned char)0x3f);
                            }
                        } else {
                            *cursor++ = (unsigned char)0xe0 | ((unsigned char) (u >> 12));
                        }
                        *cursor++ = (unsigned char)0x80 | (((unsigned char) (u >> 6)) & (unsigned char)0x3f);
                    }
                    *cursor++ = (unsigned char)0x80 | ((unsigned char) (u & (unsigned char)0x3f));
                }
                ++ch;
            }
            stringLength = (int)(cursor - (unsigned char*)charsString);
        }
        return stringLength;
    }
    
    int String::charsToWide(const char * charsString, int charsStringLengthOrNegative, wchar_t * wideString) {
        if (charsStringLengthOrNegative < 0) {
            charsStringLengthOrNegative = (int)strlen(charsString);
        }
        
        wchar_t uc = 0;
        int need = 0;
        int error = -1;
        unsigned char ch;
        int stringLength = 0;
        for (int i = 0; i < charsStringLengthOrNegative; ++i) {
            ch = charsString[i];
            if (need) {
                if ((ch&(unsigned char)0xc0) == (unsigned char)0x80) {
                    uc = (uc << 6) | (ch & (unsigned char)0x3f);
                    need--;
                    if (!need) {
                        if (uc > 0xffff) {
                            uc -= 0x10000;
                            wchar_t high = (uc/0x400) + 0xd800;
                            wchar_t low = (uc%0x400) + 0xdc00;
                            *wideString++ = high;
                            *wideString++ = low;
                            stringLength++;
                        } else {
                            *wideString++ = uc;
                            stringLength++;
                        }
                    }
                } else {
                    i = error;
                    *wideString++ = (wchar_t)0xdbff;
                    *wideString++ = (wchar_t)0xde00 + ((unsigned char)charsString[i]);
                    stringLength++;
                    need = 0;
                }
            } else {
                if (ch < 128) {
                    *wideString++ = ch;
                    stringLength++;
                } else if ((ch & 0xe0) == 0xc0) {
                    uc = ch & 0x1f;
                    need = 1;
                    error = i;
                } else if ((ch & 0xf0) == 0xe0) {
                    uc = ch & 0x0f;
                    need = 2;
                    error = i;
                } else if ((ch & 0xf8) == 0xf0) {
                    uc = ch & 0x07;
                    need = 3;
                    error = i;
                } else {
                    *wideString++ = 0xdbff;
                    *wideString++ = 0xde00 + ((unsigned char)charsString[i]);
                    stringLength++;
                }
            }
        }
        if (need) {
            for (int i = error; i < charsStringLengthOrNegative; ++i) {
                *wideString++ = 0xdbff;
                *wideString++ = 0xde00 + (unsigned char)charsString[i];
                stringLength++;
            }
        }
        return stringLength;
    }
}
