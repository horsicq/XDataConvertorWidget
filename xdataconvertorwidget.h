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
#ifndef XDATACONVERTORWIDGET_H
#define XDATACONVERTORWIDGET_H

#include "xshortcutswidget.h"
#include <QResizeEvent>

#include "xbinary.h"
#include "xoptions.h"
#include "xhexview.h"
#include "xdialogprocess.h"
#include "xdataconvertor.h"

namespace Ui {
class XDataConvertorWidget;
}

class XDataConvertorWidget : public XShortcutsWidget {
    Q_OBJECT

    enum CMETHOD {
        CMETHOD_UNKNOWN = 0,
        CMETHOD_NONE,
        CMETHOD_XOR,
        CMETHOD_ADDSUB,
        CMETHOD_BASE64,
        CMETHOD_NOT,
        CMETHOD_ROTATE,
        CMETHOD_BSWAP,
        CMETHOD_REVERSE,
        CMETHOD_HEX,
        CMETHOD_KEY,
        CMETHOD_BITOPS,
        CMETHOD_SHIFT,
        CMETHOD_TEXT,
        CMETHOD_ENCODING,
        CMETHOD_DECOMPRESS,
    };

    enum SM {
        SM_BYTE = 0,
        SM_WORD,
        SM_DWORD,
        SM_QWORD
    };

    enum BITOP {
        BITOP_NEG = 0,
        BITOP_BITREVERSE,
        BITOP_NIBBLESWAP
    };

public:
    explicit XDataConvertorWidget(QWidget *pParent = nullptr);
    ~XDataConvertorWidget();

    virtual void adjustView();

    void setData(QIODevice *pDevice);

    virtual void setGlobal(XShortcuts *pShortcuts, XOptions *pXOptions);
    virtual void reloadData(bool bSaveSelection);

private slots:
    void on_listWidgetMethods_currentItemChanged(QListWidgetItem *pCurrent, QListWidgetItem *pPrevious);
    void on_comboBoxXORmethod_currentIndexChanged(int nIndex);
    void on_pushButtonXOR_clicked();
    void on_comboBoxADDSUBmethod_currentIndexChanged(int nIndex);
    void on_pushButtonADD_clicked();
    void on_pushButtonSUB_clicked();
    void on_pushButtonBase64Encode_clicked();
    void on_pushButtonBase64Decode_clicked();
    void on_pushButtonROL_clicked();
    void on_pushButtonROR_clicked();
    void on_pushButtonNOT_clicked();
    void on_pushButtonBSWAP_clicked();
    void on_pushButtonReverse_clicked();
    void on_pushButtonHexEncode_clicked();
    void on_pushButtonHexDecode_clicked();
    void on_pushButtonKEY_clicked();
    void on_pushButtonBITOPS_clicked();
    void on_pushButtonSHL_clicked();
    void on_pushButtonSHR_clicked();
    void on_pushButtonTEXT_clicked();
    void on_pushButtonEncEncode_clicked();
    void on_pushButtonEncDecode_clicked();
    void on_pushButtonDecompress_clicked();
    void on_pushButtonDumpInput_clicked();
    void on_pushButtonDumpOutput_clicked();

private:
    void _addMethod(const QString &sName, CMETHOD method);
    void showMethod(CMETHOD method);
    void process(CMETHOD method, XDataConvertor::CMETHOD methodConvertor, const XDataConvertor::OPTIONS &options);
    void processDecompress(CMETHOD method, XBinary::HANDLE_METHOD handleMethod);

protected:
    virtual void registerShortcuts(bool bState);

private:
    Ui::XDataConvertorWidget *ui;
    QIODevice *m_pDevice;
    XBinaryView::OPTIONS m_hexOptions;
    QMap<CMETHOD, XDataConvertor::DATA> m_mapData;
};

#endif  // XDATACONVERTORWIDGET_H
