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


import XCTest
@testable import Lzma

class LzmaTests: XCTestCase {
    
    func testCreation() {
        var reader:LzmaReader? = LzmaReader(url: URL(fileURLWithPath: ""))
        XCTAssertNotNil(reader)
        reader = nil
    }

    func testOpenExisted1() {
        let reader = LzmaReader(url: URL(fileURLWithPath: "/Users/olehkulykov/Downloads/LzmaSDKObjC/tests/files/lzma.7z"), type: .sevenZip)
        XCTAssertTrue(reader.open())
        XCTAssertTrue(reader.count > 0)
        XCTAssertNil(reader.error)
    }
    
    func testOpenUnexisted1() {
        let reader = LzmaReader(url: URL(fileURLWithPath: "/bla_bla_dir/bla_bla_file.7z"), type: .sevenZip)
        XCTAssertFalse(reader.open())
        XCTAssertTrue(reader.count == 0)
        XCTAssertNotNil(reader.error)
    }
    
    func testIterate1() {
//        let reader = LzmaReader(url: URL(fileURLWithPath: "/Users/olehkulykov/Downloads/LzmaSDKObjC/tests/files/emptDir.7z"), type: .sevenZip)
        let reader = LzmaReader(url: URL(fileURLWithPath: "/Users/olehkulykov/Downloads/LzmaSDKObjC/tests/files/utf8.7z"), type: .sevenZip)
        reader.password = "1MünchenПривет"
        XCTAssertTrue(reader.open())
        XCTAssertTrue(reader.count > 0)
        XCTAssertNil(reader.error)
        for item in reader {
            print("\(item)")
        }
        
        XCTAssertNotNil(reader[0])
        XCTAssertNotNil(reader[1])
        
        let items = reader.map { return $0 }
        print("\(items)")
        
//        XCTAssertTrue(reader.extract(to: "/Users/olehkulykov/Downloads/Дир/Дир/", items: items, usingFullPaths: true))
        reader.extract(to: "/Users/olehkulykov/Downloads/Дир/Дир/", items: nil, usingFullPaths: true)
        XCTAssertTrue(reader.extract(to: "/Users/olehkulykov/Downloads/Дир/Дир/", items: items, usingFullPaths: true))
    }

    static var allTests = [
        ("testIterate1", testIterate1),
        ("testOpenUnexisted1", testOpenUnexisted1),
        ("testCreation", testCreation),
        ("testOpenExisted1", testOpenExisted1),
    ]
}
