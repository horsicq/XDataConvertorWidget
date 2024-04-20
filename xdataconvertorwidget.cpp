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
    ui->lineEditSizeInput->setReadOnly(true);
    ui->lineEditSizeOutput->setReadOnly(true);
    ui->lineEditEntropyInput->setReadOnly(true);
    ui->lineEditEntropyOutput->setReadOnly(true);

    ui->listWidgetMethods->blockSignals(true);

    _addMethod(QString(""), CMETHOD_NONE);
    _addMethod(QString("XOR"), CMETHOD_XOR);
    _addMethod(QString("ADD/SUB"), CMETHOD_ADDSUB);
    _addMethod(QString("Base64"), CMETHOD_BASE64);

    XOptions::adjustListWidget(ui->listWidgetMethods);

    ui->listWidgetMethods->blockSignals(false);

    g_hexOptions = {};
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
}

XDataConvertorWidget::~XDataConvertorWidget()
{
    delete ui;
}

void XDataConvertorWidget::setData(QIODevice *pDevice)
{
    g_pDevice = pDevice;

    ui->widgetHexInput->setData(pDevice, g_hexOptions, true);

    XDataConvertor::OPTIONS options = {};
    process(CMETHOD_NONE, XDataConvertor::CMETHOD_NONE, options);

    ui->listWidgetMethods->setCurrentRow(0);
}

void XDataConvertorWidget::_addMethod(QString sName, CMETHOD method)
{
    QListWidgetItem *pItem = new QListWidgetItem(sName);
    pItem->setData(Qt::UserRole, method);

    ui->listWidgetMethods->addItem(pItem);

    XDataConvertor::DATA _data = {};
    _data.bValid = false;

    g_mapData.insert(method, _data);
}

void XDataConvertorWidget::showMethod(CMETHOD method)
{
    XDataConvertor::DATA _data = g_mapData.value(method);

    ui->lineEditEntropyOutput->setValue_double(_data.dEntropy);

    if (method == CMETHOD_NONE) {
        ui->lineEditEntropyInput->setValue_double(_data.dEntropy);
        ui->lineEditSizeInput->setValue_uint64(g_pDevice->size(), XLineEditHEX::_MODE_SIZE);
        ui->widgetHexOutput->setData(g_pDevice, g_hexOptions, true);
        ui->lineEditSizeOutput->setValue_uint64(g_pDevice->size(), XLineEditHEX::_MODE_SIZE);
    } else if (_data.bValid) {
        ui->widgetHexOutput->setData(_data.pTmpFile, g_hexOptions, true);
        ui->lineEditSizeOutput->setValue_uint64(_data.pTmpFile->size(), XLineEditHEX::_MODE_SIZE);
    } else {
        ui->widgetHexOutput->setDevice(nullptr);
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
    }
}

void XDataConvertorWidget::process(CMETHOD method, XDataConvertor::CMETHOD methodConvertor, const XDataConvertor::OPTIONS &options)
{    
    XDataConvertor::DATA _data = {};

    DialogXDataConvertorProcess dcp(this);
    dcp.setData(g_pDevice, &_data, methodConvertor, options);

    if (dcp.showDialogDelay() == QDialog::Accepted) {
        if (g_mapData[method].pTmpFile) {
            delete g_mapData[method].pTmpFile;
        }

        g_mapData[method] = _data;
    }

    showMethod(method);
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

void XDataConvertorWidget::on_pushButtonDumpInput_clicked()
{
    ui->widgetHexInput->dumpMemory(tr("Input"));
}

void XDataConvertorWidget::on_pushButtonDumpOutput_clicked()
{
    ui->widgetHexOutput->dumpMemory(tr("Output"));
}
