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


#include "lzma_wrp_writer.h"

lzma_wrp_writer lzma_wrp_writer_create(void) {
    lzma_wrp_writer writer = (lzma_wrp_writer)lzma_wrp_malloc_zero(sizeof(struct lzma_wrp_writer_struct));
    LzmaWRP::FileEncoder * encoder = new LzmaWRP::FileEncoder();
    LZMA_WRP_ASSERT(decoder)
    encoder->setWriter(writer);
    writer->encoder = encoder;
    return writer;
}

void lzma_wrp_writer_set_progress_callback(lzma_wrp_writer writer, void (*cb)(lzma_wrp_writer writer, const double progress)) {
    LZMA_WRP_ASSERT(writer)
    writer->encoder->setProgressCallback(cb);
}

void lzma_wrp_writer_set_password(lzma_wrp_writer writer, const lzma_wrp_wchar * password) {
    LZMA_WRP_ASSERT(writer)
    writer->encoder->setPassword(password);
}

void lzma_wrp_writer_delete(lzma_wrp_writer writer) {
    LZMA_WRP_ASSERT(writer)
    delete writer->encoder;
    lzma_wrp_free(writer);
}

