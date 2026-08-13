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
#include "xdataconvertorwidget.h"
#include "ui_xdataconvertorwidget.h"

#include "xdataconvertordecompress.h"

namespace {
void deleteTempFile(XDataConvertor::DATA *pData)
{
    if (pData && pData->pTmpFile) {
        delete pData->pTmpFile;
        pData->pTmpFile = nullptr;
        pData->bValid = false;
    }
}
}  // namespace

XDataConvertorWidget::XDataConvertorWidget(QWidget *pParent) : XShortcutsWidget(pParent), ui(new Ui::XDataConvertorWidget)
{
    ui->setupUi(this);

    m_pDevice = nullptr;
    ui->lineEditSizeInput->setReadOnly(true);
    ui->lineEditSizeOutput->setReadOnly(true);
    ui->lineEditEntropyInput->setReadOnly(true);
    ui->lineEditEntropyOutput->setReadOnly(true);

    ui->listWidgetMethods->blockSignals(true);

    _addMethod(QString(""), CMETHOD_NONE);
    _addMethod(QString("XOR"), CMETHOD_XOR);
    _addMethod(QString("ADD/SUB"), CMETHOD_ADDSUB);
    _addMethod(QString("Base64"), CMETHOD_BASE64);
    _addMethod(QString("NOT"), CMETHOD_NOT);
    _addMethod(QString("ROL/ROR"), CMETHOD_ROTATE);
    _addMethod(QString("Byte swap"), CMETHOD_BSWAP);
    _addMethod(QString("Reverse"), CMETHOD_REVERSE);
    _addMethod(QString("Hex"), CMETHOD_HEX);
    _addMethod(QString("Key"), CMETHOD_KEY);
    _addMethod(QString("Bit ops"), CMETHOD_BITOPS);
    _addMethod(QString("Shift"), CMETHOD_SHIFT);
    _addMethod(QString("Text"), CMETHOD_TEXT);
    _addMethod(QString("Encodings"), CMETHOD_ENCODING);
    _addMethod(QString("Decompress"), CMETHOD_DECOMPRESS);
    _addMethod(QString("Cipher"), CMETHOD_CIPHER);
    _addMethod(QString("Filter"), CMETHOD_FILTER);
    _addMethod(QString("Charset"), CMETHOD_CHARSET);

    XOptions::adjustListWidgetSize(ui->listWidgetMethods);

    ui->listWidgetMethods->blockSignals(false);

    m_hexOptions = {};
    ui->widgetHexInput->setContextMenuEnable(false);
    ui->widgetHexOutput->setContextMenuEnable(false);

    ui->stackedWidgetOptions->setCurrentWidget(ui->pageOriginal);

    {
        ui->comboBoxXORmethod->blockSignals(true);

        ui->comboBoxXORmethod->addItem("BYTE", SM_BYTE);
        ui->comboBoxXORmethod->addItem("WORD", SM_WORD);
        ui->comboBoxXORmethod->addItem("DWORD", SM_DWORD);
        ui->comboBoxXORmethod->addItem("QWORD", SM_QWORD);

        ui->lineEditXORValue->setValidatorModeValue(XLineEditValidator::MODE_HEX_8, 0);

        ui->comboBoxXORmethod->blockSignals(false);
    }
    {
        ui->comboBoxADDSUBmethod->blockSignals(true);

        ui->comboBoxADDSUBmethod->addItem("BYTE", SM_BYTE);
        ui->comboBoxADDSUBmethod->addItem("WORD", SM_WORD);
        ui->comboBoxADDSUBmethod->addItem("DWORD", SM_DWORD);
        ui->comboBoxADDSUBmethod->addItem("QWORD", SM_QWORD);

        ui->lineEditADDSUBValue->setValidatorModeValue(XLineEditValidator::MODE_HEX_8, 0);

        ui->comboBoxADDSUBmethod->blockSignals(false);
    }
    {
        ui->comboBoxROTATEmethod->blockSignals(true);

        ui->comboBoxROTATEmethod->addItem("BYTE", SM_BYTE);
        ui->comboBoxROTATEmethod->addItem("WORD", SM_WORD);
        ui->comboBoxROTATEmethod->addItem("DWORD", SM_DWORD);
        ui->comboBoxROTATEmethod->addItem("QWORD", SM_QWORD);

        // The value is a bit-rotation count (masked to the unit width in the engine),
        // so a single byte-sized validator is enough for every width.
        ui->lineEditROTATEValue->setValidatorModeValue(XLineEditValidator::MODE_HEX_8, 0);

        ui->comboBoxROTATEmethod->blockSignals(false);
    }
    {
        ui->comboBoxBSWAPmethod->blockSignals(true);

        ui->comboBoxBSWAPmethod->addItem("WORD", SM_WORD);
        ui->comboBoxBSWAPmethod->addItem("DWORD", SM_DWORD);
        ui->comboBoxBSWAPmethod->addItem("QWORD", SM_QWORD);

        ui->comboBoxBSWAPmethod->blockSignals(false);
    }
    {
        // Key family: the combo carries the concrete engine method; the key line-edit
        // holds a hex byte string.
        ui->comboBoxKEYop->blockSignals(true);

        ui->comboBoxKEYop->addItem("XOR", XDataConvertor::CMETHOD_XOR_KEY);
        ui->comboBoxKEYop->addItem("ADD", XDataConvertor::CMETHOD_ADD_KEY);
        ui->comboBoxKEYop->addItem("SUB", XDataConvertor::CMETHOD_SUB_KEY);
        ui->comboBoxKEYop->addItem("XOR rolling", XDataConvertor::CMETHOD_XOR_ROLLING);

        ui->comboBoxKEYop->blockSignals(false);
    }
    {
        ui->comboBoxBITOPSwidth->blockSignals(true);
        ui->comboBoxBITOPSop->blockSignals(true);

        ui->comboBoxBITOPSwidth->addItem("BYTE", SM_BYTE);
        ui->comboBoxBITOPSwidth->addItem("WORD", SM_WORD);
        ui->comboBoxBITOPSwidth->addItem("DWORD", SM_DWORD);
        ui->comboBoxBITOPSwidth->addItem("QWORD", SM_QWORD);

        ui->comboBoxBITOPSop->addItem("NEG", BITOP_NEG);
        ui->comboBoxBITOPSop->addItem("Bit reverse", BITOP_BITREVERSE);
        ui->comboBoxBITOPSop->addItem("Nibble swap", BITOP_NIBBLESWAP);

        ui->comboBoxBITOPSop->blockSignals(false);
        ui->comboBoxBITOPSwidth->blockSignals(false);
    }
    {
        ui->comboBoxSHIFTwidth->blockSignals(true);

        ui->comboBoxSHIFTwidth->addItem("BYTE", SM_BYTE);
        ui->comboBoxSHIFTwidth->addItem("WORD", SM_WORD);
        ui->comboBoxSHIFTwidth->addItem("DWORD", SM_DWORD);
        ui->comboBoxSHIFTwidth->addItem("QWORD", SM_QWORD);

        ui->lineEditSHIFTValue->setValidatorModeValue(XLineEditValidator::MODE_HEX_8, 0);

        ui->comboBoxSHIFTwidth->blockSignals(false);
    }
    {
        ui->comboBoxTEXTop->blockSignals(true);

        ui->comboBoxTEXTop->addItem("ROT13", XDataConvertor::CMETHOD_ROT13);
        ui->comboBoxTEXTop->addItem("ROT47", XDataConvertor::CMETHOD_ROT47);

        ui->comboBoxTEXTop->blockSignals(false);
    }
    {
        // The format combo carries the "encode" method id; the decode id is the next
        // enum value (encode/decode are declared as adjacent pairs).
        ui->comboBoxENCODINGformat->blockSignals(true);

        ui->comboBoxENCODINGformat->addItem("Base64 URL", XDataConvertor::CMETHOD_BASE64URL_ENCODE);
        ui->comboBoxENCODINGformat->addItem("Base32", XDataConvertor::CMETHOD_BASE32_ENCODE);
        ui->comboBoxENCODINGformat->addItem("Base58", XDataConvertor::CMETHOD_BASE58_ENCODE);
        ui->comboBoxENCODINGformat->addItem("Ascii85", XDataConvertor::CMETHOD_ASCII85_ENCODE);
        ui->comboBoxENCODINGformat->addItem("URL", XDataConvertor::CMETHOD_URL_ENCODE);
        ui->comboBoxENCODINGformat->addItem("Quoted-printable", XDataConvertor::CMETHOD_QP_ENCODE);
        ui->comboBoxENCODINGformat->addItem("UUencode", XDataConvertor::CMETHOD_UU_ENCODE);

        ui->comboBoxENCODINGformat->blockSignals(false);
    }
    {
        // The combo carries an XBinary::HANDLE_METHOD; only self-describing decoders
        // that work on a whole stream without extra parameters are exposed.
        ui->comboBoxDECOMPRESSformat->blockSignals(true);

        ui->comboBoxDECOMPRESSformat->addItem("zlib", XBinary::HANDLE_METHOD_ZLIB);
        ui->comboBoxDECOMPRESSformat->addItem("Deflate (raw)", XBinary::HANDLE_METHOD_DEFLATE);
        ui->comboBoxDECOMPRESSformat->addItem("bzip2", XBinary::HANDLE_METHOD_BZIP2);
        ui->comboBoxDECOMPRESSformat->addItem("XZ", XBinary::HANDLE_METHOD_XZ);
        ui->comboBoxDECOMPRESSformat->addItem("LZIP", XBinary::HANDLE_METHOD_LZIP);
        ui->comboBoxDECOMPRESSformat->addItem("Zstandard", XBinary::HANDLE_METHOD_ZSTD);
        ui->comboBoxDECOMPRESSformat->addItem("Brotli", XBinary::HANDLE_METHOD_BROTLI);
        ui->comboBoxDECOMPRESSformat->addItem("LZO (lzop)", XBinary::HANDLE_METHOD_LZOP);
        ui->comboBoxDECOMPRESSformat->addItem("Compress (.Z)", XBinary::HANDLE_METHOD_COMPRESS);
        ui->comboBoxDECOMPRESSformat->addItem("LZW (PDF)", XBinary::HANDLE_METHOD_LZW_PDF);
        ui->comboBoxDECOMPRESSformat->addItem("RunLength (PackBits)", XBinary::HANDLE_METHOD_RUNLENGTH);
        ui->comboBoxDECOMPRESSformat->addItem("x86 BCJ", XBinary::HANDLE_METHOD_BCJ);

        ui->comboBoxDECOMPRESSformat->blockSignals(false);
    }
    {
        // Cipher family: combo carries the engine method, key line-edit holds a hex key.
        ui->comboBoxCIPHERop->blockSignals(true);

        ui->comboBoxCIPHERop->addItem("RC4", XDataConvertor::CMETHOD_RC4);

        ui->comboBoxCIPHERop->blockSignals(false);
    }
    {
        // Filter family: combo carries the "encode" method (decode = encode + 1); the
        // distance line-edit holds the byte distance.
        ui->comboBoxFILTERop->blockSignals(true);

        ui->comboBoxFILTERop->addItem("Delta", XDataConvertor::CMETHOD_DELTA_ENCODE);
        ui->comboBoxFILTERop->addItem("XOR prev", XDataConvertor::CMETHOD_XORPREV_ENCODE);

        ui->lineEditFILTERdist->setValidatorModeValue(XLineEditValidator::MODE_HEX_32, 1);

        ui->comboBoxFILTERop->blockSignals(false);
    }
    {
        ui->comboBoxCHARSETop->blockSignals(true);

        ui->comboBoxCHARSETop->addItem("Uppercase", XDataConvertor::CMETHOD_UPPERCASE);
        ui->comboBoxCHARSETop->addItem("Lowercase", XDataConvertor::CMETHOD_LOWERCASE);
        ui->comboBoxCHARSETop->addItem("Swap case", XDataConvertor::CMETHOD_SWAPCASE);
        ui->comboBoxCHARSETop->addItem("Atbash", XDataConvertor::CMETHOD_ATBASH);
        ui->comboBoxCHARSETop->addItem("ROT5", XDataConvertor::CMETHOD_ROT5);
        ui->comboBoxCHARSETop->addItem("ROT18", XDataConvertor::CMETHOD_ROT18);
        ui->comboBoxCHARSETop->addItem("EBCDIC -> ASCII", XDataConvertor::CMETHOD_EBCDIC_TO_ASCII);
        ui->comboBoxCHARSETop->addItem("ASCII -> EBCDIC", XDataConvertor::CMETHOD_ASCII_TO_EBCDIC);

        ui->comboBoxCHARSETop->blockSignals(false);
    }
}

