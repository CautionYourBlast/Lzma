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

public struct LzmaError: Error {
    
    public let description: String
    
    public let possibleReason: String
    
    public let file: String
    
    public let code: Int64
    
    public let line: Int32
    
    internal init(error: clzma_error_t) {
        if let unsafeDescription = clzma_error_get_description(error), let description = String(cString: unsafeDescription, encoding: .utf8) {
            self.description = description
        } else {
            self.description = ""
        }
        if let unsafeReason = clzma_error_get_possible_reason(error), let reason = String(cString: unsafeReason, encoding: .utf8) {
            self.possibleReason = reason
        } else {
            self.possibleReason = ""
        }
        if let unsafeFile = clzma_error_get_file(error), let file = String(cString: unsafeFile, encoding: .utf8) {
            self.file = file
        } else {
            self.file = ""
        }
        self.code = clzma_error_get_code(error)
        self.line = clzma_error_get_line(error)
    }
}
