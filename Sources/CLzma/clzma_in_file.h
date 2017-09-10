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


#ifndef __CLZMA_IN_FILE_H__
#define __CLZMA_IN_FILE_H__ 1

#include "clzma_private.h"

#include "CPP/Common/MyCom.h"
#include "CPP/Common/MyString.h"
#include "CPP/7zip/Common/FileStreams.h"

namespace CLzma {
	
	class InFile : public IInStream, public CMyUnknownImp {
	private:
		FILE * _f;
		
	public:
		MY_UNKNOWN_IMP1(IInStream)
		
		STDMETHOD(Seek)(Int64 offset, UInt32 seekOrigin, UInt64 *newPosition);
		STDMETHOD(Read)(void *data, UInt32 size, UInt32 *processedSize);

		bool open(const char * p);
		void close();

		InFile();
		virtual ~InFile();
	};
}

#endif