XDataConvertorWidget::~XDataConvertorWidget()
{
    QMap<CMETHOD, XDataConvertor::DATA>::iterator it = m_mapData.begin();
    while (it != m_mapData.end()) {
        deleteTempFile(&it.value());
        ++it;
    }

    delete ui;
}

void XDataConvertorWidget::adjustView()
{
    // TODO
}

void XDataConvertorWidget::setData(QIODevice *pDevice)
{
    m_pDevice = pDevice;

    // Drop results cached from a previous device so switching methods recomputes
    // against the new input instead of showing stale output.
    QMap<CMETHOD, XDataConvertor::DATA>::iterator it = m_mapData.begin();
    while (it != m_mapData.end()) {
        deleteTempFile(&it.value());
        it.value().dEntropy = 0;
        ++it;
    }

    ui->widgetHexInput->setData(pDevice, m_hexOptions, true);

    XDataConvertor::OPTIONS options = {};
    process(CMETHOD_NONE, XDataConvertor::CMETHOD_NONE, options);

    ui->listWidgetMethods->setCurrentRow(0);
}

void XDataConvertorWidget::setGlobal(XShortcuts *pShortcuts, XOptions *pXOptions)
{
    ui->widgetHexInput->setGlobal(pShortcuts, pXOptions);
    ui->widgetHexOutput->setGlobal(pShortcuts, pXOptions);

    XShortcutsWidget::setGlobal(pShortcuts, pXOptions);
}

