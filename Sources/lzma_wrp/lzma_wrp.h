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


#ifndef __LZMA_WRP_H__
#define __LZMA_WRP_H__ 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

//#define LZMA_WRP_VERSION_MAJOR X
//#define LZMA_WRP_VERSION_MINOR Y
//#define LZMA_WRP_VERSION_PATCH Z

// check windows
#if defined(WIN32) || defined(_WIN32) || defined(WIN32_LEAN_AND_MEAN) || defined(_WIN64) || defined(WIN64)
#define LZMA_WRP_OS_WINDOWS 1
#endif


// extern
#if defined(__cplusplus) || defined(_cplusplus)
#define LZMA_WRP_EXTERN extern "C"
#else
#define LZMA_WRP_EXTERN extern
#endif


// attribute
#if defined(__GNUC__)
#if (__GNUC__ >= 4)
#if defined(__cplusplus) || defined(_cplusplus)
#define LZMA_WRP_ATTRIB __attribute__((visibility("default")))
#else
#define LZMA_WRP_ATTRIB __attribute__((visibility("default")))
#endif
#endif
#endif


// check attrib and define empty if not defined
#if !defined(LZMA_WRP_ATTRIB)
#define LZMA_WRP_ATTRIB
#endif


// dll api
#if defined(LZMA_WRP_OS_WINDOWS)
#if defined(LZMA_WRP_BUILD)
#define LZMA_WRP_DYLIB_API __declspec(dllexport)
#else
#define LZMA_WRP_DYLIB_API __declspec(dllimport)
#endif
#endif


// check dll api and define empty if not defined
#if !defined(LZMA_WRP_DYLIB_API)
#define LZMA_WRP_DYLIB_API
#endif


// combined lib api
#define LZMA_WRP_API(return_type) LZMA_WRP_EXTERN LZMA_WRP_ATTRIB LZMA_WRP_DYLIB_API return_type

typedef void* lzma_wrp_handle;

typedef struct lzma_wrp_reader_struct * lzma_wrp_reader;
typedef struct lzma_wrp_writer_struct * lzma_wrp_writer;
typedef struct lzma_wrp_item_struct * lzma_wrp_item;
typedef lzma_wrp_handle lzma_wrp_error;
typedef char lzma_wrp_char;
typedef wchar_t lzma_wrp_wchar;

//#define LZMA_WRP_TRUE   1
//#define LZMA_WRP_FALSE  0
//
//#define LZMA_WRP_FILE_TYPE_UNDEFINED    0
//#define LZMA_WRP_FILE_TYPE_7Z           1
//
//#define LZMA_WRP_METHOD_LZMA    0
//#define LZMA_WRP_METHOD_LZMA2   1

// common
typedef void (*lzma_wrp_wchars_to_chars_callback)(const lzma_wrp_wchar * src, const size_t src_len, lzma_wrp_char * dst);

LZMA_WRP_API(void) lzma_wrp_common_set_wchars_to_chars_callback(lzma_wrp_wchars_to_chars_callback cb);

LZMA_WRP_API(lzma_wrp_wchars_to_chars_callback) lzma_wrp_common_get_wchars_to_chars_callback(void);

// mem
LZMA_WRP_API(lzma_wrp_handle) lzma_wrp_malloc(const size_t size);

LZMA_WRP_API(lzma_wrp_handle) lzma_wrp_malloc_zero(const size_t size);

LZMA_WRP_API(void) lzma_wrp_free(lzma_wrp_handle mem);

LZMA_WRP_API(void) lzma_wrp_free_clean(lzma_wrp_handle * mem);

// reader
LZMA_WRP_API(lzma_wrp_reader) lzma_wrp_reader_create(void);

LZMA_WRP_API(void) lzma_wrp_reader_set_progress_callback(lzma_wrp_reader reader, void (*cb)(lzma_wrp_reader reader, const double progress));

LZMA_WRP_API(void) lzma_wrp_reader_set_password(lzma_wrp_reader reader, const lzma_wrp_wchar * password);

/**
 @param type LZMA_WRP_FILE_TYPE_UNDEFINED (0) | LZMA_WRP_FILE_TYPE_7Z (1)
 */
LZMA_WRP_API(void) lzma_wrp_reader_set_type(lzma_wrp_reader reader, const int32_t type);

