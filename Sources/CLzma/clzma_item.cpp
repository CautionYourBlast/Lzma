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


#include "clzma_item.h"

namespace CLzma {
    
    Item::Item(BSTR path, const uint32_t index) :
        _path(path),
        _index(0),
        size(0),
        packSize(0),
        crc32(0),
        creationTime(0),
        accessTime(0),
        modificationTime(0),
        isEncrypted(false),
        isDir(false) {
        
        }
}

const clzma_char_t * clzma_item_get_path(clzma_item_t item) {
    CLZMA_ASSERT(item)
    return ((CLzma::Item *)item)->path();
}

uint64_t clzma_item_get_size(clzma_item_t item) {
    CLZMA_ASSERT(item)
    return ((CLzma::Item *)item)->size;
}

uint64_t clzma_item_get_packed_size(clzma_item_t item) {
    CLZMA_ASSERT(item)
    return ((CLzma::Item *)item)->packSize;
}

uint32_t clzma_item_get_index(clzma_item_t item) {
    CLZMA_ASSERT(item)
    return ((CLzma::Item *)item)->index();
}

uint32_t clzma_item_get_crc32(clzma_item_t item) {
    CLZMA_ASSERT(item)
    return ((CLzma::Item *)item)->crc32;
}

time_t clzma_item_get_creation_time(clzma_item_t item) {
    CLZMA_ASSERT(item)
    return ((CLzma::Item *)item)->creationTime;
}

time_t clzma_item_get_access_time(clzma_item_t item) {
    CLZMA_ASSERT(item)
    return ((CLzma::Item *)item)->accessTime;
}

time_t clzma_item_get_modification_time(clzma_item_t item) {
    CLZMA_ASSERT(item)
    return ((CLzma::Item *)item)->modificationTime;
}

int clzma_item_is_encrypted(clzma_item_t item) {
    CLZMA_ASSERT(item)
    return ((CLzma::Item *)item)->isEncrypted ? CLZMA_TRUE : CLZMA_FALSE;
}

int clzma_item_is_dir(clzma_item_t item) {
    CLZMA_ASSERT(item)
    return ((CLzma::Item *)item)->isDir ? CLZMA_TRUE : CLZMA_FALSE;
}

void clzma_item_delete(clzma_item_t item) {
    CLZMA_ASSERT(item)
    CLzma::Item * i = (CLzma::Item *)item;
    delete i;
}