void XDataConvertorWidget::reloadData(bool bSaveSelection)
{
    Q_UNUSED(bSaveSelection)
}

void XDataConvertorWidget::_addMethod(const QString &sName, CMETHOD method)
{
    QListWidgetItem *pItem = new QListWidgetItem(sName);
    pItem->setData(Qt::UserRole, method);

    ui->listWidgetMethods->addItem(pItem);

    XDataConvertor::DATA _data = {};
    _data.bValid = false;

    m_mapData.insert(method, _data);
}

void XDataConvertorWidget::showMethod(CMETHOD method)
{
    XDataConvertor::DATA _data = m_mapData.value(method);

    ui->lineEditEntropyOutput->setValue_double(_data.dEntropy);

    if (!m_pDevice) {
        ui->widgetHexOutput->reset();
        ui->lineEditSizeInput->setValue_uint64(0, XLineEditHEX::_MODE_SIZE);
        ui->lineEditSizeOutput->setValue_uint64(0, XLineEditHEX::_MODE_SIZE);
        ui->lineEditEntropyInput->setValue_double(0);
        ui->lineEditEntropyOutput->setValue_double(0);
    } else if (method == CMETHOD_NONE) {
        ui->lineEditEntropyInput->setValue_double(_data.dEntropy);
        ui->lineEditSizeInput->setValue_uint64(m_pDevice->size(), XLineEditHEX::_MODE_SIZE);
        ui->widgetHexOutput->setData(m_pDevice, m_hexOptions, true);
        ui->lineEditSizeOutput->setValue_uint64(m_pDevice->size(), XLineEditHEX::_MODE_SIZE);
    } else if (_data.bValid) {
        ui->widgetHexOutput->setData(_data.pTmpFile, m_hexOptions, true);
        ui->lineEditSizeOutput->setValue_uint64(_data.pTmpFile->size(), XLineEditHEX::_MODE_SIZE);
    } else {
        ui->widgetHexOutput->reset();
        ui->lineEditSizeOutput->setValue_uint64(0, XLineEditHEX::_MODE_SIZE);
    }

    if (method == CMETHOD_NONE) {
        ui->stackedWidgetOptions->setCurrentWidget(ui->pageOriginal);
    } else if (method == CMETHOD_XOR) {
        ui->stackedWidgetOptions->setCurrentWidget(ui->pageXOR);
    } else if (method == CMETHOD_ADDSUB) {
        ui->stackedWidgetOptions->setCurrentWidget(ui->pageADDSUB);
    } else if (method == CMETHOD_BASE64) {
        ui->stackedWidgetOptions->setCurrentWidget(ui->pageBASE64);
    } else if (method == CMETHOD_NOT) {
        ui->stackedWidgetOptions->setCurrentWidget(ui->pageNOT);
    } else if (method == CMETHOD_ROTATE) {
        ui->stackedWidgetOptions->setCurrentWidget(ui->pageROTATE);
    } else if (method == CMETHOD_BSWAP) {
        ui->stackedWidgetOptions->setCurrentWidget(ui->pageBSWAP);
    } else if (method == CMETHOD_REVERSE) {
        ui->stackedWidgetOptions->setCurrentWidget(ui->pageREVERSE);
    } else if (method == CMETHOD_HEX) {
        ui->stackedWidgetOptions->setCurrentWidget(ui->pageHEX);
    } else if (method == CMETHOD_KEY) {
        ui->stackedWidgetOptions->setCurrentWidget(ui->pageKEY);
    } else if (method == CMETHOD_BITOPS) {
        ui->stackedWidgetOptions->setCurrentWidget(ui->pageBITOPS);
    } else if (method == CMETHOD_SHIFT) {
        ui->stackedWidgetOptions->setCurrentWidget(ui->pageSHIFT);
    } else if (method == CMETHOD_TEXT) {
        ui->stackedWidgetOptions->setCurrentWidget(ui->pageTEXT);
    } else if (method == CMETHOD_ENCODING) {
        ui->stackedWidgetOptions->setCurrentWidget(ui->pageENCODING);
    } else if (method == CMETHOD_DECOMPRESS) {
        ui->stackedWidgetOptions->setCurrentWidget(ui->pageDECOMPRESS);
    } else if (method == CMETHOD_CIPHER) {
        ui->stackedWidgetOptions->setCurrentWidget(ui->pageCIPHER);
    } else if (method == CMETHOD_FILTER) {
        ui->stackedWidgetOptions->setCurrentWidget(ui->pageFILTER);
    } else if (method == CMETHOD_CHARSET) {
        ui->stackedWidgetOptions->setCurrentWidget(ui->pageCHARSET);
    }
}

