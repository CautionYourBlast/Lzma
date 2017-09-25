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


#include "clzma_file_encoder.h"
#include "clzma_update_callback.h"

#include "CPP/7zip/Archive/DllExports2.h" // custom header with `STDAPI CreateObject(const GUID *clsid, const GUID *iid, void **outObject);`
#include "CPP/7zip/Archive/IArchive.h"
#include "CPP/7zip/IPassword.h"

namespace CLzma {
    
    void FileEncoder::cleanUpdateCallbackRef() {
        CMyComPtr<IArchiveUpdateCallback2> updateCallback = _updateCallback;
        if (updateCallback != NULL && _updateCallbackRef) {
            _updateCallbackRef->coder = NULL;
        }
        
        _updateCallbackRef = NULL;
        _updateCallback.Release();
    }
    
    void FileEncoder::cleanOutFileStreamRef() {
        CMyComPtr<IOutStream> outFileStream = _outFileStream;
        if (outFileStream != NULL && _outFileStreamRef) {
            _outFileStreamRef->Close();
        }
        
        _outFileStreamRef = NULL;
        _outFileStream.Release();
    }
    
    bool FileEncoder::requiredCallback1() const {
        return settingsValue(CLZMA_ENC_ENC_CONTENT) || settingsValue(CLZMA_ENC_ENC_HEADER);
    }
    
    bool FileEncoder::prepare(const int type) {
        this->clearLastError();
        if (type != CLZMA_FILE_TYPE_7Z) {
            this->setLastError(E_ABORT, __LINE__, __FILE__, "Unsupported encoding type: %i", (int)type);
            return false;
        }
        this->createObject(type, &IID_IOutArchive, (void **)&_archive);
        return (_archive != NULL && this->lastError() == NULL);
    }
    
#define LZMAOBJC_SETTINGS_COUNT 9
    
    void FileEncoder::upplySettings() {
        const wchar_t * names[LZMAOBJC_SETTINGS_COUNT] = {
            L"0",		// method
            L"s",		// solid
            L"x",		// compression level
            L"hc",		// compress header
            L"he",		// encode header
            L"tc",		// write creation time
            L"ta",		// write access time
            L"tm",		// write modification time
            
            L"hcf"		// compress header full, true - add, false - don't add/ignore
        };
        NWindows::NCOM::CPropVariant values[LZMAOBJC_SETTINGS_COUNT] = {
            (UInt32)0,									// method dummy value
            settingsValue(CLZMA_ENC_SOLID),			// solid mode ON
            (UInt32)compressionLevel,					// compression level = 9 - ultra
            settingsValue(CLZMA_ENC_COMPR_HDR),		// compress header
            settingsValue(CLZMA_ENC_ENC_HEADER),		// encode header
            settingsValue(CLZMA_ENC_WRITE_CTIME),	// write creation time
            settingsValue(CLZMA_ENC_WRITE_ATIME),	// write access time
            settingsValue(CLZMA_ENC_WRITE_MTIME),	// write modification time
            
            true										// compress header full, true - add, false - don't add/ignore
        };
        
        switch (method) {
            case CLZMA_METHOD_LZMA: values[0] = L"LZMA"; break;
            case CLZMA_METHOD_LZMA2: values[0] = L"LZMA2"; break;
            default: break;
        }
        
        CMyComPtr<ISetProperties> setProperties;
        _archive->QueryInterface(IID_ISetProperties, (void **)&setProperties);
        if (setProperties) {
            const bool compressHeaderFull = settingsValue(CLZMA_ENC_COMPR_HDR_FULL);
            setProperties->SetProperties(names, values, compressHeaderFull ? LZMAOBJC_SETTINGS_COUNT : LZMAOBJC_SETTINGS_COUNT - 1);
        }
    }
    
    bool FileEncoder::openFile(const char * path) {
        this->cleanOutFileStreamRef();
        this->cleanUpdateCallbackRef();
        this->clearLastError();
        
        _outFileStreamRef = new COutFileStream();
        if (!_outFileStreamRef) {
            this->setLastError(E_ABORT, __LINE__, __FILE__, "Can't create archive out stream object for path: %s", path);
            return false;
        }
        
        _outFileStream = CMyComPtr<IOutStream>(_outFileStreamRef);
        if (!_outFileStreamRef->Create(path, false)) {
            this->setLastError(E_ABORT, __LINE__, __FILE__, "Can't create archive out file for path: %s", path);
            return false;
        }
        
        _updateCallbackRef = new CLzma::UpdateCallback();
        if (!_updateCallbackRef) {
            this->setLastError(E_ABORT, __LINE__, __FILE__, "Can't create archive update callback object for path: %s", path);
            return false;
        }
        
        _updateCallback = CMyComPtr<IArchiveUpdateCallback2>(_updateCallbackRef);
        this->upplySettings();
        
        return true;
    }
    
    bool FileEncoder::encodeItems(void * items, const uint32_t numItems) {
        this->clearLastError();
        if (_updateCallbackRef) {
            _updateCallbackRef->clearLastError();
            _updateCallbackRef->items = items;
            _updateCallbackRef->coder = this;
            const HRESULT result = _archive->UpdateItems(_outFileStream, numItems, _updateCallback);
            if (result == S_OK) {
                return true;
            }
            this->setLastError(_updateCallbackRef);
        }
        return S_FALSE;
    }
    
    void FileEncoder::setWriter(clzma_writer_t writer) {
        _writer = writer;
    }
    
    void FileEncoder::setProgressCallback(void (*cb)(clzma_writer_t writer, const double progress)) {
        _progressCallback = cb;
    }
    
    void FileEncoder::onProgress(const double progress) {
        if (_writer && _progressCallback) {
            _progressCallback(_writer, progress);
        }
    }
    
    FileEncoder::FileEncoder() : CLzma::BaseCoder(),
        _writer(NULL),
        _progressCallback(NULL),
        _updateCallbackRef(NULL),
        _outFileStreamRef(NULL),
        _settings(0),
        compressionLevel(7),
        method(CLZMA_METHOD_LZMA2) {
            
        setSettingsValue(true, CLZMA_ENC_SOLID);
        setSettingsValue(true, CLZMA_ENC_COMPR_HDR);
        setSettingsValue(true, CLZMA_ENC_COMPR_HDR_FULL);
        setSettingsValue(true, CLZMA_ENC_WRITE_CTIME);
        setSettingsValue(true, CLZMA_ENC_WRITE_ATIME);
        setSettingsValue(true, CLZMA_ENC_WRITE_MTIME);
    }
    
    FileEncoder::~FileEncoder() {
        this->cleanOutFileStreamRef();
        this->cleanUpdateCallbackRef();
    }
    
    // Settings
    void FileEncoder::setSettingsValue(const bool value, const unsigned char flag) {
        if (value)  {
            _settings |= flag;
        } else {
            _settings &= ~flag;
        }
    }
    
    bool FileEncoder::settingsValue(const unsigned char flag) const {
        return (_settings & flag) ? true : false;
    }
}
