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

public class LzmaItem {
    
    public let path: String
    
    public let size: UInt64
    
    public let packedSize: UInt64
    
    public let crc32: UInt32
    
    public let creationDate: Date?
    
    public let accessDate: Date?
    
    public let modificationDate: Date?
    
    public let isEncrypted: Bool
    
    public let isDirectory: Bool
    
    internal let index: UInt32
    
    internal init(item: clzma_item_t) {
        if let cPath = clzma_item_get_path(item), let path = String(cString: cPath, encoding: .utf8) {
            self.path = path
        } else {
            self.path = ""
        }
        self.size = clzma_item_get_size(item)
        self.packedSize = clzma_item_get_packed_size(item)
        self.crc32 = clzma_item_get_crc32(item)
        var time = TimeInterval(clzma_item_get_creation_time(item))
        self.creationDate = time > 0 ? Date(timeIntervalSince1970: time) : nil
        time = TimeInterval(clzma_item_get_access_time(item))
        self.accessDate = time > 0 ? Date(timeIntervalSince1970: time) : nil
        time = TimeInterval(clzma_item_get_modification_time(item))
        self.modificationDate = time > 0 ? Date(timeIntervalSince1970: time) : nil
        self.index = clzma_item_get_index(item)
        self.isEncrypted = clzma_item_is_encrypted(item) != 0 ? true : false
        self.isDirectory = clzma_item_is_dir(item) != 0 ? true : false
    }
}
