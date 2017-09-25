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


#ifndef __CLZMA_FILE_ENCODER_H__
#define __CLZMA_FILE_ENCODER_H__ 1

#include "clzma_private.h"
#include "clzma_base_coder.h"

#include "CPP/7zip/Archive/IArchive.h"
#include "CPP/7zip/IPassword.h"
#include "CPP/7zip/Common/FileStreams.h"
#include "CPP/Common/MyCom.h"
#include "CPP/Common/MyString.h"
#include "CPP/Windows/PropVariant.h"

#define CLZMA_ENC_SOLID             (1 << 0)
#define CLZMA_ENC_COMPR_HDR         (1 << 1)
#define CLZMA_ENC_COMPR_HDR_FULL    (1 << 2)
#define CLZMA_ENC_ENC_CONTENT       (1 << 3)
#define CLZMA_ENC_ENC_HEADER        (1 << 4)
#define CLZMA_ENC_WRITE_CTIME       (1 << 5)
#define CLZMA_ENC_WRITE_MTIME       (1 << 6)
#define CLZMA_ENC_WRITE_ATIME       (1 << 7)

namespace CLzma {
	
	class UpdateCallback;

	class FileEncoder : public CLzma::BaseCoder {
	private:
        clzma_writer_t _writer;
        void (*_progressCallback)(clzma_writer_t writer, const double progress);
		CLzma::UpdateCallback * _updateCallbackRef;
		COutFileStream * _outFileStreamRef;
		
		CMyComPtr<IOutArchive> _archive;
		CMyComPtr<IArchiveUpdateCallback2> _updateCallback;
		CMyComPtr<IOutStream> _outFileStream;

		unsigned char _settings;

		void cleanUpdateCallbackRef();
		void cleanOutFileStreamRef();
		void upplySettings();
	public:
        void setWriter(clzma_writer_t writer);
        void setProgressCallback(void (*cb)(clzma_writer_t writer, const double progress));
        
        virtual void onProgress(const double progress);
        
		bool encodeItems(void * items, const uint32_t numItems);

		virtual bool requiredCallback1() const; // pwd.

		// Required section, `LzmaSDKObjC::BaseCoder`
		// find codec, create encode/decode object and check error.
		virtual bool prepare(const int type);

		virtual bool openFile(const char * path);

		// Properties
		void setSettingsValue(const bool value, const unsigned char flag);
		bool settingsValue(const unsigned char flag) const;
		unsigned char compressionLevel; //[1 .. 9]
		unsigned char method;

		FileEncoder();
		virtual ~FileEncoder();
	};
}

#endif