void XDataConvertorWidget::process(CMETHOD method, XDataConvertor::CMETHOD methodConvertor, const XDataConvertor::OPTIONS &options)
{
    XDataConvertor::DATA _data = {};

    XDataConvertor dataConverter;
    XDialogProcess dcp(this, &dataConverter);
    dcp.setGlobal(getShortcuts(), getGlobalOptions());
    dataConverter.setData(m_pDevice, &_data, methodConvertor, options, dcp.getPdStruct());
    dcp.start();

    if (dcp.showDialogDelay() == QDialog::Accepted) {
        deleteTempFile(&m_mapData[method]);

        m_mapData[method] = _data;
        _data.pTmpFile = nullptr;
    }

    deleteTempFile(&_data);

    showMethod(method);
}

void XDataConvertorWidget::processDecompress(CMETHOD method, XBinary::HANDLE_METHOD handleMethod)
{
    XDataConvertor::DATA _data = {};

    XDataConvertorDecompress worker;
    XDialogProcess dcp(this, &worker);
    dcp.setGlobal(getShortcuts(), getGlobalOptions());
    worker.setData(m_pDevice, &_data, handleMethod, dcp.getPdStruct());
    dcp.start();

    if (dcp.showDialogDelay() == QDialog::Accepted) {
        deleteTempFile(&m_mapData[method]);

        m_mapData[method] = _data;
        _data.pTmpFile = nullptr;
    }

    deleteTempFile(&_data);

    showMethod(method);
}

