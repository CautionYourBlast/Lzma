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


#include "clzma_path.h"

#include <sys/stat.h>

namespace CLzma {
    
    void Path::deleteLastPathComponent() {
        const size_t len = Len();
        if (len > 0) {
            const int sepPos = ReverseFind_PathSepar();
            if (sepPos > 0) {
                DeleteFrom(sepPos);
            }
        }
    }
    
    const char * Path::lastPathComponent() const {
        const size_t len = Len();
        if (len > 0) {
            const int sepPos = ReverseFind_PathSepar();
            if (sepPos >= 0) {
                return Ptr(sepPos);
            }
        }
        return Ptr();
    }
    
    void Path::appendPath(const char * path) {
        if (IS_PATH_SEPAR(*path)) {
            *this += path;
        } else {
            Add_PathSepar();
            *this += path;
        }
    }
    
    bool Path::createDir(bool with_intermediate_directories /* = true */ ) const {
        if (Len() == 0) {
            return false;
        }
        
        if (with_intermediate_directories) {
            CLzma::Path holder(Ptr());
            char * path = holder.GetBuf(0);
            char * p = path;
            unsigned int componentsCount = 0;
            bool lastComponentProcessed = false;
            while (*p) {
                if (IS_PATH_SEPAR(*p)) {
                    lastComponentProcessed = true;
                    if (componentsCount > 0) {
                        const char sep = *p;
                        *p = 0;
                        if (!Path::existsDir(path)) {
                            if (!Path::makeDir(path)) {
                                return false;
                            }
                        }
                        *p = sep;
                    }
                    componentsCount++;
                } else {
                    lastComponentProcessed = false;
                }
                p++;
            }
            if (!lastComponentProcessed) {
                Path::makeDir(path);
            }
            return Path::existsDir(Ptr());
        } else return Path::makeDir(Ptr());
    }
    
    bool Path::exists(bool * is_dir /* = NULL */) const {
        if (Len() == 0) {
            return false;
        }
        bool isDir = false;
        const bool res = Path::exists(Ptr(), &isDir);
        if (is_dir) {
            *is_dir = isDir;
        }
        return res;
    }
    
    void Path::removeEndingPathSeparator() {
        const size_t len = Len();
        if (len > 0) {
            const int sepPos = ReverseFind_PathSepar();
            if (sepPos >= 0 && (len - 1) == sepPos) {
                DeleteBack();
            }
        }
    }
    
    Path::Path(const char * s) : CLzma::String(s) {
        removeEndingPathSeparator();
    }
    
    Path::Path(const wchar_t * s) : CLzma::String(s) {
        removeEndingPathSeparator();
    }
    
    bool Path::makeDir(const char * path) {
        const mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR |
                            S_IRGRP |           S_IXGRP |
                                                S_IXOTH ;
        
        //#define	S_IRWXU		0000700		/* [XSI] RWX mask for owner */
        //#define	S_IRUSR		0000400		/* [XSI] R for owner */
        //#define	S_IWUSR		0000200		/* [XSI] W for owner */
        //#define	S_IXUSR		0000100		/* [XSI] X for owner */
        //        /* Read, write, execute/search by group */
        //#define	S_IRWXG		0000070		/* [XSI] RWX mask for group */
        //#define	S_IRGRP		0000040		/* [XSI] R for group */
        //#define	S_IWGRP		0000020		/* [XSI] W for group */
        //#define	S_IXGRP		0000010		/* [XSI] X for group */
        //        /* Read, write, execute/search by others */
        //#define	S_IRWXO		0000007		/* [XSI] RWX mask for other */
        //#define	S_IROTH		0000004		/* [XSI] R for other */
        //#define	S_IWOTH		0000002		/* [XSI] W for other */
        //#define	S_IXOTH		0000001		/* [XSI] X for other */
        
        return mkdir(path, mode) == 0;
    }
    
    bool Path::exists(const char * path, bool * isDir) {
        struct stat statbuf;
        if (stat(path, &statbuf) == 0) {
            *isDir = (statbuf.st_mode & S_IFMT) == S_IFDIR;
            return true;
        }
        return false;
    }
    
    bool Path::existsDir(const char * path) {
        bool isDir = false;
        return Path::exists(path, &isDir) ? isDir : false;
    }
    
    bool Path::existsFile(const char * path) {
        bool isDir = false;
        return Path::exists(path, &isDir) ? !isDir : false;
    }
}

