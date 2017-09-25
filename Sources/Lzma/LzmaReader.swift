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
import CLzma

public protocol LzmaReaderDelegate: class {
    func onLzmaReader(reader: LzmaReader, progress: Double) -> Void
}

public final class LzmaReader: LzmaObject, Sequence {
    //TODO: type
//    public private(set) var type: LzmaFileType {
//        get {
//            return LzmaFileType(rawValue: clzma_reader_get_type(reader))!
//        }
//        set {
//            clzma_reader_set_type(reader, newValue.rawValue)
//        }
//    }
    
    public weak var delegate: LzmaReaderDelegate?
    
    public let url: URL
    
    public var count: UInt32 {
        return clzma_reader_get_items_count(reader)
    }
    
    public var error: LzmaError? {
        guard let error = clzma_reader_get_last_error(reader) else {
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
                clzma_reader_set_password(reader, chars)
                chars.deinitialize()
            } else {
                _password = nil
                clzma_reader_set_password(reader, nil)
            }
        }
    }
    
    private let reader: clzma_reader_t = clzma_reader_create()
    
    private func setupReader() {
//        let unmanagedSelf = clzma_wrp_handle(Unmanaged.passUnretained(self).toOpaque())
//        lzma_wrp_reader_set_user_object(reader, unmanagedSelf)
//        
//        lzma_wrp_reader_set_progress_callback(reader) { unsafeReader, progress in
//            guard let reader = unsafeReader, let userObject = lzma_wrp_reader_get_user_object(reader) else {
//                return
//            }
//            
//            let lzmaReader = Unmanaged<LzmaReader>.fromOpaque(userObject).takeUnretainedValue()
//            if let delegate = lzmaReader.delegate {
//                delegate.onLzmaReader(reader: lzmaReader, progress: progress)
//            }
//        }
    }
    
    public init(url: URL) {
        self.url = url
        super.init()
        setupReader()
    }
    
    public init(url: URL, type: LzmaFileType) {
        self.url = url
        super.init()
        //TODO: type
//        self.type = type
        setupReader()
    }
    
    public func open() -> Bool {
        //TODO: type
        guard let path = url.path.cString(using: .utf8), clzma_reader_open_file_path(reader, path, 1) != 0 else {
            return false
        }
        return true
    }
    
    public func extract(to path: String, items: [LzmaItem]? = nil, usingFullPaths: Bool = true) -> Bool {
        guard let cPath = path.cString(using: .utf8) else {
            return false
        }
        
        let sorted = items?.sorted { a, b in
            return a.index < b.index
        }
        
        if let sortedItems = sorted  {
            if sortedItems.isEmpty {
                return true
            }
            let count = sortedItems.count
            let sortedIndices = UnsafeMutablePointer<UInt32>.allocate(capacity: count)
            var index = 0
            for item in sortedItems {
                sortedIndices[index] = item.index
                index += 1
            }
            let result = clzma_reader_extract_item_indices(reader, sortedIndices, UInt32(count), cPath, usingFullPaths ? 1 : 0)
            sortedIndices.deinitialize()
            return result != 0
        }
        return clzma_reader_extract_all_items(reader, cPath, usingFullPaths ? 1 : 0) != 0
    }
    
    deinit {
        clzma_reader_set_user_object(reader, nil)
        clzma_reader_delete(reader)
    }
    
    //MARK: Sequence
    public struct Iterator: IteratorProtocol {
        public typealias Element = LzmaItem
        
        private let reader: clzma_reader_t
        private var index = UInt32(0)
        private let count: UInt32
        
        public mutating func next() -> LzmaItem? {
            if index < count {
                guard let nativeItem = clzma_reader_get_item_at_index(reader, index) else {
                    return nil
                }
                let item = LzmaItem(item: nativeItem)
                index += 1
                clzma_item_delete(nativeItem)
                return item
            }
            return nil
        }
        
        fileprivate init(reader: clzma_reader_t) {
            self.reader = reader
            self.count = clzma_reader_get_items_count(reader)
        }
    }
    
    public func makeIterator() -> LzmaReader.Iterator {
        return Iterator(reader: reader)
    }
    
    public subscript(index: UInt32) -> LzmaItem? {
        if index < count {
            guard let nativeItem = clzma_reader_get_item_at_index(reader, index) else {
                return nil
            }
            let item = LzmaItem(item: nativeItem)
            clzma_item_delete(nativeItem)
            return item
        }
        return nil
    }
}
