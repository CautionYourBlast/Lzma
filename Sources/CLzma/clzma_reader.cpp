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


#include "clzma_reader.h"
#include "clzma_common.h"
#include "clzma_in_file.h"
#include "clzma_open_callback.h"
#include "clzma_out_streams.h"

#include "CPP/Common/MyWindows.h"
#include "CPP/Common/Defs.h"
#include "CPP/Common/MyGuidDef.h"
#include "CPP/Common/IntToString.h"
#include "CPP/Common/StringConvert.h"

#include "CPP/Windows/PropVariant.h"
#include "CPP/Windows/PropVariantConv.h"

#include "CPP/7zip/Common/FileStreams.h"
#include "CPP/7zip/Archive/DllExports2.h" // custom header with `STDAPI CreateObject(const GUID *clsid, const GUID *iid, void **outObject);`
#include "CPP/7zip/Archive/IArchive.h"
#include "CPP/7zip/IPassword.h"


namespace CLzma {
    
    bool Reader::isSolidArchive() const {
        if (_archive != NULL) {
            PROPVARIANT solidProp = { 0 };
            if (_archive->GetArchiveProperty(kpidSolid, &solidProp) == S_OK) {
                return CLzma::PROPVARIANTGetBool(&solidProp);
            }
        }
        return false;
    }
    
    CLzma::Item * Reader::itemAtIndex(const uint32_t index) {
        this->clearLastError();
        
        if (_itemsCount == 0) {
            return NULL;
        }
        
        CLZMA_ASSERT(index < _itemsCount)
        
        CLzma::Item * item = NULL;
        PROPVARIANT prop = { 0 }, name = { 0 };
        if ((_archive->GetProperty(index, kpidSize, &prop) == S_OK) && (_archive->GetProperty(index, kpidPath, &name) == S_OK)) {
            item = new CLzma::Item((name.vt == VT_BSTR) ? name.bstrVal : NULL, index);
            item->size = CLzma::PROPVARIANTGetUInt64(&prop);
            
            memset(&prop, 0, sizeof(PROPVARIANT));
            if (_archive->GetProperty(index, kpidSize, &prop) == S_OK) {
                item->packSize = CLzma::PROPVARIANTGetUInt64(&prop);
            }
            
            memset(&prop, 0, sizeof(PROPVARIANT));
            if ((_archive->GetProperty(index, kpidCTime, &prop) == S_OK) && (prop.vt == VT_FILETIME)) {
                item->creationTime = CLzma::FILETIMEToUnixTime(prop.filetime);
            }
            
            memset(&prop, 0, sizeof(PROPVARIANT));
            if ((_archive->GetProperty(index, kpidATime, &prop) == S_OK) && (prop.vt == VT_FILETIME)) {
                item->accessTime = CLzma::FILETIMEToUnixTime(prop.filetime);
            }
            
            memset(&prop, 0, sizeof(PROPVARIANT));
            if ((_archive->GetProperty(index, kpidMTime, &prop) == S_OK) && (prop.vt == VT_FILETIME)) {
                item->modificationTime = CLzma::FILETIMEToUnixTime(prop.filetime);
            }
            
            memset(&prop, 0, sizeof(PROPVARIANT));
            if (_archive->GetProperty(index, kpidCRC, &prop) == S_OK) {
                item->crc32 = (uint32_t)CLzma::PROPVARIANTGetUInt64(&prop);
            }
            
            memset(&prop, 0, sizeof(PROPVARIANT));
            if ((_archive->GetProperty(index, kpidEncrypted, &prop) == S_OK) && (prop.vt == VT_BOOL) && prop.boolVal) {
                item->isEncrypted = true;
            }
            
            memset(&prop, 0, sizeof(PROPVARIANT));
            if ((_archive->GetProperty(index, kpidIsDir, &prop) == S_OK) && (prop.vt == VT_BOOL) && prop.boolVal) {
                item->isDir = true;
            }
        }
        NWindows::NCOM::PropVariant_Clear(&name);
        return item;
    }
    
    bool Reader::extract(const char * path /* = NULL */ , bool isWithFullPaths /* = false */ ) {
        this->cleanExtractCallbackRef();
        this->clearLastError();
        
        CLzma::ExtractCallback2 * extractCallback = new CLzma::ExtractCallback2(_archive, this);
        
        int32_t mode = NArchive::NExtract::NAskMode::kSkip;
        if (path) {
            if (extractCallback->prepare(path, isWithFullPaths)) {
                mode = NArchive::NExtract::NAskMode::kExtract;
            } else {
                this->setLastError(extractCallback);
                delete extractCallback;
                return false;
            }
        } else {
            mode = NArchive::NExtract::NAskMode::kTest;
        }
        extractCallback->setMode(mode);
        
        size_t processed = 0;
        const size_t maxIndicies = 1;
        uint32_t indices[maxIndicies];
        bool extracting = true;
        while (extracting) {
            uint32_t count = 0;
            while (count < maxIndicies && processed < _itemsCount) {
                indices[count++] = processed++;
            }
            CMyComPtr<CLzma::ExtractCallback2> extractCallbackHolder(extractCallback);
            const HRESULT result = (count > 0) ? _archive->Extract(indices, count, mode, extractCallback) : S_OK;
            if (result != S_OK) {
                this->setLastError(extractCallback);
                return false;
            }
            extracting = (processed < _itemsCount);
        }
        
        return true;
    }
    
