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


#include "lzma_wrp_item.h"

const lzma_wrp_wchar * lzma_wrp_item_get_name(lzma_wrp_item item) {
    LZMA_WRP_ASSERT(item)
    return item->name;
}

uint64_t lzma_wrp_item_get_size(lzma_wrp_item item) {
    LZMA_WRP_ASSERT(item)
    return item->size;
}

uint64_t lzma_wrp_item_get_packed_size(lzma_wrp_item item) {
    LZMA_WRP_ASSERT(item)
    return item->pack_size;
}

uint32_t lzma_wrp_item_get_index(lzma_wrp_item item) {
    LZMA_WRP_ASSERT(item)
    return item->index;
}

uint32_t lzma_wrp_item_get_crc32(lzma_wrp_item item) {
    LZMA_WRP_ASSERT(item)
    return item->crc;
}

time_t lzma_wrp_item_get_creation_time(lzma_wrp_item item) {
    LZMA_WRP_ASSERT(item)
    return item->creation_time;
}

time_t lzma_wrp_item_get_access_time(lzma_wrp_item item) {
    LZMA_WRP_ASSERT(item)
    return item->access_time;
}

time_t lzma_wrp_item_get_modification_time(lzma_wrp_item item) {
    LZMA_WRP_ASSERT(item)
    return item->modification_time;
}

int lzma_wrp_item_is_encrypted(lzma_wrp_item item) {
    LZMA_WRP_ASSERT(item)
    return (item->flags & lzma_wrp_item_flag_is_encrypted) ? 1 : 0;
}

int lzma_wrp_item_is_dir(lzma_wrp_item item) {
    LZMA_WRP_ASSERT(item)
    return (item->flags & lzma_wrp_item_flag_is_dir) ? 1 : 0;
}

void lzma_wrp_item_delete(lzma_wrp_item item) {
    LZMA_WRP_ASSERT(item)
    lzma_wrp_free(item->name);
    lzma_wrp_free(item);
}