void XDataConvertorWidget::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
}

void XDataConvertorWidget::on_listWidgetMethods_currentItemChanged(QListWidgetItem *pCurrent, QListWidgetItem *pPrevious)
{
    Q_UNUSED(pPrevious)

    if (!pCurrent) {
        return;
    }

    CMETHOD method = (CMETHOD)(pCurrent->data(Qt::UserRole).toInt());

    showMethod(method);
}

void XDataConvertorWidget::on_comboBoxXORmethod_currentIndexChanged(int nIndex)
{
    Q_UNUSED(nIndex)

    SM sm = (SM)(ui->comboBoxXORmethod->currentData(Qt::UserRole).toUInt());

    if (sm == SM_BYTE) {
        ui->lineEditXORValue->setValidatorMode(XLineEditValidator::MODE_HEX_8);
    } else if (sm == SM_WORD) {
        ui->lineEditXORValue->setValidatorMode(XLineEditValidator::MODE_HEX_16);
    } else if (sm == SM_DWORD) {
        ui->lineEditXORValue->setValidatorMode(XLineEditValidator::MODE_HEX_32);
    } else if (sm == SM_QWORD) {
        ui->lineEditXORValue->setValidatorMode(XLineEditValidator::MODE_HEX_64);
    }
}

void XDataConvertorWidget::on_pushButtonXOR_clicked()
{
    XDataConvertor::CMETHOD methodConvertor = XDataConvertor::CMETHOD_UNKNOWN;
    XDataConvertor::OPTIONS options = {};

    SM sm = (SM)(ui->comboBoxXORmethod->currentData(Qt::UserRole).toUInt());

    if (sm == SM_BYTE) {
        methodConvertor = XDataConvertor::CMETHOD_XOR_BYTE;
        options.varKey = ui->lineEditXORValue->getValue_uint8();
    } else if (sm == SM_WORD) {
        methodConvertor = XDataConvertor::CMETHOD_XOR_WORD;
        options.varKey = ui->lineEditXORValue->getValue_uint16();
    } else if (sm == SM_DWORD) {
        methodConvertor = XDataConvertor::CMETHOD_XOR_DWORD;
        options.varKey = ui->lineEditXORValue->getValue_uint32();
    } else if (sm == SM_QWORD) {
        methodConvertor = XDataConvertor::CMETHOD_XOR_QWORD;
        options.varKey = ui->lineEditXORValue->getValue_uint64();
    }

    process(CMETHOD_XOR, methodConvertor, options);
}

void XDataConvertorWidget::on_comboBoxADDSUBmethod_currentIndexChanged(int nIndex)
{
    Q_UNUSED(nIndex)

    SM sm = (SM)(ui->comboBoxADDSUBmethod->currentData(Qt::UserRole).toUInt());

    if (sm == SM_BYTE) {
        ui->lineEditADDSUBValue->setValidatorMode(XLineEditValidator::MODE_HEX_8);
    } else if (sm == SM_WORD) {
        ui->lineEditADDSUBValue->setValidatorMode(XLineEditValidator::MODE_HEX_16);
    } else if (sm == SM_DWORD) {
        ui->lineEditADDSUBValue->setValidatorMode(XLineEditValidator::MODE_HEX_32);
    } else if (sm == SM_QWORD) {
        ui->lineEditADDSUBValue->setValidatorMode(XLineEditValidator::MODE_HEX_64);
    }
}

