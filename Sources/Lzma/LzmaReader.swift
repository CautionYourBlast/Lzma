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


import Foundation
import lzma_wrp

public protocol LzmaReaderDelegate: class {
    func onLzmaReader(reader: LzmaReader, progress: Double) -> Void
}

public final class LzmaReader: LzmaObject, Sequence {
    
    public private(set) var type: LzmaFileType {
        get {
            return LzmaFileType(rawValue: lzma_wrp_reader_get_type(reader))!
        }
        set {
            lzma_wrp_reader_set_type(reader, newValue.rawValue)
        }
    }
    
    public weak var delegate: LzmaReaderDelegate?
    
    public let url: URL
    
    public var count: UInt32 {
        return lzma_wrp_reader_get_items_count(reader)
    }
    
    public var error: LzmaError? {
        guard let error = lzma_wrp_reader_get_last_error(reader) else {
            return nil
        }
        return LzmaError(error: error);
    }
    
    private var _password: String?
    
    public var password: String? {
        get {
            return _password
        }
        set {
            if let passwd = newValue, let chars = passwd.wString {
                _password = newValue
                lzma_wrp_reader_set_password(reader, chars)
                chars.deinitialize()
            } else {
                _password = nil
                lzma_wrp_reader_set_password(reader, nil)
            }
        }
    }
    
    private let reader: lzma_wrp_reader = lzma_wrp_reader_create()
    
    private func setupReader() {
        let unmanagedSelf = lzma_wrp_handle(Unmanaged.passUnretained(self).toOpaque())
        lzma_wrp_reader_set_user_object(reader, unmanagedSelf)
        
        lzma_wrp_reader_set_progress_callback(reader) { unsafeReader, progress in
            guard let reader = unsafeReader, let userObject = lzma_wrp_reader_get_user_object(reader) else {
                return
            }
            
            let lzmaReader = Unmanaged<LzmaReader>.fromOpaque(userObject).takeUnretainedValue()
            if let delegate = lzmaReader.delegate {
                delegate.onLzmaReader(reader: lzmaReader, progress: progress)
            }
        }
    }
    
    public init(url: URL) {
        self.url = url
        super.init()
        setupReader()
    }
    
    public init(url: URL, type: LzmaFileType) {
        self.url = url
        super.init()
        
        self.type = type
        setupReader()
    }
    
    public func open() -> Bool {
        guard let path = url.path.cString(using: .utf8), lzma_wrp_reader_open_file_path(reader, path) != 0 else {
            return false
        }
        return true
    }
    
    public func extract(items: [LzmaItem], toPath path: String, usingFullPaths: Bool = true) -> Bool {
        if items.isEmpty {
            return true
        }
        let count = items.count
        guard let cPath = path.cString(using: .utf8) else {
            return false
        }
        
        let indices = UnsafeMutablePointer<UInt32>.allocate(capacity: count)
        var index = 0
        for item in items {
            indices[index] = item.index
            index += 1
        }
        let result = lzma_wrp_reader_process_item_indices(reader, indices, UInt32(count), cPath, usingFullPaths ? 1 : 0)
        indices.deinitialize()
        return result != 0
    }
    
    deinit {
        lzma_wrp_reader_set_user_object(reader, nil)
        lzma_wrp_reader_delete(reader)
    }
    
    //MARK: Sequence
    public struct Iterator: IteratorProtocol {
        public typealias Element = LzmaItem
        
        private let reader: lzma_wrp_reader
        private var index = UInt32(0)
        private let count: UInt32
        
        public mutating func next() -> LzmaItem? {
            if index < count {
                guard let nativeItem = lzma_wrp_reader_get_item_at_index(reader, index) else {
                    return nil
                }
                let item = LzmaItem(item: nativeItem)
                index += 1
                lzma_wrp_item_delete(nativeItem)
                return item
            }
            return nil
        }
        
        fileprivate init(reader: lzma_wrp_reader) {
            self.reader = reader
            self.count = lzma_wrp_reader_get_items_count(reader)
        }
    }
    
    public func makeIterator() -> LzmaReader.Iterator {
        return Iterator(reader: reader)
    }
    
    public subscript(index: UInt32) -> LzmaItem? {
        if index < count {
            guard let nativeItem = lzma_wrp_reader_get_item_at_index(reader, index) else {
                return nil
            }
            let item = LzmaItem(item: nativeItem)
            lzma_wrp_item_delete(nativeItem)
            return item
        }
        return nil
    }
}
