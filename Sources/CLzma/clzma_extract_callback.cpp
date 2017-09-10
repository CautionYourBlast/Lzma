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


#include "clzma_extract_callback.h"
#include "clzma_common.h"

#include "CPP/Common/Defs.h"
#include "CPP/Windows/PropVariant.h"
#include "CPP/7zip/Archive/Common/DummyOutStream.h"

namespace CLzma {
	
	STDMETHODIMP ExtractCallback::ReportExtractResult(UInt32 indexType, UInt32 index, Int32 opRes) {
		return S_OK;
	}

	STDMETHODIMP ExtractCallback::SetRatioInfo(const UInt64 *inSize, const UInt64 *outSize) {
		return S_OK;
	}

	STDMETHODIMP ExtractCallback::SetTotal(UInt64 size) {
		_total = size;
        if (_coder) {
            _coder->onProgress(0);
        }
		return S_OK;
	}

	STDMETHODIMP ExtractCallback::SetCompleted(const UInt64 * completeValue) {
		if (completeValue && _coder) {
			const long double complete = *completeValue;
			const double progress = (_total > 0) ? (double)(complete / _total) : 0;
            _coder->onProgress(progress);
		}
		return S_OK;
	}

	HRESULT ExtractCallback::getTestStream(uint32_t index, ISequentialOutStream ** outStream) {
		CDummyOutStream * dummy = new CDummyOutStream();
		if (!dummy) {
            this->setLastError(E_ABORT, __LINE__, __FILE__, "Can't create out test stream");
			return E_ABORT;
		}

		CMyComPtr<ISequentialOutStream> outStreamLoc = dummy;

		_outFileStream = outStreamLoc;
		*outStream = outStreamLoc.Detach();

		return S_OK;
	}

	HRESULT ExtractCallback::getExtractStream(uint32_t index, ISequentialOutStream ** outStream) {
		PROPVARIANT pathProp;
        memset(&pathProp, 0, sizeof(PROPVARIANT));
		if (_archive->GetProperty(index, kpidPath, &pathProp) != S_OK) {
			this->setLastError(E_ABORT, __LINE__, __FILE__, "Can't read path property by index: %i", (int)index);
			return E_ABORT;
		}
        
        const size_t pathLen = pathProp.bstrVal ? wcslen(pathProp.bstrVal) : 0;
        if (pathLen == 0) {
            return S_OK;
        }
        
        PROPVARIANT isDirProp;
        memset(&isDirProp, 0, sizeof(PROPVARIANT));
        HRESULT res = _archive->GetProperty(index, kpidIsDir, &isDirProp);
        if (res != S_OK) {
            this->setLastError(res, __LINE__, __FILE__, "Can't get property of the item by index: %u", (unsigned int)index);
            return res;
        }
        
        clzma_wchars_to_chars_callback cb = clzma_common_get_wchars_to_chars_callback();
        char * cpath = (char *)clzma_malloc((pathLen + 1) * sizeof(clzma_wchar_t));
        cb(pathProp.bstrVal, pathLen, cpath);
        
        CLzma::Path pullPath(_dstPath->Ptr());
        pullPath.appendPath(cpath);
        
        clzma_free(cpath);
        
        if (CLzma::PROPVARIANTGetBool(&isDirProp)) {
            if (pullPath.createDir(true)) {
                return S_OK;
            } else {
                this->setLastError(E_ABORT, __LINE__, __FILE__, "Can't create item directory by index: %u", (unsigned int)index);
                return E_ABORT;
            }
        }
        
        CLzma::Path pullPathWithoutFileName(pullPath.Ptr());
        pullPathWithoutFileName.deleteLastPathComponent();
        
        bool isDir = false;
        if (pullPathWithoutFileName.exists(&isDir)) {
            if (!isDir) {
                this->setLastError(E_ABORT, __LINE__, __FILE__, "Subpath for item by index: %u exists as file: %a", (unsigned int)index, pullPathWithoutFileName.Ptr());
                return E_ABORT;
            }
        } else if (!pullPathWithoutFileName.createDir(true)) {
            this->setLastError(E_ABORT, __LINE__, __FILE__, "Can't create item directory by index: %u", (unsigned int)index);
            return E_ABORT;
        }
        
        CLzma::OutFile * outFile = new CLzma::OutFile();
        if (!outFile) {
            this->setLastError(E_ABORT, __LINE__, __FILE__, "Can't create out file stream");
            return E_ABORT;
        }
        
        if (!outFile->open(pullPath.Ptr())) {
            delete outFile;
            this->setLastError(E_ABORT, __LINE__, __FILE__, "Can't open destination for write: [%s]", pullPath.Ptr());
            return E_ABORT;
        }
        
        _outFileStreamRef = outFile;
        CMyComPtr<ISequentialOutStream> outStreamLoc = _outFileStreamRef;
        
        _outFileStream = outStreamLoc;
        *outStream = outStreamLoc.Detach();

		return S_OK;
	}

