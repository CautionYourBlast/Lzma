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


#ifndef __CLZMA_ITEM_H__
#define __CLZMA_ITEM_H__ 1

#include "clzma_private.h"
#include "clzma_common.h"
#include "clzma_string.h"
#include "clzma_path.h"

#include <time.h>

namespace CLzma {
    class Item {
    private:
        CLzma::Path _path;
        uint32_t _index;
        
    public:
        uint64_t size;
        uint64_t packSize;
        uint32_t crc32;
        time_t creationTime;
        time_t accessTime;
        time_t modificationTime;
        bool isEncrypted;
        bool isDir;
        
        const char * path() const { return _path.Ptr(); }
        uint32_t index() const { return _index; }
        
        Item(BSTR path, const uint32_t index);
    };
}

#endif
