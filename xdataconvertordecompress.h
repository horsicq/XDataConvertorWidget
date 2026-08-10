/* Copyright (c) 2020-2026 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef XDATACONVERTORDECOMPRESS_H
#define XDATACONVERTORDECOMPRESS_H

#include "xdataconvertor.h"
#include "xdecompress.h"

// Worker that decompresses the whole input device with one XDecompress handle method
// and fills an XDataConvertor::DATA (temp file + entropy), so the widget can treat it
// exactly like an XDataConvertor result. Lives in the widget (app) layer because it
// depends on XArchive, which the low-level Formats/XDataConvertor engine must not.
class XDataConvertorDecompress : public XThreadObject {
    Q_OBJECT

public:
    explicit XDataConvertorDecompress(QObject *pParent = nullptr);
    void setData(QIODevice *pDeviceIn, XDataConvertor::DATA *pData, XBinary::HANDLE_METHOD method, XBinary::PDSTRUCT *pPdStruct);
    virtual void process();

private:
    QIODevice *m_pDeviceIn;
    XDataConvertor::DATA *m_pData;
    XBinary::HANDLE_METHOD m_method;
    XBinary::PDSTRUCT *m_pPdStruct;
};

#endif  // XDATACONVERTORDECOMPRESS_H