    STDMETHODIMP ExtractCallback::GetStream(UInt32 index,
                                             ISequentialOutStream ** outStream,
                                             Int32 askExtractMode) {
		*outStream = NULL;
		_outFileStream.Release();
		_outFileStreamRef = NULL;

		if (!_archive) {
			this->setLastError(E_ABORT, __LINE__, __FILE__, "No input archive");
			return E_ABORT;
		}

		switch (_mode) {
			case NArchive::NExtract::NAskMode::kExtract:
				return this->getExtractStream(index, outStream);
				break;

			case NArchive::NExtract::NAskMode::kTest:
				return this->getTestStream(index, outStream);
				break;

			case NArchive::NExtract::NAskMode::kSkip:
				return S_OK;
				break;
		}

		return E_FAIL;
	}

	STDMETHODIMP ExtractCallback::PrepareOperation(Int32 askExtractMode) {
		return S_OK;
	}

	STDMETHODIMP ExtractCallback::SetOperationResult(Int32 operationResult) {
		HRESULT res = E_FAIL;
		switch (operationResult) {
			case NArchive::NExtract::NOperationResult::kOK:
				res = S_OK;
				break;

			case NArchive::NExtract::NOperationResult::kUnsupportedMethod:
				this->setLastError(operationResult, __LINE__, __FILE__, "kUnsupportedMethod");
				break;

			case NArchive::NExtract::NOperationResult::kCRCError:
				this->setLastError(operationResult, __LINE__, __FILE__, "kCRCError");
				break;

			case NArchive::NExtract::NOperationResult::kDataError:
				this->setLastError(operationResult, __LINE__, __FILE__, "kDataError");
				break;

			case NArchive::NExtract::NOperationResult::kUnavailable:
				this->setLastError(operationResult, __LINE__, __FILE__, "kUnavailable");
				break;

			case NArchive::NExtract::NOperationResult::kUnexpectedEnd:
				this->setLastError(operationResult, __LINE__, __FILE__, "kUnexpectedEnd");
				break;

			case NArchive::NExtract::NOperationResult::kDataAfterEnd:
				this->setLastError(operationResult, __LINE__, __FILE__, "kDataAfterEnd");
				break;

			case NArchive::NExtract::NOperationResult::kIsNotArc:
				this->setLastError(operationResult, __LINE__, __FILE__, "kIsNotArc");
				break;

			case NArchive::NExtract::NOperationResult::kHeadersError:
				this->setLastError(operationResult, __LINE__, __FILE__, "kHeadersError");
				break;

			default:
				break;
		}

		if (_outFileStream != NULL) {
            if (_outFileStreamRef) {
                _outFileStreamRef->close();
            }
		}
		_outFileStream.Release();
		_outFileStreamRef = NULL;
		return res;
	}

	STDMETHODIMP ExtractCallback::CryptoGetTextPassword(BSTR *password) {
		if (_coder) {
//			UString w(_coder->onGetVoidCallback1());
//			if (w.Len() > 0) return StringToBstr(w, password);
		}
		return S_OK;
	}

	STDMETHODIMP ExtractCallback::CryptoGetTextPassword2(Int32 * passwordIsDefined, BSTR * password) {
		if (passwordIsDefined) *passwordIsDefined = BoolToInt(false);
		if (_coder) {
//			UString w(_coder->onGetVoidCallback1());
//			if (w.Len() > 0) {
//				if (passwordIsDefined) *passwordIsDefined = BoolToInt(true);
//				return StringToBstr(w, password);
//			}
		}
		return S_OK;
	}

	bool ExtractCallback::prepare(const char * extractPath, bool isFullPath) {
        if (_dstPath) {
            delete _dstPath;
            _dstPath = NULL;
        }
		
        CLzma::Path * path = new CLzma::Path(extractPath);
		_isFullPath = isFullPath;

        bool isDir = false;
        if (path->exists(&isDir)) {
            if (!isDir) {
                delete path;
                this->setLastError(-1, __LINE__, __FILE__, "Extract path: [%s] exists and it's file", extractPath);
                return false;
            }
        } if (!path->createDir(true)) {
            delete path;
            this->setLastError(-1, __LINE__, __FILE__, "Can't create directory path: [%s]", path->Ptr());
            return false;
        }
        
        _dstPath = path;
		return true;
	}

	ExtractCallback::ExtractCallback() : CLzma::LastErrorHolder(),
		_outFileStreamRef(NULL),
		_coder(NULL),
		_archive(NULL),
        _dstPath(NULL),
		_total(0),
		_mode(NArchive::NExtract::NAskMode::kSkip),
		_isFullPath(false) {

	}

	ExtractCallback::~ExtractCallback() {
        if (_dstPath) {
            delete _dstPath;
        }
	}
	
}

