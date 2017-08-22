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


#ifndef __LZMA_WRP_FILE_DECODER_H__
#define __LZMA_WRP_FILE_DECODER_H__ 1

#include "lzma_wrp_base_coder.h"
#include "lzma_wrp_open_callback.h"
#include "lzma_wrp_extract_callback.h"

#include "CPP/7zip/Archive/IArchive.h"
#include "CPP/7zip/IPassword.h"
#include "CPP/Common/MyCom.h"
#include "CPP/Common/MyString.h"
#include "CPP/Windows/PropVariant.h"

namespace LzmaWRP {
	
	class FileDecoder : public LzmaWRP::BaseCoder {
	private:
        lzma_wrp_reader _reader;
        void (*_progressCallback)(lzma_wrp_reader reader, const double progress);
		LzmaWRP::OpenCallback * _openCallbackRef;
		LzmaWRP::ExtractCallback * _extractCallbackRef;

		CMyComPtr<IInArchive> _archive;
		CMyComPtr<IInStream> _inFile;
		CMyComPtr<IArchiveOpenCallback> _openCallback;
		CMyComPtr<IArchiveExtractCallback> _extractCallback;

		uint32_t _itemsCount;

		void cleanOpenCallbackRef();
		void cleanExtractCallbackRef();

	public:
        void setReader(lzma_wrp_reader reader);
        void setProgressCallback(void (*cb)(lzma_wrp_reader reader, const double progress));
        
        virtual void onProgress(const double progress);
        
        uint32_t itemsCount() const;
        
        bool readIteratorProperty(PROPVARIANT * property, const uint32_t identifier, const uint32_t index);
        
		bool process(const uint32_t * itemsIndices,
					 const uint32_t itemsCount,
					 const char * path = NULL,
					 bool isWithFullPaths = false);

		// Required section, `lzma_wrp::base_coder`
		// find codec, create encode/decode object and check error.
		virtual bool prepare(const int type);

		virtual bool openFile(const char * path);

		FileDecoder();
		virtual ~FileDecoder();
	};
}

#endif 