    bool Reader::extract(const uint32_t * itemsIndices,
                         const uint32_t itemsCount,
                         const char * path /* = NULL */,
                         bool isWithFullPaths /* = false */) {
		this->cleanExtractCallbackRef();
		this->clearLastError();
		
        CLzma::ExtractCallback2 * extractCallback = new CLzma::ExtractCallback2(_archive, this);
//        CLzma::ExtractCallback3 * extractCallback = new CLzma::ExtractCallback3(_archive, this);
		int32_t mode = NArchive::NExtract::NAskMode::kSkip;
		if (path) {
            if (extractCallback->prepare(path, isWithFullPaths)) {
				mode = NArchive::NExtract::NAskMode::kExtract;
			} else {
                this->setLastError(extractCallback);
                delete extractCallback;
				return false;
			}
		} else {
			mode = NArchive::NExtract::NAskMode::kTest;
		}
        extractCallback->setMode(mode);

		const HRESULT result = _archive->Extract(itemsIndices, itemsCount, mode, extractCallback);
		if (result != S_OK) {
            this->setLastError(extractCallback);
			return false;
		}

		return true;
	}
    
    bool Reader::readIteratorProperty(PROPVARIANT * property, const uint32_t identifier, const uint32_t index) {
        CLZMA_ASSERT(index < _itemsCount)
        return _archive->GetProperty(index, identifier, property) == S_OK;
    }
    
	bool Reader::prepare(const int type) {
		this->clearLastError();
		this->createObject(type, &IID_IInArchive, (void **)&_archive);
		return (_archive != NULL && this->lastError() == NULL);
	}

	bool Reader::openFile(const char * path) {
		this->cleanOpenCallbackRef();
		this->cleanExtractCallbackRef();
		this->clearLastError();
		
		CLzma::InFile * inFile = new CLzma::InFile();
		if (!inFile) {
			this->setLastError(-1, __LINE__, __FILE__, "Can't open file for reading: [%s]", path);
			return false;
		}

		_inFile = inFile;

		_openCallbackRef = new CLzma::OpenCallback();
		_openCallback = CMyComPtr<IArchiveOpenCallback>(_openCallbackRef);
		if (!_openCallbackRef) {
			this->setLastError(-1, __LINE__, __FILE__, "Can't create open callback");
			return false;
		}

		_openCallbackRef->setCoder(this);

		if (!inFile->open(path)) {
			this->setLastError(-1, __LINE__, __FILE__, "Can't open file for reading: [%s]", path);
			this->setLastErrorReason("- File not exists.\n"
									 "- File have no read permissions for the current user.");
			return false;
		}

		HRESULT res = _archive->Open(_inFile, 0, _openCallback);
		if (res == S_OK) {
			UInt32 numItems = 0;
			res = _archive->GetNumberOfItems(&numItems);
			if (res != S_OK) {
				this->setLastError(res, __LINE__, __FILE__, "Can't receive number of archive items with result: %lli", (long long)res);
				return false;
			}
			_itemsCount = numItems;
			return true;
		}

		this->setLastError(res, __LINE__, __FILE__, "Can't open archive file with result: %lli", (long long)res);
		return false;
	}

    uint32_t Reader::itemsCount() const {
        return _itemsCount;
    }
    
    void Reader::setReader(clzma_reader_t reader) {
        _reader = reader;
    }
    
    void Reader::setProgressCallback(void (*cb)(clzma_reader_t reader, const double progress)) {
        _progressCallback = cb;
    }
    
    void Reader::onProgress(const double progress) {
        if (_reader && _progressCallback) {
            _progressCallback(_reader, progress);
        }
    }
    
	Reader::Reader() : CLzma::BaseCoder(),
        _reader(NULL),
        _progressCallback(NULL),
		_openCallbackRef(NULL),
		_extractCallbackRef(NULL),
		_itemsCount(0) {

	}

	void Reader::cleanOpenCallbackRef() {
		CMyComPtr<IArchiveOpenCallback> op = _openCallback;
		if (op != NULL && _openCallbackRef) {
			_openCallbackRef->setCoder(NULL);
		}

		_openCallbackRef = NULL;
		_openCallback.Release();
	}

