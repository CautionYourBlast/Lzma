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


#include "clzma_reader1.h"
#include "clzma_item.h"
#include "clzma_private.h"
#include "clzma_common.h"
#include "clzma_string.h"

//uint32_t clzma_reader_get_items_count(clzma_reader_t reader) {
//    CLZMA_ASSERT(reader)
//    return reader->decoder->itemsCount();
//}
//
//clzma_item_t clzma_reader_get_item_at_index(clzma_reader_t reader, const uint32_t index) {
//    CLZMA_ASSERT(reader)
//    CLzma::Reader * d = reader->decoder;
//    
//    d->clearLastError();
//    if (d->itemsCount() == 0) {
//        return NULL;
//    }
//    
//    clzma_item_t item = NULL;
//    
//    PROPVARIANT prop, name;
//    memset(&prop, 0, sizeof(PROPVARIANT));
//    memset(&name, 0, sizeof(PROPVARIANT));
//    if (d->readIteratorProperty(&prop, kpidSize, index) && d->readIteratorProperty(&name, kpidPath, index)) {
//        item = (clzma_item_t)clzma_malloc_zero(sizeof(struct clzma_item_struct));
//        item->index = index;
//        item->size = CLzma::PROPVARIANTGetUInt64(&prop);
//        
//        memset(&prop, 0, sizeof(PROPVARIANT));
//        if (d->readIteratorProperty(&prop, kpidPackSize, index)) {
//            item->pack_size = CLzma::PROPVARIANTGetUInt64(&prop);
//        }
//        
//        if (name.vt == VT_BSTR && name.bstrVal) {
//            const size_t len = wcslen(name.bstrVal);
//            if (len > 0) {
//                item->name = (wchar_t *)clzma_malloc(sizeof(wchar_t) * (len + 1));
//                memcpy(item->name, name.bstrVal, sizeof(wchar_t) * len);
//                item->name[len] = 0;
//            }
//        }
//        
//        memset(&prop, 0, sizeof(PROPVARIANT));
//        if (d->readIteratorProperty(&prop, kpidCTime, index) && prop.vt == VT_FILETIME) {
//            item->creation_time = CLzma::FILETIMEToUnixTime(prop.filetime);
//        }
//        
//        memset(&prop, 0, sizeof(PROPVARIANT));
//        if (d->readIteratorProperty(&prop, kpidATime, index) && prop.vt == VT_FILETIME) {
//            item->access_time = CLzma::FILETIMEToUnixTime(prop.filetime);
//        }
//        
//        memset(&prop, 0, sizeof(PROPVARIANT));
//        if (d->readIteratorProperty(&prop, kpidMTime, index) && prop.vt == VT_FILETIME) {
//            item->modification_time = CLzma::FILETIMEToUnixTime(prop.filetime);
//        }
//        
//        memset(&prop, 0, sizeof(PROPVARIANT));
//        if (d->readIteratorProperty(&prop, kpidEncrypted, index) && prop.vt == VT_BOOL && prop.boolVal) {
//            item->flags |= clzma_item_flag_is_encrypted;
//        }
//        
//        memset(&prop, 0, sizeof(PROPVARIANT));
//        if (d->readIteratorProperty(&prop, kpidCRC, index)) {
//            item->crc = (uint32_t)CLzma::PROPVARIANTGetUInt64(&prop);
//        }
//        
//        memset(&prop, 0, sizeof(PROPVARIANT));
//        if (d->readIteratorProperty(&prop, kpidIsDir, index) && prop.vt == VT_BOOL && prop.boolVal) {
//            item->flags |= clzma_item_flag_is_dir;
//        }
//    }
//    NWindows::NCOM::PropVariant_Clear(&name);
//    return item;
//}
//
//int clzma_reader_process_items(clzma_reader_t reader, const clzma_item_t * items, const uint32_t items_count, const clzma_char_t * path, const int is_with_full_paths) {
//    CLZMA_ASSERT(reader)
//    CLZMA_ASSERT(items)
//    CLzma::Reader * d = reader->decoder;
//    
//    int is_ok = 0;
//    uint32_t * indices = (uint32_t *)clzma_malloc(items_count * sizeof(uint32_t));
//    if (indices) {
//        for (uint32_t i = 0; i < items_count; i++) {
//            clzma_item_t item = items[i];
//            CLZMA_ASSERT(item)
//            indices[i] = item->index;
//        }
//        is_ok = clzma_reader_process_item_indices(reader, indices, items_count, path, is_with_full_paths);
//        clzma_free(indices);
//    } else {
//        d->setLastError(-1, __LINE__, __FILE__, "Can't allocate enough memory for items indexes: [%u] bytes", (unsigned int)(items_count * sizeof(uint32_t)));
//    }
//    return is_ok;
//}
//
//int clzma_reader_process_item_indices(clzma_reader_t reader, const uint32_t * items_indices, const uint32_t items_count, const clzma_char_t * path, const int is_with_full_paths) {
//    CLZMA_ASSERT(reader)
//    CLZMA_ASSERT(items)
//    CLzma::Reader * d = reader->decoder;
//    
//    d->clearLastError();
//    bool is_full_paths = false;
//    if (path) {
//        is_full_paths = is_with_full_paths != 0 ? true : false;
//    }
//    return d->process(items_indices, items_count, path, is_full_paths) ? 1 : 0;
//}
//
//int clzma_reader_open_file_path(clzma_reader_t reader, const clzma_char_t * file_path) {
//    CLZMA_ASSERT(reader)
//    CLzma::Reader * d = reader->decoder;
//    if (!d->prepare(reader->type)) {
//        return 0;
//    }
//    return d->openFile(file_path) ? CLZMA_TRUE : CLZMA_FALSE;
//}
//
//void clzma_reader_set_type(clzma_reader_t reader, const int32_t type) {
//    CLZMA_ASSERT(reader)
//    reader->type = type;
//}
//
//int32_t clzma_reader_get_type(clzma_reader_t reader) {
//    CLZMA_ASSERT(reader)
//    return reader->type;
//}
//
//void clzma_reader_set_user_object(clzma_reader_t reader, void * user_object) {
//    CLZMA_ASSERT(reader)
//    reader->user_object = user_object;
//}
//
//void * clzma_reader_get_user_object(clzma_reader_t reader) {
//    CLZMA_ASSERT(reader)
//    return reader->user_object;
//}
//
//clzma_error_t clzma_reader_get_last_error(clzma_reader_t reader) {
//    CLZMA_ASSERT(reader)
//    return reader->decoder->lastError();
//}
//
//void clzma_reader_set_progress_callback(clzma_reader_t reader, void (*cb)(clzma_reader_t reader, const double progress)) {
//    CLZMA_ASSERT(reader)
//    reader->decoder->setProgressCallback(cb);
//}
//
//void clzma_reader_set_password(clzma_reader_t reader, const clzma_wchar_t * password) {
//    CLZMA_ASSERT(reader)
//    reader->decoder->setPassword(password);
//}
//
////clzma_reader_t clzma_reader_create(void) {
////    clzma_reader_t reader = (clzma_reader_t)clzma_malloc_zero(sizeof(struct clzma_reader_struct));
////    CLzma::Reader * decoder = new CLzma::Reader();
////    CLZMA_ASSERT(decoder)
////    decoder->setReader(reader);
////    reader->decoder = decoder;
////    
////    CLzma::String s(L"Hello");
////    UString us = s.ustring();
////    
////    return reader;
////}
//
//void clzma_reader_delete(clzma_reader_t reader) {
//    CLZMA_ASSERT(reader)
//    delete reader->decoder;
//    clzma_free(reader);
//}
