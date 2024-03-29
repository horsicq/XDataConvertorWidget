/* Copyright (c) 2020-2024 hors<horsicq@gmail.com>
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

XDataConvertorWidget::XDataConvertorWidget(QWidget *pParent) : QWidget(pParent), ui(new Ui::XDataConvertorWidget)
{
    ui->setupUi(this);

    g_pDevice = nullptr;
    g_currentMethod = CMETHOD_UNKNOWN;
    ui->lineEditSizeOriginal->setReadOnly(true);
    ui->lineEditSizeConverted->setReadOnly(true);
    ui->lineEditEntropyOriginal->setReadOnly(true);
    ui->lineEditEntropyConverted->setReadOnly(true);

    ui->listWidgetMethods->blockSignals(true);

    _addMethod(QString(""), CMETHOD_NONE);
    _addMethod(QString("XOR"), CMETHOD_XOR);
    _addMethod(QString("ADD/SUB"), CMETHOD_ADDSUB);

    XOptions::adjustListWidget(ui->listWidgetMethods);

    ui->listWidgetMethods->blockSignals(false);

    g_hexOptions = {};
    ui->widgetHex->setContextMenuEnable(false);

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
}

XDataConvertorWidget::~XDataConvertorWidget()
{
    delete ui;
}

void XDataConvertorWidget::setData(QIODevice *pDevice)
{
    g_pDevice = pDevice;

    ui->lineEditSizeOriginal->setValue_uint64(pDevice->size(), XLineEditHEX::_MODE_SIZE);
    ui->lineEditSizeConverted->setValue_uint64(pDevice->size(), XLineEditHEX::_MODE_SIZE);

    double dEntropy = XBinary::getEntropy(pDevice);

    ui->lineEditEntropyOriginal->setValue_double(dEntropy);
    ui->lineEditEntropyConverted->setValue_double(dEntropy);
}

void XDataConvertorWidget::_addMethod(QString sName, CMETHOD method)
{
    QListWidgetItem *pItem = new QListWidgetItem(sName);
    pItem->setData(Qt::UserRole, method);

    ui->listWidgetMethods->addItem(pItem);

    DATA _data = {};
    _data.bValid = (method == CMETHOD_NONE);

    g_mapData.insert(CMETHOD_NONE, _data);
}

void XDataConvertorWidget::showMethod(CMETHOD method)
{
    if (method != g_currentMethod) {
        g_currentMethod = method;

        DATA _data = g_mapData.value(method);

        if (method == CMETHOD_NONE) {
            ui->widgetHex->setData(g_pDevice, g_hexOptions, true);
            ui->lineEditSizeConverted->setValue_uint64(g_pDevice->size(), XLineEditHEX::_MODE_SIZE);
        } else if (_data.bValid) {
            ui->widgetHex->setData(_data.pTmpFile, g_hexOptions, true);
            ui->lineEditSizeConverted->setValue_uint64(_data.pTmpFile->size(), XLineEditHEX::_MODE_SIZE);
            ui->lineEditEntropyConverted->setValue_double(_data.dEntropy);
        } else {
            ui->widgetHex->setDevice(nullptr);
            ui->lineEditSizeConverted->setValue_uint64(0, XLineEditHEX::_MODE_SIZE);
            ui->lineEditEntropyConverted->setValue_double(0);
        }

        if (method == CMETHOD_NONE) {
            ui->stackedWidgetOptions->setCurrentWidget(ui->pageOriginal);
        } else if (method == CMETHOD_XOR) {
            ui->stackedWidgetOptions->setCurrentWidget(ui->pageXOR);
        } else if (method == CMETHOD_ADDSUB) {
            ui->stackedWidgetOptions->setCurrentWidget(ui->pageADDSUB);
        }
    }
}

void XDataConvertorWidget::process(CMETHOD method, XDataConvertor::CMETHOD methodConvertor, const XDataConvertor::OPTIONS &options)
{
    QTemporaryFile *pTmpFile = new QTemporaryFile;
    if (pTmpFile->open()) {
        // TODO
        // file.fileName() returns the unique file name
        DialogXDataConvertorProcess dcp(this);
        dcp.setData(g_pDevice, pTmpFile, methodConvertor, options);

        if (dcp.showDialogDelay() == QDialog::Accepted) {
            g_mapData[method].bValid = true;

            ui->widgetHex->setData(pTmpFile, g_hexOptions, true);

            if (g_mapData[method].pTmpFile) {
                delete g_mapData[method].pTmpFile;
            }

            g_mapData[method].pTmpFile = pTmpFile;
            g_mapData[method].dEntropy = XBinary::getEntropy(pTmpFile);
            ui->lineEditSizeConverted->setValue_uint64(pTmpFile->size(), XLineEditHEX::_MODE_SIZE);
            ui->lineEditEntropyConverted->setValue_double(g_mapData[method].dEntropy);
        }
    }
}

void XDataConvertorWidget::on_listWidgetMethods_itemClicked(QListWidgetItem *pItem)
{
    CMETHOD method = (CMETHOD)(pItem->data(Qt::UserRole).toInt());

    showMethod(method);
}

void XDataConvertorWidget::on_listWidgetMethods_currentItemChanged(QListWidgetItem *pCurrent, QListWidgetItem *pPrevious)
{
    Q_UNUSED(pPrevious)

    CMETHOD method = (CMETHOD)(pCurrent->data(Qt::UserRole).toInt());

    showMethod(method);
}

void XDataConvertorWidget::on_pushButtonDump_clicked()
{
    ui->widgetHex->dumpMemory();
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
