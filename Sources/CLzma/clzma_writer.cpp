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


#include "clzma_writer.h"

clzma_writer_t clzma_writer_create(void) {
    clzma_writer_t writer = (clzma_writer_t)clzma_malloc_zero(sizeof(struct clzma_writer_struct));
    CLzma::FileEncoder * encoder = new CLzma::FileEncoder();
    CLZMA_ASSERT(decoder)
    encoder->setWriter(writer);
    writer->encoder = encoder;
    return writer;
}

void clzma_writer_set_progress_callback(clzma_writer_t writer, void (*cb)(clzma_writer_t writer, const double progress)) {
    CLZMA_ASSERT(writer)
    writer->encoder->setProgressCallback(cb);
}

void clzma_writer_set_password(clzma_writer_t writer, const clzma_wchar_t * password) {
    CLZMA_ASSERT(writer)
    writer->encoder->setPassword(password);
}

void clzma_writer_delete(clzma_writer_t writer) {
    CLZMA_ASSERT(writer)
    delete writer->encoder;
    clzma_free(writer);
}

