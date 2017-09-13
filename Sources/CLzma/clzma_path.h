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


#ifndef __CLZMA_PATH_H__
#define __CLZMA_PATH_H__ 1

#include "clzma_string.h"

namespace CLzma {
    class Path : public CLzma::String {
    
    private:
        void removeEndingPathSeparator();
        
        static bool makeDir(const char * path);
        static bool exists(const char * path, bool * is_dir);
        static bool existsDir(const char * path);
        static bool existsFile(const char * path);
    public:
        void deleteLastPathComponent();
        const char * lastPathComponent() const;
        void appendPath(const char * path);
        bool createDir(bool with_intermediate_directories = true) const;
        bool exists(bool * is_dir = NULL) const;
        Path(const char * s);
        Path(const wchar_t * s);
    };
}

#endif
