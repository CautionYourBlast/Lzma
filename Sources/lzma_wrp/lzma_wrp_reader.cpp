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


#include "lzma_wrp_reader.h"
#include "lzma_wrp_item.h"
#include "lzma_wrp_private.h"
#include "lzma_wrp_common.h"

uint32_t lzma_wrp_reader_get_items_count(lzma_wrp_reader reader) {
    LZMA_WRP_ASSERT(reader)
    return reader->decoder->itemsCount();
}

lzma_wrp_item lzma_wrp_reader_get_item_at_index(lzma_wrp_reader reader, const uint32_t index) {
    LZMA_WRP_ASSERT(reader)
    LzmaWRP::FileDecoder * d = reader->decoder;
    
    d->clearLastError();
    if (d->itemsCount() == 0) {
        return NULL;
    }
    
    lzma_wrp_item item = NULL;
    
    PROPVARIANT prop, name;
    memset(&prop, 0, sizeof(PROPVARIANT));
    memset(&name, 0, sizeof(PROPVARIANT));
    if (d->readIteratorProperty(&prop, kpidSize, index) && d->readIteratorProperty(&name, kpidPath, index)) {
        item = (lzma_wrp_item)lzma_wrp_malloc_zero(sizeof(struct lzma_wrp_item_struct));
        item->index = index;
        item->size = lzma_wrp_common_PROPVARIANT_get_uint64(&prop);
        
        memset(&prop, 0, sizeof(PROPVARIANT));
        if (d->readIteratorProperty(&prop, kpidPackSize, index)) {
            item->pack_size = lzma_wrp_common_PROPVARIANT_get_uint64(&prop);
        }
        
        if (name.vt == VT_BSTR && name.bstrVal) {
            const size_t len = wcslen(name.bstrVal);
            if (len > 0) {
                item->name = (wchar_t *)lzma_wrp_malloc(sizeof(wchar_t) * (len + 1));
                memcpy(item->name, name.bstrVal, sizeof(wchar_t) * len);
                item->name[len] = 0;
            }
        }
        
        memset(&prop, 0, sizeof(PROPVARIANT));
        if (d->readIteratorProperty(&prop, kpidCTime, index) && prop.vt == VT_FILETIME) {
            item->creation_time = lzma_wrp_common_FILETIME_to_unix_time(prop.filetime);
        }
        
        memset(&prop, 0, sizeof(PROPVARIANT));
        if (d->readIteratorProperty(&prop, kpidATime, index) && prop.vt == VT_FILETIME) {
            item->access_time = lzma_wrp_common_FILETIME_to_unix_time(prop.filetime);
        }
        
        memset(&prop, 0, sizeof(PROPVARIANT));
        if (d->readIteratorProperty(&prop, kpidMTime, index) && prop.vt == VT_FILETIME) {
            item->modification_time = lzma_wrp_common_FILETIME_to_unix_time(prop.filetime);
        }
        
        memset(&prop, 0, sizeof(PROPVARIANT));
        if (d->readIteratorProperty(&prop, kpidEncrypted, index) && prop.vt == VT_BOOL && prop.boolVal) {
            item->flags |= lzma_wrp_item_flag_is_encrypted;
        }
        
        memset(&prop, 0, sizeof(PROPVARIANT));
        if (d->readIteratorProperty(&prop, kpidCRC, index)) {
            item->crc = (uint32_t)lzma_wrp_common_PROPVARIANT_get_uint64(&prop);
        }
        
        memset(&prop, 0, sizeof(PROPVARIANT));
        if (d->readIteratorProperty(&prop, kpidIsDir, index) && prop.vt == VT_BOOL && prop.boolVal) {
            item->flags |= lzma_wrp_item_flag_is_dir;
        }
    }
    NWindows::NCOM::PropVariant_Clear(&name);
    return item;
}

