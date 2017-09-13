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


#ifndef __CLZMA_BASE_EXTRACT_CALLBACK_H__
#define __CLZMA_BASE_EXTRACT_CALLBACK_H__ 1

#include "clzma_base_coder.h"
#include "clzma_out_streams.h"
#include "clzma_path.h"

#include "CPP/7zip/Archive/IArchive.h"
#include "CPP/7zip/IPassword.h"
#include "CPP/7zip/ICoder.h"
#include "CPP/Common/MyCom.h"
#include "CPP/Common/MyString.h"
#include "CPP/7zip/Common/FileStreams.h"

namespace CLzma {
    
    class BaseExtractCallback :
        public IArchiveExtractCallback,
        public IArchiveExtractCallbackMessage,
        public ICryptoGetTextPassword,
        public ICryptoGetTextPassword2,
        public ICompressProgressInfo,
        public CMyUnknownImp,
        public CLzma::LastErrorHolder {
    private:
        CMyComPtr<CLzma::BaseOutStream> _outStream;
    protected:
        IInArchive * _archive;
        CLzma::BaseCoder * _coder;
        uint32_t * _indices;
        uint64_t _total;
        size_t _indicesCount;
        int32_t _mode;
        
        virtual HRESULT createExtractStreamAtIndex(const uint32_t index, CLzma::BaseOutStream ** outStream);
    public:
        MY_UNKNOWN_IMP4(IArchiveExtractCallbackMessage, ICryptoGetTextPassword, ICryptoGetTextPassword2, ICompressProgressInfo)
        
        INTERFACE_IArchiveExtractCallback(;)
        INTERFACE_IArchiveExtractCallbackMessage(;)
        
        STDMETHOD(SetRatioInfo)(const UInt64 * inSize, const UInt64 * outSize);
        
        // ICryptoGetTextPassword
        STDMETHOD(CryptoGetTextPassword)(BSTR * password);
        
        // ICryptoGetTextPassword2
        STDMETHOD(CryptoGetTextPassword2)(Int32 * passwordIsDefined, BSTR * password);
        
        CMyComPtr<CLzma::BaseOutStream> outStream() const;
        void setIndices(uint32_t * indices, const size_t indicesCount);
        void setMode(int32_t mode);
        
        BaseExtractCallback(IInArchive * archive, CLzma::BaseCoder * coder);
        virtual ~BaseExtractCallback();
    };
    
}

#endif