	void Reader::cleanExtractCallbackRef() {
		CMyComPtr<IArchiveExtractCallback> ep = _extractCallback;
		if (ep != NULL && _extractCallbackRef) {
			_extractCallbackRef->setCoder(NULL);
		}

		_extractCallbackRef = NULL;
		_extractCallback.Release();
	}

	Reader::~Reader() {
		this->cleanOpenCallbackRef();
		this->cleanExtractCallbackRef();
	}

}


clzma_reader_t clzma_reader_create(void) {
    CLzma::Reader * reader = new CLzma::Reader();
    CLZMA_ASSERT(reader)
    return reader;
}

uint32_t clzma_reader_get_items_count(clzma_reader_t reader) {
    CLZMA_ASSERT(reader)
    return ((CLzma::Reader *)reader)->itemsCount();
}

clzma_item_t clzma_reader_get_item_at_index(clzma_reader_t reader, const uint32_t index) {
    CLZMA_ASSERT(reader)
    return ((CLzma::Reader *)reader)->itemAtIndex(index);
}

int clzma_reader_extract_items(clzma_reader_t reader, const clzma_item_t * items, const uint32_t items_count, const clzma_char_t * path, const int is_with_full_paths) {
    CLZMA_ASSERT(reader)
    CLZMA_ASSERT(items)
    
    const size_t indicesMemSize = sizeof(uint32_t) * items_count;
    uint32_t * indices = (uint32_t *)clzma_malloc(indicesMemSize);
    if (indices) {
        for (uint32_t i = 0; i < items_count; i++) {
            CLzma::Item * item = (CLzma::Item *)items[i];
            CLZMA_ASSERT(item)
            indices[i] = item->index();
        }
        if (((CLzma::Reader *)reader)->isSolidArchive()) {
            qsort(indices, items_count, sizeof(uint32_t), CLzma::compareIndices<uint32_t>);
        }
        const int is_ok = clzma_reader_extract_item_indices(reader, indices, items_count, path, is_with_full_paths);
        clzma_free(indices);
        return is_ok;
    }
    ((CLzma::Reader *)reader)->setLastError(-1, __LINE__, __FILE__, "Can't allocate enough memory for items indexes: [%llu] bytes", (unsigned long long)indicesMemSize);
    return CLZMA_FALSE;
}

int clzma_reader_extract_item_indices(clzma_reader_t reader, const uint32_t * items_indices, const uint32_t items_count, const clzma_char_t * path, const int is_with_full_paths) {
    CLZMA_ASSERT(reader)
    return ((CLzma::Reader *)reader)->extract(items_indices, items_count, path, is_with_full_paths ? true : false) ? CLZMA_TRUE : CLZMA_FALSE;
}

int clzma_reader_extract_all_items(clzma_reader_t reader, const clzma_char_t * path, const int is_with_full_paths) {
    CLZMA_ASSERT(reader)
    return ((CLzma::Reader *)reader)->extract(path, is_with_full_paths ? true : false) ? CLZMA_TRUE : CLZMA_FALSE;
}

int clzma_reader_open_file_path(clzma_reader_t reader, const clzma_char_t * file_path, const int32_t type) {
    CLZMA_ASSERT(reader)
    CLzma::Reader * r = (CLzma::Reader *)reader;
    if (r->prepare(type)) {
        return r->openFile(file_path) ? CLZMA_TRUE : CLZMA_FALSE;
    }
    return CLZMA_FALSE;
}

void clzma_reader_set_user_object(clzma_reader_t reader, void * user_object) {
    CLZMA_ASSERT(reader)
    ((CLzma::Reader *)reader)->setUserObject(user_object);
}

void * clzma_reader_get_user_object(clzma_reader_t reader) {
    CLZMA_ASSERT(reader)
    return ((CLzma::Reader *)reader)->userObject();
}

clzma_error_t clzma_reader_get_last_error(clzma_reader_t reader) {
    CLZMA_ASSERT(reader)
    return ((CLzma::Reader *)reader)->lastError();
}

void clzma_reader_set_progress_callback(clzma_reader_t reader, void (*cb)(clzma_reader_t reader, const double progress)) {
    CLZMA_ASSERT(reader)
    ((CLzma::Reader *)reader)->setProgressCallback(cb);
}

void clzma_reader_set_password(clzma_reader_t reader, const clzma_wchar_t * password) {
    CLZMA_ASSERT(reader)
    ((CLzma::Reader *)reader)->setPassword(password);
}

void clzma_reader_delete(clzma_reader_t reader) {
    CLZMA_ASSERT(reader)
    CLzma::Reader * r = (CLzma::Reader *)reader;
    delete r;
}

