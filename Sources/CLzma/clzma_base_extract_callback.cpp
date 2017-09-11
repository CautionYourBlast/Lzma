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


#include "clzma_base_extract_callback.h"
#include "clzma_common.h"

#include "CPP/Common/Defs.h"
#include "CPP/Windows/PropVariant.h"
#include "CPP/7zip/Archive/Common/DummyOutStream.h"

namespace CLzma {
    
    HRESULT BaseExtractCallback::createTestStreamAtIndex(const uint32_t index, ISequentialOutStream ** outStream) {
        *outStream = new CDummyOutStream();
        return S_OK;
    }
    
    HRESULT BaseExtractCallback::createExtractStreamAtIndex(const uint32_t index, ISequentialOutStream ** outStream) {
        return S_OK;
    }
    
    STDMETHODIMP BaseExtractCallback::ReportExtractResult(UInt32 indexType, UInt32 index, Int32 opRes) {
        return S_OK;
    }
    
    STDMETHODIMP BaseExtractCallback::SetRatioInfo(const UInt64 * inSize, const UInt64 * outSize) {
        return S_OK;
    }
    
    STDMETHODIMP BaseExtractCallback::SetTotal(UInt64 size) {
        _total = size;
        if (_coder) {
            _coder->onProgress(0);
        }
        return S_OK;
    }
    
    STDMETHODIMP BaseExtractCallback::SetCompleted(const UInt64 * completeValue) {
        if (completeValue && _coder) {
            const long double complete = *completeValue;
            const double progress = (_total > 0) ? (double)(complete / _total) : 0;
            _coder->onProgress(progress);
        }
        return S_OK;
    }
    
    STDMETHODIMP BaseExtractCallback::GetStream(UInt32 index, ISequentialOutStream ** outStream, Int32 askExtractMode) {
        *outStream = NULL;
        HRESULT res = E_FAIL;
        switch (_mode) {
            case NArchive::NExtract::NAskMode::kExtract:
                res = this->createExtractStreamAtIndex(index, outStream);
                break;
            case NArchive::NExtract::NAskMode::kTest:
                res = this->createTestStreamAtIndex(index, outStream);
                break;
            case NArchive::NExtract::NAskMode::kSkip:
                return S_OK;
                break;
        }
        if (res != S_OK) {
            this->setLastError(E_ABORT, __LINE__, __FILE__, "Can't create extract or test stream");
            return res;
        }
        return S_OK;
    }
    
    STDMETHODIMP BaseExtractCallback::PrepareOperation(Int32 askExtractMode) {
        return S_OK;
    }
    
    STDMETHODIMP BaseExtractCallback::SetOperationResult(Int32 operationResult) {
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
        return res;
    }
    
    STDMETHODIMP BaseExtractCallback::CryptoGetTextPassword(BSTR * password) {
        const clzma_wchar_t * p = _coder ? _coder->password() : NULL;
        return p ? StringToBstr(p, password): S_OK;
    }
    
    STDMETHODIMP BaseExtractCallback::CryptoGetTextPassword2(Int32 * passwordIsDefined, BSTR * password) {
        const clzma_wchar_t * p = _coder ? _coder->password() : NULL;
        if (passwordIsDefined) {
            *passwordIsDefined = BoolToInt(p != NULL);
        }
        return p ? StringToBstr(p, password): S_OK;
    }
    
    void BaseExtractCallback::setMode(int32_t mode) {
        _mode = mode;
    }
    
    BaseExtractCallback::BaseExtractCallback(IInArchive * archive, CLzma::BaseCoder * coder) : CLzma::LastErrorHolder(),
        _archive(archive),
        _coder(coder),
        _total(0),
        _mode(NArchive::NExtract::NAskMode::kSkip) {
        
    }
    
    BaseExtractCallback::~BaseExtractCallback() {
        
    }
    
}

