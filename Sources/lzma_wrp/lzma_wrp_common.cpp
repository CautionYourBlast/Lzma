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


#include "lzma_wrp_common.h"
#include "lzma_wrp_private.h"

#include "CPP/Common/MyGuidDef.h"

#include "C/7zVersion.h"
#include "C/7zCrc.h"
#include "C/Aes.h"
#include "C/XzCrc64.h"

// common functs

static lzma_wrp_wchars_to_chars_callback _wchars_to_chars_callback = NULL;

void lzma_wrp_common_set_wchars_to_chars_callback(lzma_wrp_wchars_to_chars_callback cb) {
    _wchars_to_chars_callback = cb;
}

//void lzma_wrp_common_default_wchars_to_chars_callback(const lzma_wrp_wchar * src, const size_t src_len, lzma_wrp_char * dst) {
//    const lzma_wrp_wchar * p = src;
//    mbstate_t mbs;
//    mbrlen(NULL, 0, &mbs);
//    const int ret = wcsrtombs(dst, &p, (src_len + 1) * sizeof(lzma_wrp_wchar), &mbs);
//    if (ret >= 0) {
//        dst[ret] = 0;
//    } else {
//        dst[0] = 0;
//    }
//}

lzma_wrp_wchars_to_chars_callback lzma_wrp_common_get_wchars_to_chars_callback(void) {
    return _wchars_to_chars_callback; // ? _wchars_to_chars_callback : lzma_wrp_common_default_wchars_to_chars_callback;
}

static bool _is_lzma_wrp_common_initialized = false;

void lzma_wrp_common_initialize(void) {
    if (_is_lzma_wrp_common_initialized) {
        return;
    }
    _is_lzma_wrp_common_initialized = true;
    
    CrcGenerateTable();
    AesGenTables();
    Crc64GenerateTable();
}

GUID lzma_wrp_common_CLSID_format_7z(void) {
    return CONSTRUCT_GUID(0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x07, 0x00, 0x00);
}

uint64_t lzma_wrp_common_PROPVARIANT_get_uint64(PROPVARIANT * prop) {
    switch (prop->vt) {
        case VT_UI8: return prop->uhVal.QuadPart;
            
        case VT_HRESULT:
        case VT_UI4:
            return prop->ulVal;
            
        case VT_UINT: return prop->uintVal;
        case VT_I8: return prop->hVal.QuadPart;
        case VT_UI1: return prop->bVal;
        case VT_I4: return prop->lVal;
        case VT_INT: return prop->intVal;
            
        default: break;
    }
    return 0;
}

bool lzma_wrp_common_PROPVARIANT_get_bool(PROPVARIANT * prop) {
    switch (prop->vt) {
        case VT_BOOL: return (prop->boolVal == 0) ? false : true;
        default: break;
    }
    return (lzma_wrp_common_PROPVARIANT_get_uint64(prop) == 0) ? false : true;
}

time_t lzma_wrp_common_FILETIME_to_unix_time(const FILETIME filetime) {
    uint64_t t = filetime.dwHighDateTime;
    t <<= 32;
    t += filetime.dwLowDateTime;
    t -= 116444736000000000LL;
    return (time_t)(t / 10000000);
}

FILETIME lzma_wrp_common_unix_time_to_FILETIME(const time_t t) {
    uint64_t ll = t;
    ll *= 10000000;
    ll += 116444736000000000LL;
    
    FILETIME FT;
    FT.dwLowDateTime = (DWORD)ll;
    FT.dwHighDateTime = (DWORD)(ll >> 32);
    return FT;
}

#define STRING_I(s) #s
#define TO_STRING(s) STRING_I(s)

const lzma_wrp_char * lzma_wrp_version_description(void) {
    const char * info = "Lzma SDK C wrapper library.\n"
    "The library based on original Lzma SDK package.\nOriginal Lzma SDK was patched to support non Windows environment.\n"
    "Original Lzma SDK info:\n - " TO_STRING(MY_VERSION_COPYRIGHT_DATE) "\n"
    "Library info:\n - Version: " TO_STRING(LZMA_WRP_VERSION_MAJOR) "." TO_STRING(LZMA_WRP_VERSION_MINOR) "." TO_STRING(LZMA_WRP_VERSION_PATCH) "\n"
    "Build info:\n"
#if defined(__DATE__) && defined(__TIME__)
    " - Date, time: " TO_STRING(__DATE__) ", " TO_STRING(__TIME__) "\n"
#else
#if defined(__DATE__)
    " - Date: " TO_STRING(__DATE__) "\n"
#endif
#if defined(__TIME__)
    " - Time: " TO_STRING(__TIME__) "\n"
#endif
#endif
#if defined(__unix__)
    " - Compiler targeting Unix systems\n"
#endif
#if defined(__STDC__)
    " - Compiler conforms to ISO Standard C\n"
#endif
#if defined(LZMA_WRP_PLATFORM_32BIT)
    " - Compiled for 32 bit platform\n"
#endif
#if defined(LZMA_WRP_PLATFORM_64BIT)
    " - Compiled for 64 bit platform\n"
#endif
#if defined(__arm__) || defined(__thumb__) || defined(_ARM) || defined(_M_ARM) || defined(_M_ARMT) || defined(__arm)
    " - ARM architecture\n"
#if defined(__ARM_ARCH_7__)
    " - ARMv7\n"
#endif
#if defined(__ARM_ARCH_7A__)
    " - ARMv7a\n"
#endif
#if defined(__ARM_ARCH_7R__)
    " - ARMv7r\n"
#endif
#if defined(__ARM_ARCH_7M__)
    " - ARMv7m\n"
#endif
#if defined(__ARM_ARCH_7S__)
    " - ARMv7s\n"
#endif
#endif
#if defined(__aarch64__)
    " - ARM64 architecture\n"
#endif
#if defined(__ARM_ARCH_8A__)
    " - ARMv8a\n"
#endif
#if defined(__clang__)
    " - Compiled with clang"
#if defined(__clang_version__)
    ", version: " TO_STRING(__clang_version__) "\n"
#else
    "\n"
#endif
#endif
#if defined(_M_ARM)
    " - Compiled for ARM processors\n"
#endif
    ;
    return info;
}
