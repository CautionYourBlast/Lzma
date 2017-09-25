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


#ifndef __LIBCLZMA_H__
#define __LIBCLZMA_H__ 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#ifndef LIBCLZMA
#define LIBCLZMA 1
#endif

// check windows
#if defined(WIN32) || defined(_WIN32) || defined(WIN32_LEAN_AND_MEAN) || defined(_WIN64) || defined(WIN64)
#define CLZMA_OS_WINDOWS 1
#endif


// extern
#if defined(__cplusplus) || defined(_cplusplus)
#define CLZMA_EXTERN extern "C"
#else
#define CLZMA_EXTERN extern
#endif


// attribute
#if defined(__GNUC__)
#if (__GNUC__ >= 4)
#if defined(__cplusplus) || defined(_cplusplus)
#define CLZMA_ATTRIB __attribute__((visibility("default")))
#else
#define CLZMA_ATTRIB __attribute__((visibility("default")))
#endif
#endif
#endif


// check attrib and define empty if not defined
#if !defined(CLZMA_ATTRIB)
#define CLZMA_ATTRIB
#endif


// dll api
#if defined(CLZMA_OS_WINDOWS)
#if defined(CLZMA_BUILD)
#define CLZMA_DYLIB_API __declspec(dllexport)
#else
#define CLZMA_DYLIB_API __declspec(dllimport)
#endif
#endif


// check dll api and define empty if not defined
#if !defined(CLZMA_DYLIB_API)
#define CLZMA_DYLIB_API
#endif


// combined lib api
#define CLZMA_API(return_type) CLZMA_EXTERN CLZMA_ATTRIB CLZMA_DYLIB_API return_type

typedef void * clzma_reader_t;
typedef struct clzma_writer_struct * clzma_writer_t;
typedef void * clzma_item_t;
typedef void * clzma_error_t;
typedef char clzma_char_t;
typedef wchar_t clzma_wchar_t;

//#define CLZMA_TRUE   1
//#define CLZMA_FALSE  0
//
//#define CLZMA_FILE_TYPE_UNDEFINED    0
//#define CLZMA_FILE_TYPE_7Z           1
//
//#define CLZMA_METHOD_LZMA    0
//#define CLZMA_METHOD_LZMA2   1

// mem
CLZMA_API(void *) clzma_malloc(const size_t size);

CLZMA_API(void *) clzma_malloc_zero(const size_t size);

CLZMA_API(void *) clzma_realloc(void * mem, const size_t new_size);

CLZMA_API(void) clzma_free(void * mem);

CLZMA_API(void) clzma_free_clean(void ** mem);

// reader
CLZMA_API(clzma_reader_t) clzma_reader_create(void);

CLZMA_API(void) clzma_reader_set_progress_callback(clzma_reader_t reader, void (*cb)(clzma_reader_t reader, const double progress));

CLZMA_API(void) clzma_reader_set_password(clzma_reader_t reader, const clzma_wchar_t * password);

CLZMA_API(void) clzma_reader_set_user_object(clzma_reader_t reader, void * user_object);

CLZMA_API(void *) clzma_reader_get_user_object(clzma_reader_t reader);

/**
 @param type CLZMA_FILE_TYPE_UNDEFINED (0) | CLZMA_FILE_TYPE_7Z (1)
 @return CLZMA_TRUE (1) | CLZMA_FALSE (0)
 */
CLZMA_API(int) clzma_reader_open_file_path(clzma_reader_t reader, const clzma_char_t * file_path, const int32_t type);

CLZMA_API(uint32_t) clzma_reader_get_items_count(clzma_reader_t reader);

CLZMA_API(clzma_item_t) clzma_reader_get_item_at_index(clzma_reader_t reader, const uint32_t index);

CLZMA_API(int) clzma_reader_extract_items(clzma_reader_t reader,
                                          const clzma_item_t * items,
                                          const uint32_t items_count,
                                          const clzma_char_t * path,
                                          const int is_with_full_paths);

/**
 @param items_indices Sorted list of item indices. Use 'qsort' to sort indices before.
 */
CLZMA_API(int) clzma_reader_extract_item_indices(clzma_reader_t reader,
                                                 const uint32_t * items_indices,
                                                 const uint32_t items_count,
                                                 const clzma_char_t * path,
                                                 const int is_with_full_paths);

CLZMA_API(int) clzma_reader_extract_all_items(clzma_reader_t reader, const clzma_char_t * path, const int is_with_full_paths);

CLZMA_API(clzma_error_t) clzma_reader_get_last_error(clzma_reader_t reader);

CLZMA_API(void) clzma_reader_delete(clzma_reader_t reader);

// item
CLZMA_API(const clzma_char_t *) clzma_item_get_path(clzma_item_t item);

CLZMA_API(uint64_t) clzma_item_get_size(clzma_item_t item);

CLZMA_API(uint64_t) clzma_item_get_packed_size(clzma_item_t item);

CLZMA_API(uint32_t) clzma_item_get_index(clzma_item_t item);

CLZMA_API(uint32_t) clzma_item_get_crc32(clzma_item_t item);

CLZMA_API(time_t) clzma_item_get_creation_time(clzma_item_t item);

CLZMA_API(time_t) clzma_item_get_access_time(clzma_item_t item);

CLZMA_API(time_t) clzma_item_get_modification_time(clzma_item_t item);

/**
 @return CLZMA_TRUE (1) | CLZMA_FALSE (0)
 */
CLZMA_API(int) clzma_item_is_encrypted(clzma_item_t item);

/**
 @return CLZMA_TRUE (1) | CLZMA_FALSE (0)
 */
CLZMA_API(int) clzma_item_is_dir(clzma_item_t item);

CLZMA_API(void) clzma_item_delete(clzma_item_t item);

// writer
CLZMA_API(clzma_writer_t) clzma_writer_create(void);

CLZMA_API(void) clzma_writer_set_progress_callback(clzma_writer_t writer, void (*cb)(clzma_writer_t writer, const double progress));

CLZMA_API(void) clzma_writer_set_password(clzma_writer_t writer, const clzma_wchar_t * password);

CLZMA_API(void) clzma_writer_delete(clzma_writer_t writer);

// error
CLZMA_API(const clzma_char_t *) clzma_error_get_description(clzma_error_t error);

CLZMA_API(const clzma_char_t *) clzma_error_get_possible_reason(clzma_error_t error);

CLZMA_API(const clzma_char_t *) clzma_error_get_file(clzma_error_t error);

CLZMA_API(int64_t) clzma_error_get_code(clzma_error_t error);

CLZMA_API(int32_t) clzma_error_get_line(clzma_error_t error);

CLZMA_API(const clzma_char_t *) clzma_version_description(void);

#endif