/**
 @return LZMA_WRP_FILE_TYPE_UNDEFINED (0) | LZMA_WRP_FILE_TYPE_7Z (1)
 */
LZMA_WRP_API(int32_t) lzma_wrp_reader_get_type(lzma_wrp_reader reader);

LZMA_WRP_API(void) lzma_wrp_reader_set_user_object(lzma_wrp_reader reader, lzma_wrp_handle user_object);

LZMA_WRP_API(lzma_wrp_handle) lzma_wrp_reader_get_user_object(lzma_wrp_reader reader);

/**
 @return LZMA_WRP_TRUE (1) | LZMA_WRP_FALSE (0)
 */
LZMA_WRP_API(int) lzma_wrp_reader_open_file_path(lzma_wrp_reader reader, const lzma_wrp_char * file_path);

LZMA_WRP_API(uint32_t) lzma_wrp_reader_get_items_count(lzma_wrp_reader reader);

LZMA_WRP_API(lzma_wrp_item) lzma_wrp_reader_get_item_at_index(lzma_wrp_reader reader, const uint32_t index);

LZMA_WRP_API(int) lzma_wrp_reader_process_items(lzma_wrp_reader reader, const lzma_wrp_item * items, const uint32_t items_count, const lzma_wrp_char * path, const int is_with_full_paths);

LZMA_WRP_API(int) lzma_wrp_reader_process_item_indices(lzma_wrp_reader reader, const uint32_t * items_indices, const uint32_t items_count, const lzma_wrp_char * path, const int is_with_full_paths);

LZMA_WRP_API(lzma_wrp_error) lzma_wrp_reader_get_last_error(lzma_wrp_reader reader);

LZMA_WRP_API(void) lzma_wrp_reader_delete(lzma_wrp_reader reader);

// item
LZMA_WRP_API(const lzma_wrp_wchar *) lzma_wrp_item_get_name(lzma_wrp_item item);

LZMA_WRP_API(uint64_t) lzma_wrp_item_get_size(lzma_wrp_item item);

LZMA_WRP_API(uint64_t) lzma_wrp_item_get_packed_size(lzma_wrp_item item);

LZMA_WRP_API(uint32_t) lzma_wrp_item_get_index(lzma_wrp_item item);

LZMA_WRP_API(uint32_t) lzma_wrp_item_get_crc32(lzma_wrp_item item);

LZMA_WRP_API(time_t) lzma_wrp_item_get_creation_time(lzma_wrp_item item);

LZMA_WRP_API(time_t) lzma_wrp_item_get_access_time(lzma_wrp_item item);

LZMA_WRP_API(time_t) lzma_wrp_item_get_modification_time(lzma_wrp_item item);

/**
 @return LZMA_WRP_TRUE (1) | LZMA_WRP_FALSE (0)
 */
LZMA_WRP_API(int) lzma_wrp_item_is_encrypted(lzma_wrp_item item);

/**
 @return LZMA_WRP_TRUE (1) | LZMA_WRP_FALSE (0)
 */
LZMA_WRP_API(int) lzma_wrp_item_is_dir(lzma_wrp_item item);

LZMA_WRP_API(void) lzma_wrp_item_delete(lzma_wrp_item item);

// writer
LZMA_WRP_API(lzma_wrp_writer) lzma_wrp_writer_create(void);

LZMA_WRP_API(void) lzma_wrp_writer_set_progress_callback(lzma_wrp_writer writer, void (*cb)(lzma_wrp_writer writer, const double progress));

LZMA_WRP_API(void) lzma_wrp_writer_set_password(lzma_wrp_writer writer, const lzma_wrp_wchar * password);

LZMA_WRP_API(void) lzma_wrp_writer_delete(lzma_wrp_writer writer);

// error
LZMA_WRP_API(const lzma_wrp_char *) lzma_wrp_get_description(lzma_wrp_error error);

LZMA_WRP_API(const lzma_wrp_char *) lzma_wrp_get_possible_reason(lzma_wrp_error error);

LZMA_WRP_API(const lzma_wrp_char *) lzma_wrp_get_file(lzma_wrp_error error);

LZMA_WRP_API(int64_t) lzma_wrp_get_code(lzma_wrp_error error);

LZMA_WRP_API(int32_t) lzma_wrp_get_line(lzma_wrp_error error);

LZMA_WRP_API(const lzma_wrp_char *) lzma_wrp_version_description(void);

#endif

