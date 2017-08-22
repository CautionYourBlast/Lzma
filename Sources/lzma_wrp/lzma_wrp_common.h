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


#ifndef __LZMA_WRP_COMMON_H__
#define __LZMA_WRP_COMMON_H__ 1

#include <stdio.h>
#include <time.h>
#include <string.h>

#include "CPP/Common/MyGuidDef.h"
#include "CPP/Common/MyWindows.h"
#include "lzma_wrp_private.h"

// common functs
void lzma_wrp_common_initialize(void);
GUID lzma_wrp_common_CLSID_format_7z(void);
uint64_t lzma_wrp_common_PROPVARIANT_get_uint64(PROPVARIANT * prop);
bool lzma_wrp_common_PROPVARIANT_get_bool(PROPVARIANT * prop);
time_t lzma_wrp_common_FILETIME_to_unix_time(const FILETIME filetime);
FILETIME lzma_wrp_common_unix_time_to_FILETIME(const time_t t);


#endif 