int lzma_wrp_reader_process_items(lzma_wrp_reader reader, const lzma_wrp_item * items, const uint32_t items_count, const lzma_wrp_char * path, const int is_with_full_paths) {
    LZMA_WRP_ASSERT(reader)
    LZMA_WRP_ASSERT(items)
    LzmaWRP::FileDecoder * d = reader->decoder;
    
    int is_ok = 0;
    uint32_t * indices = (uint32_t *)lzma_wrp_malloc(items_count * sizeof(uint32_t));
    if (indices) {
        for (uint32_t i = 0; i < items_count; i++) {
            lzma_wrp_item item = items[i];
            LZMA_WRP_ASSERT(item)
            indices[i] = item->index;
        }
        is_ok = lzma_wrp_reader_process_item_indices(reader, indices, items_count, path, is_with_full_paths);
        lzma_wrp_free(indices);
    } else {
        d->setLastError(-1, __LINE__, __FILE__, "Can't allocate enough memory for items indexes: [%u] bytes", (unsigned int)(items_count * sizeof(uint32_t)));
    }
    return is_ok;
}

int lzma_wrp_reader_process_item_indices(lzma_wrp_reader reader, const uint32_t * items_indices, const uint32_t items_count, const lzma_wrp_char * path, const int is_with_full_paths) {
    LZMA_WRP_ASSERT(reader)
    LZMA_WRP_ASSERT(items)
    LzmaWRP::FileDecoder * d = reader->decoder;
    
    d->clearLastError();
    bool is_full_paths = false;
    if (path) {
        is_full_paths = is_with_full_paths != 0 ? true : false;
    }
    return d->process(items_indices, items_count, path, is_full_paths) ? 1 : 0;
}

int lzma_wrp_reader_open_file_path(lzma_wrp_reader reader, const lzma_wrp_char * file_path) {
    LZMA_WRP_ASSERT(reader)
    LzmaWRP::FileDecoder * d = reader->decoder;
    if (!d->prepare(reader->type)) {
        return 0;
    }
    return d->openFile(file_path) ? LZMA_WRP_TRUE : LZMA_WRP_FALSE;
}

void lzma_wrp_reader_set_type(lzma_wrp_reader reader, const int32_t type) {
    LZMA_WRP_ASSERT(reader)
    reader->type = type;
}

int32_t lzma_wrp_reader_get_type(lzma_wrp_reader reader) {
    LZMA_WRP_ASSERT(reader)
    return reader->type;
}

void lzma_wrp_reader_set_user_object(lzma_wrp_reader reader, lzma_wrp_handle user_object) {
    LZMA_WRP_ASSERT(reader)
    reader->user_object = user_object;
}

lzma_wrp_handle lzma_wrp_reader_get_user_object(lzma_wrp_reader reader) {
    LZMA_WRP_ASSERT(reader)
    return reader->user_object;
}

lzma_wrp_error lzma_wrp_reader_get_last_error(lzma_wrp_reader reader) {
    LZMA_WRP_ASSERT(reader)
    return reader->decoder->lastError();
}

void lzma_wrp_reader_set_progress_callback(lzma_wrp_reader reader, void (*cb)(lzma_wrp_reader reader, const double progress)) {
    LZMA_WRP_ASSERT(reader)
    reader->decoder->setProgressCallback(cb);
}

void lzma_wrp_reader_set_password(lzma_wrp_reader reader, const lzma_wrp_wchar * password) {
    LZMA_WRP_ASSERT(reader)
    reader->decoder->setPassword(password);
}

lzma_wrp_reader lzma_wrp_reader_create(void) {
    lzma_wrp_reader reader = (lzma_wrp_reader)lzma_wrp_malloc_zero(sizeof(struct lzma_wrp_reader_struct));
    LzmaWRP::FileDecoder * decoder = new LzmaWRP::FileDecoder();
    LZMA_WRP_ASSERT(decoder)
    decoder->setReader(reader);
    reader->decoder = decoder;
    return reader;
}

void lzma_wrp_reader_delete(lzma_wrp_reader reader) {
    LZMA_WRP_ASSERT(reader)
    delete reader->decoder;
    lzma_wrp_free(reader);
}