void XDataConvertorWidget::on_pushButtonADD_clicked()
{
    XDataConvertor::CMETHOD methodConvertor = XDataConvertor::CMETHOD_UNKNOWN;
    XDataConvertor::OPTIONS options = {};

    SM sm = (SM)(ui->comboBoxADDSUBmethod->currentData(Qt::UserRole).toUInt());

    if (sm == SM_BYTE) {
        methodConvertor = XDataConvertor::CMETHOD_ADD_BYTE;
        options.varKey = ui->lineEditADDSUBValue->getValue_uint8();
    } else if (sm == SM_WORD) {
        methodConvertor = XDataConvertor::CMETHOD_ADD_WORD;
        options.varKey = ui->lineEditADDSUBValue->getValue_uint16();
    } else if (sm == SM_DWORD) {
        methodConvertor = XDataConvertor::CMETHOD_ADD_DWORD;
        options.varKey = ui->lineEditADDSUBValue->getValue_uint32();
    } else if (sm == SM_QWORD) {
        methodConvertor = XDataConvertor::CMETHOD_ADD_QWORD;
        options.varKey = ui->lineEditADDSUBValue->getValue_uint64();
    }

    process(CMETHOD_ADDSUB, methodConvertor, options);
}

void XDataConvertorWidget::on_pushButtonSUB_clicked()
{
    XDataConvertor::CMETHOD methodConvertor = XDataConvertor::CMETHOD_UNKNOWN;
    XDataConvertor::OPTIONS options = {};

    SM sm = (SM)(ui->comboBoxADDSUBmethod->currentData(Qt::UserRole).toUInt());

    if (sm == SM_BYTE) {
        methodConvertor = XDataConvertor::CMETHOD_SUB_BYTE;
        options.varKey = ui->lineEditADDSUBValue->getValue_uint8();
    } else if (sm == SM_WORD) {
        methodConvertor = XDataConvertor::CMETHOD_SUB_WORD;
        options.varKey = ui->lineEditADDSUBValue->getValue_uint16();
    } else if (sm == SM_DWORD) {
        methodConvertor = XDataConvertor::CMETHOD_SUB_DWORD;
        options.varKey = ui->lineEditADDSUBValue->getValue_uint32();
    } else if (sm == SM_QWORD) {
        methodConvertor = XDataConvertor::CMETHOD_SUB_QWORD;
        options.varKey = ui->lineEditADDSUBValue->getValue_uint64();
    }

    process(CMETHOD_ADDSUB, methodConvertor, options);
}

void XDataConvertorWidget::on_pushButtonBase64Encode_clicked()
{
    XDataConvertor::OPTIONS options = {};
    process(CMETHOD_BASE64, XDataConvertor::CMETHOD_BASE64_ENCODE, options);
}

void XDataConvertorWidget::on_pushButtonBase64Decode_clicked()
{
    XDataConvertor::OPTIONS options = {};
    process(CMETHOD_BASE64, XDataConvertor::CMETHOD_BASE64_DECODE, options);
}

void XDataConvertorWidget::on_pushButtonROL_clicked()
{
    XDataConvertor::CMETHOD methodConvertor = XDataConvertor::CMETHOD_UNKNOWN;
    XDataConvertor::OPTIONS options = {};

    SM sm = (SM)(ui->comboBoxROTATEmethod->currentData(Qt::UserRole).toUInt());

    if (sm == SM_BYTE) {
        methodConvertor = XDataConvertor::CMETHOD_ROL_BYTE;
    } else if (sm == SM_WORD) {
        methodConvertor = XDataConvertor::CMETHOD_ROL_WORD;
    } else if (sm == SM_DWORD) {
        methodConvertor = XDataConvertor::CMETHOD_ROL_DWORD;
    } else if (sm == SM_QWORD) {
        methodConvertor = XDataConvertor::CMETHOD_ROL_QWORD;
    }

    options.varKey = ui->lineEditROTATEValue->getValue_uint8();

    process(CMETHOD_ROTATE, methodConvertor, options);
}

void XDataConvertorWidget::on_pushButtonROR_clicked()
{
    XDataConvertor::CMETHOD methodConvertor = XDataConvertor::CMETHOD_UNKNOWN;
    XDataConvertor::OPTIONS options = {};

    SM sm = (SM)(ui->comboBoxROTATEmethod->currentData(Qt::UserRole).toUInt());

    if (sm == SM_BYTE) {
        methodConvertor = XDataConvertor::CMETHOD_ROR_BYTE;
    } else if (sm == SM_WORD) {
        methodConvertor = XDataConvertor::CMETHOD_ROR_WORD;
    } else if (sm == SM_DWORD) {
        methodConvertor = XDataConvertor::CMETHOD_ROR_DWORD;
    } else if (sm == SM_QWORD) {
        methodConvertor = XDataConvertor::CMETHOD_ROR_QWORD;
    }

    options.varKey = ui->lineEditROTATEValue->getValue_uint8();

    process(CMETHOD_ROTATE, methodConvertor, options);
}

void XDataConvertorWidget::on_pushButtonNOT_clicked()
{
    XDataConvertor::OPTIONS options = {};
    process(CMETHOD_NOT, XDataConvertor::CMETHOD_NOT, options);
}

