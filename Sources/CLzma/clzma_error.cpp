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


#include "clzma_error.h"
#include "clzma_private.h"

namespace CLzma {
	
	Error::Error() : code(-1), line(-1) {

	}

	void LastErrorHolder::clearLastError() {
		if (_lastError) {
			delete _lastError;
			_lastError = NULL;
		}
	}

	void LastErrorHolder::setLastError(CLzma::LastErrorHolder * holder) {
		this->clearLastError();
		if (!holder) return;
		if (holder->_lastError) {
            _lastError = new CLzma::Error();
            assert(_lastError);
            _lastError->file = holder->_lastError->file;
            _lastError->line = holder->_lastError->line;
            _lastError->code = holder->_lastError->code;
            _lastError->description = holder->_lastError->description;
            _lastError->possibleReason = holder->_lastError->possibleReason;
		}
	}

	void LastErrorHolder::setLastError(int64_t code, int line, const char * file, const char * format, ...) {
		this->clearLastError();
        _lastError = new CLzma::Error();
        assert(_lastError);
        _lastError->code = code;
        _lastError->line = line;
        if (file) _lastError->file = file;
        if (format) {
            va_list args;
            va_start(args, format);
            char buff[1024];
            vsprintf(buff, format, args);
            va_end(args);
            _lastError->description = buff;
            
            CLZMA_DEBUG_LOG("ERROR: code = %lli, file = \'%s\', line = %i, description = %s", code, file, line, buff)
        }
	}

	void LastErrorHolder::setLastErrorReason(const char * format, ...) {
		if (_lastError && format) {
			va_list args;
			va_start(args, format);
			char buff[1024];
			vsprintf(buff, format, args);
			va_end(args);
			_lastError->possibleReason = buff;
			CLZMA_DEBUG_LOG("ERROR REASON: %s", buff)
		}
	}

	LastErrorHolder::LastErrorHolder() : _lastError(NULL) {

	}

	LastErrorHolder::~LastErrorHolder() {
        if (_lastError) {
            delete _lastError;
        }
	}
}

const clzma_char_t * clzma_error_get_description(clzma_error_t error) {
    CLZMA_ASSERT(error)
    CLzma::Error * e = (CLzma::Error *)error;
    const size_t len = e->description.Len();
    return len > 0 ? e->description.Ptr() : NULL;
}

const clzma_char_t * clzma_error_get_possible_reason(clzma_error_t error) {
    CLZMA_ASSERT(error)
    CLzma::Error * e = (CLzma::Error *)error;
    const size_t len = e->possibleReason.Len();
    return len > 0 ? e->possibleReason.Ptr() : NULL;
}

const clzma_char_t * clzma_error_get_file(clzma_error_t error) {
    CLZMA_ASSERT(error)
    CLzma::Error * e = (CLzma::Error *)error;
    const size_t len = e->file.Len();
    return len > 0 ? e->file.Ptr() : NULL;
}

int64_t clzma_error_get_code(clzma_error_t error) {
    CLZMA_ASSERT(error)
    return ((CLzma::Error *)error)->code;
}

int32_t clzma_error_get_line(clzma_error_t error) {
    CLZMA_ASSERT(error)
    return ((CLzma::Error *)error)->line;
}
