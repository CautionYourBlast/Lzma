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


#include "clzma_file_decoder.h"
#include "clzma_common.h"
#include "clzma_in_file.h"
#include "clzma_open_callback.h"
#include "clzma_out_file.h"

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
    
    bool FileDecoder::process(const uint32_t * itemsIndices,
                              const uint32_t itemsCount,
                              const char * path /* = NULL */,
                              bool isWithFullPaths /* = false */) {
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

//		_extractCallbackRef->setCoder(this);
//		_extractCallbackRef->setArchive(_archive);
//		_extractCallbackRef->setMode(mode);

		const HRESULT result = _archive->Extract(itemsIndices, itemsCount, mode, extractCallback);
//		_extractCallbackRef->setArchive(NULL);

//		if (result != S_OK) {
//			this->setLastError(result, __LINE__, __FILE__, "Archive extract error with result: %lli", (long long)result);
//			return false;
//		}

		return true;
	}
    
    bool FileDecoder::readIteratorProperty(PROPVARIANT * property, const uint32_t identifier, const uint32_t index) {
        CLZMA_ASSERT(index < _itemsCount)
        return _archive->GetProperty(index, identifier, property) == S_OK;
    }
    
	bool FileDecoder::prepare(const int type) {
		this->clearLastError();
		this->createObject(type, &IID_IInArchive, (void **)&_archive);
		return (_archive != NULL && this->lastError() == NULL);
	}

	bool FileDecoder::openFile(const char * path) {
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

    uint32_t FileDecoder::itemsCount() const {
        return _itemsCount;
    }
    
    void FileDecoder::setReader(clzma_reader_t reader) {
        _reader = reader;
    }
    
    void FileDecoder::setProgressCallback(void (*cb)(clzma_reader_t reader, const double progress)) {
        _progressCallback = cb;
    }
    
    void FileDecoder::onProgress(const double progress) {
        if (_reader && _progressCallback) {
            _progressCallback(_reader, progress);
        }
    }
    
	FileDecoder::FileDecoder() : CLzma::BaseCoder(),
        _reader(NULL),
        _progressCallback(NULL),
		_openCallbackRef(NULL),
		_extractCallbackRef(NULL),
		_itemsCount(0) {

	}

	void FileDecoder::cleanOpenCallbackRef() {
		CMyComPtr<IArchiveOpenCallback> op = _openCallback;
		if (op != NULL && _openCallbackRef) {
			_openCallbackRef->setCoder(NULL);
		}

		_openCallbackRef = NULL;
		_openCallback.Release();
	}

	void FileDecoder::cleanExtractCallbackRef() {
		CMyComPtr<IArchiveExtractCallback> ep = _extractCallback;
		if (ep != NULL && _extractCallbackRef) {
			_extractCallbackRef->setCoder(NULL);
		}

		_extractCallbackRef = NULL;
		_extractCallback.Release();
	}

	FileDecoder::~FileDecoder() {
		this->cleanOpenCallbackRef();
		this->cleanExtractCallbackRef();
	}

}

