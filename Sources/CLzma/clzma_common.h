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


#ifndef __CLZMA_COMMON_H__
#define __CLZMA_COMMON_H__ 1

#include <stdio.h>
#include <time.h>
#include <string.h>

#include "clzma_private.h"

#include "CPP/Common/MyGuidDef.h"
#include "CPP/Common/MyWindows.h"

namespace CLzma {
    void initialize(void);
    GUID CLSIDFormat7z(void);
    uint64_t PROPVARIANTGetUInt64(PROPVARIANT * prop);
    bool PROPVARIANTGetBool(PROPVARIANT * prop);
    time_t FILETIMEToUnixTime(const FILETIME filetime);
    FILETIME UnixTimeToFILETIME(const time_t t);
    
    template<typename T>
    int compareIndices(const void * a, const void * b) {
        if ( *(T*)a < *(T*)b ) return -1;
        else if ( *(T*)a > *(T*)b ) return 1;
        return 0;
    }
}

#endif