void XDataConvertorWidget::on_pushButtonBSWAP_clicked()
{
    XDataConvertor::CMETHOD methodConvertor = XDataConvertor::CMETHOD_UNKNOWN;
    XDataConvertor::OPTIONS options = {};

    SM sm = (SM)(ui->comboBoxBSWAPmethod->currentData(Qt::UserRole).toUInt());

    if (sm == SM_WORD) {
        methodConvertor = XDataConvertor::CMETHOD_BSWAP_WORD;
    } else if (sm == SM_DWORD) {
        methodConvertor = XDataConvertor::CMETHOD_BSWAP_DWORD;
    } else if (sm == SM_QWORD) {
        methodConvertor = XDataConvertor::CMETHOD_BSWAP_QWORD;
    }

    process(CMETHOD_BSWAP, methodConvertor, options);
}

void XDataConvertorWidget::on_pushButtonReverse_clicked()
{
    XDataConvertor::OPTIONS options = {};
    process(CMETHOD_REVERSE, XDataConvertor::CMETHOD_REVERSE, options);
}

void XDataConvertorWidget::on_pushButtonHexEncode_clicked()
{
    XDataConvertor::OPTIONS options = {};
    process(CMETHOD_HEX, XDataConvertor::CMETHOD_HEX_ENCODE, options);
}

void XDataConvertorWidget::on_pushButtonHexDecode_clicked()
{
    XDataConvertor::OPTIONS options = {};
    process(CMETHOD_HEX, XDataConvertor::CMETHOD_HEX_DECODE, options);
}

void XDataConvertorWidget::on_pushButtonKEY_clicked()
{
    XDataConvertor::CMETHOD methodConvertor = (XDataConvertor::CMETHOD)(ui->comboBoxKEYop->currentData(Qt::UserRole).toInt());

    XDataConvertor::OPTIONS options = {};
    options.baKey = QByteArray::fromHex(ui->lineEditKEYValue->text().toLatin1());

    process(CMETHOD_KEY, methodConvertor, options);
}

void XDataConvertorWidget::on_pushButtonBITOPS_clicked()
{
    BITOP bitop = (BITOP)(ui->comboBoxBITOPSop->currentData(Qt::UserRole).toInt());
    SM sm = (SM)(ui->comboBoxBITOPSwidth->currentData(Qt::UserRole).toUInt());

    XDataConvertor::CMETHOD methodConvertor = XDataConvertor::CMETHOD_UNKNOWN;

    if (bitop == BITOP_NEG) {
        if (sm == SM_BYTE) {
            methodConvertor = XDataConvertor::CMETHOD_NEG_BYTE;
        } else if (sm == SM_WORD) {
            methodConvertor = XDataConvertor::CMETHOD_NEG_WORD;
        } else if (sm == SM_DWORD) {
            methodConvertor = XDataConvertor::CMETHOD_NEG_DWORD;
        } else if (sm == SM_QWORD) {
            methodConvertor = XDataConvertor::CMETHOD_NEG_QWORD;
        }
    } else if (bitop == BITOP_BITREVERSE) {
        if (sm == SM_BYTE) {
            methodConvertor = XDataConvertor::CMETHOD_BITREVERSE_BYTE;
        } else if (sm == SM_WORD) {
            methodConvertor = XDataConvertor::CMETHOD_BITREVERSE_WORD;
        } else if (sm == SM_DWORD) {
            methodConvertor = XDataConvertor::CMETHOD_BITREVERSE_DWORD;
        } else if (sm == SM_QWORD) {
            methodConvertor = XDataConvertor::CMETHOD_BITREVERSE_QWORD;
        }
    } else if (bitop == BITOP_NIBBLESWAP) {
        methodConvertor = XDataConvertor::CMETHOD_NIBBLESWAP;  // byte width only
    }

    XDataConvertor::OPTIONS options = {};
    process(CMETHOD_BITOPS, methodConvertor, options);
}

void XDataConvertorWidget::on_pushButtonSHL_clicked()
{
    SM sm = (SM)(ui->comboBoxSHIFTwidth->currentData(Qt::UserRole).toUInt());

    XDataConvertor::CMETHOD methodConvertor = XDataConvertor::CMETHOD_UNKNOWN;

    if (sm == SM_BYTE) {
        methodConvertor = XDataConvertor::CMETHOD_SHL_BYTE;
    } else if (sm == SM_WORD) {
        methodConvertor = XDataConvertor::CMETHOD_SHL_WORD;
    } else if (sm == SM_DWORD) {
        methodConvertor = XDataConvertor::CMETHOD_SHL_DWORD;
    } else if (sm == SM_QWORD) {
        methodConvertor = XDataConvertor::CMETHOD_SHL_QWORD;
    }

    XDataConvertor::OPTIONS options = {};
    options.varKey = ui->lineEditSHIFTValue->getValue_uint8();

    process(CMETHOD_SHIFT, methodConvertor, options);
}

