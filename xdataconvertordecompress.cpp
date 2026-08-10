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
#include "xdataconvertordecompress.h"

XDataConvertorDecompress::XDataConvertorDecompress(QObject *pParent) : XThreadObject(pParent)
{
    m_pDeviceIn = nullptr;
    m_pData = nullptr;
    m_method = XBinary::HANDLE_METHOD_UNKNOWN;
    m_pPdStruct = nullptr;
}

void XDataConvertorDecompress::setData(QIODevice *pDeviceIn, XDataConvertor::DATA *pData, XBinary::HANDLE_METHOD method, XBinary::PDSTRUCT *pPdStruct)
{
    m_pDeviceIn = pDeviceIn;
    m_pData = pData;
    m_method = method;
    m_pPdStruct = pPdStruct;
}

void XDataConvertorDecompress::process()
{
    XBinary::PDSTRUCT pdStructEmpty = XBinary::createPdStruct();
    XBinary::PDSTRUCT *pPdStruct = m_pPdStruct;

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);
    XBinary::setPdStructInit(pPdStruct, _nFreeIndex, 0);

    m_pData->bValid = false;
    m_pData->pTmpFile = nullptr;
    m_pData->dEntropy = 0;

    if (m_pDeviceIn) {
        m_pData->pTmpFile = new QTemporaryFile;

        if (m_pData->pTmpFile->open()) {
            XDecompress decompress;
            qint64 nInSize = m_pDeviceIn->size();

            QByteArray baOut = decompress.decomressToByteArray(m_pDeviceIn, 0, nInSize, m_method, pPdStruct);

            if ((baOut.size() > 0) && (!XBinary::isPdStructStopped(pPdStruct))) {
                if (m_pData->pTmpFile->write(baOut.constData(), baOut.size()) == baOut.size()) {
                    m_pData->pTmpFile->flush();
                    m_pData->bValid = true;
                    m_pData->dEntropy = XBinary::getEntropy(m_pData->pTmpFile, pPdStruct);
                }
            } else {
                XBinary::setPdStructInfoString(pPdStruct, tr("Cannot decompress"));
            }
        }
    } else {
        XBinary::setPdStructInfoString(pPdStruct, tr("No data"));
    }

    XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);
}
