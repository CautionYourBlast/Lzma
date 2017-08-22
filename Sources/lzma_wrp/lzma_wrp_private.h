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


#ifndef __LZMA_WRP_PRIVATE_H__
#define __LZMA_WRP_PRIVATE_H__ 1

#include "lzma_wrp.h"

#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <string.h>
#include <wchar.h>
#include <assert.h>

#define LZMA_WRP 1
#define LZMA_WRP_VERSION_MAJOR 3
#define LZMA_WRP_VERSION_MINOR 0
#define LZMA_WRP_VERSION_PATCH 0

/*   Detect building platform 32bit/64bit  */
#if ( (!defined(LZMA_WRP_PLATFORM_32BIT)) && (!defined(LZMA_WRP_PLATFORM_64BIT)) )

#if ( defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(_AMD64_) || defined(_M_AMD64) )
#define LZMA_WRP_PLATFORM_64BIT 1
#endif /* Detect 64bit AMD64 */

#ifndef LZMA_WRP_PLATFORM_64BIT
#if ( defined(__LP64__) || defined(__ia64__) || defined(_IA64) || defined(__IA64__) || defined(__ia64) || defined(_M_IA64) )
#define LZMA_WRP_PLATFORM_64BIT 1
#endif /* Detect 64bit Intel Architecture-64 */
#endif

#ifndef LZMA_WRP_PLATFORM_64BIT
#if ( defined(_WIN64) || defined(__X86_64__) || defined(WIN64) || defined(_LP64) || defined(ppc64) || defined(x86_64) )
#define LZMA_WRP_PLATFORM_64BIT 1
#endif /* Detect 64bit common defines */
#endif

#ifndef LZMA_WRP_PLATFORM_64BIT
#if ( defined(__x86_64__) || defined(__ppc64__) )
#define LZMA_WRP_PLATFORM_64BIT 1
#endif /* Detect 64bit common defines */
#endif


#ifndef LZMA_WRP_PLATFORM_64BIT
#define LZMA_WRP_PLATFORM_32BIT 1
#endif /* if not 64bit than select 32bit */

#endif

// check os
#if defined(LZMA_WRP_OS_WINDOWS)

#ifndef _WIN32
#define _WIN32
#endif

#ifndef WIN32
#define WIN32
#endif

#else

#include <unistd.h>
#include <pthread.h>

// check apple
#if defined(__APPLE__) || defined(TARGET_OS_MAC) || defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_EMBEDDED)
#define LZMA_WRP_OS_APPLE 1
#endif

#endif


// types
typedef void *PVOID, *LPVOID;
typedef PVOID HANDLE;

#if !defined(DWORD_SIZE)
#define DWORD_SIZE 4
typedef uint32_t DWORD;
#endif

#if !defined(__OBJC__)
typedef uint8_t BOOL;
#endif

#if !defined(TRUE)
#define TRUE 1
#endif

#if !defined(FALSE)
#define FALSE 0
#endif

#ifndef USE_MIXER_ST
#define USE_MIXER_ST
#endif

#ifndef _7ZIP_ST
#define _7ZIP_ST 1
#endif

#define LZMA_WRP_TRUE   1
#define LZMA_WRP_FALSE  0

#define LZMA_WRP_FILE_TYPE_UNDEFINED    0
#define LZMA_WRP_FILE_TYPE_7Z           1

#define LZMA_WRP_METHOD_LZMA    0
#define LZMA_WRP_METHOD_LZMA2   1

typedef void * (*LzmaSDKObjCGetVoidCallback)(void * context);
typedef void (*LzmaSDKObjCSetFloatCallback)(void * context, float value);

#if (defined(DEBUG) || defined(_DEBUG)) && !defined(LZMA_WRP_NO_DEBUG_LOG)
#define LZMA_WRP_DEBUG_LOG(M, ...) fprintf(stdout, "LZMA DEBUG %d: " M "\n", __LINE__, ##__VA_ARGS__);
#define LZMA_WRP_DEBUG_ERR(M, ...) fprintf(stderr, "LZMA ERROR %d: " M "\n", __LINE__, ##__VA_ARGS__);
#else
#define LZMA_WRP_DEBUG_LOG(M, ...)
#define LZMA_WRP_DEBUG_ERR(M, ...)
#endif

#if defined(DEBUG) || defined(_DEBUG)
#define LZMA_WRP_ASSERT(ASSERT_CONDITION) assert(ASSERT_CONDITION);
#else
#define LZMA_WRP_ASSERT(ASSERT_CONDITION)
#endif

#endif