void XDataConvertorWidget::on_pushButtonSHR_clicked()
{
    SM sm = (SM)(ui->comboBoxSHIFTwidth->currentData(Qt::UserRole).toUInt());

    XDataConvertor::CMETHOD methodConvertor = XDataConvertor::CMETHOD_UNKNOWN;

    if (sm == SM_BYTE) {
        methodConvertor = XDataConvertor::CMETHOD_SHR_BYTE;
    } else if (sm == SM_WORD) {
        methodConvertor = XDataConvertor::CMETHOD_SHR_WORD;
    } else if (sm == SM_DWORD) {
        methodConvertor = XDataConvertor::CMETHOD_SHR_DWORD;
    } else if (sm == SM_QWORD) {
        methodConvertor = XDataConvertor::CMETHOD_SHR_QWORD;
    }

    XDataConvertor::OPTIONS options = {};
    options.varKey = ui->lineEditSHIFTValue->getValue_uint8();

    process(CMETHOD_SHIFT, methodConvertor, options);
}

void XDataConvertorWidget::on_pushButtonTEXT_clicked()
{
    XDataConvertor::CMETHOD methodConvertor = (XDataConvertor::CMETHOD)(ui->comboBoxTEXTop->currentData(Qt::UserRole).toInt());

    XDataConvertor::OPTIONS options = {};
    process(CMETHOD_TEXT, methodConvertor, options);
}

void XDataConvertorWidget::on_pushButtonEncEncode_clicked()
{
    // The combo carries the encode method; encode/decode are declared as adjacent pairs.
    XDataConvertor::CMETHOD methodConvertor = (XDataConvertor::CMETHOD)(ui->comboBoxENCODINGformat->currentData(Qt::UserRole).toInt());

    XDataConvertor::OPTIONS options = {};
    process(CMETHOD_ENCODING, methodConvertor, options);
}

void XDataConvertorWidget::on_pushButtonEncDecode_clicked()
{
    XDataConvertor::CMETHOD methodConvertor = (XDataConvertor::CMETHOD)(ui->comboBoxENCODINGformat->currentData(Qt::UserRole).toInt() + 1);

    XDataConvertor::OPTIONS options = {};
    process(CMETHOD_ENCODING, methodConvertor, options);
}

void XDataConvertorWidget::on_pushButtonDecompress_clicked()
{
    XBinary::HANDLE_METHOD handleMethod = (XBinary::HANDLE_METHOD)(ui->comboBoxDECOMPRESSformat->currentData(Qt::UserRole).toInt());

    processDecompress(CMETHOD_DECOMPRESS, handleMethod);
}

void XDataConvertorWidget::on_pushButtonCIPHER_clicked()
{
    XDataConvertor::CMETHOD methodConvertor = (XDataConvertor::CMETHOD)(ui->comboBoxCIPHERop->currentData(Qt::UserRole).toInt());

    XDataConvertor::OPTIONS options = {};
    options.baKey = QByteArray::fromHex(ui->lineEditCIPHERkey->text().toLatin1());

    process(CMETHOD_CIPHER, methodConvertor, options);
}

void XDataConvertorWidget::on_pushButtonFilterEncode_clicked()
{
    XDataConvertor::CMETHOD methodConvertor = (XDataConvertor::CMETHOD)(ui->comboBoxFILTERop->currentData(Qt::UserRole).toInt());

    XDataConvertor::OPTIONS options = {};
    options.varKey = ui->lineEditFILTERdist->getValue_uint32();

    process(CMETHOD_FILTER, methodConvertor, options);
}

void XDataConvertorWidget::on_pushButtonFilterDecode_clicked()
{
    // The combo carries the encode method; decode is the adjacent enum value.
    XDataConvertor::CMETHOD methodConvertor = (XDataConvertor::CMETHOD)(ui->comboBoxFILTERop->currentData(Qt::UserRole).toInt() + 1);

    XDataConvertor::OPTIONS options = {};
    options.varKey = ui->lineEditFILTERdist->getValue_uint32();

    process(CMETHOD_FILTER, methodConvertor, options);
}

void XDataConvertorWidget::on_pushButtonCHARSET_clicked()
{
    XDataConvertor::CMETHOD methodConvertor = (XDataConvertor::CMETHOD)(ui->comboBoxCHARSETop->currentData(Qt::UserRole).toInt());

    XDataConvertor::OPTIONS options = {};
    process(CMETHOD_CHARSET, methodConvertor, options);
}

void XDataConvertorWidget::on_pushButtonDumpInput_clicked()
{
    ui->widgetHexInput->dumpMemory(tr("Input"));
}

void XDataConvertorWidget::on_pushButtonDumpOutput_clicked()
{
    ui->widgetHexOutput->dumpMemory(tr("Output"));
}
