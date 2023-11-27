/* Copyright (c) 2020-2023 hors<horsicq@gmail.com>
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

XDataConvertorWidget::XDataConvertorWidget(QWidget *pParent) :
    QWidget(pParent),
    ui(new Ui::XDataConvertorWidget)
{
    ui->setupUi(this);

    g_pDevice = nullptr;
    g_currentMethod = METHOD_UNKNOWN;

    ui->listWidgetMethods->blockSignals(true);

    _addMethod(QString(""), METHOD_NONE);
    _addMethod(QString("XOR"), METHOD_XOR);

    XOptions::adjustListWidget(ui->listWidgetMethods);

    ui->listWidgetMethods->blockSignals(false);
}

XDataConvertorWidget::~XDataConvertorWidget()
{
    delete ui;
}

void XDataConvertorWidget::setData(QIODevice *pDevice)
{
    g_pDevice = pDevice;
}

void XDataConvertorWidget::_addMethod(QString sName, METHOD method)
{
    QListWidgetItem *pItem = new QListWidgetItem(sName);
    pItem->setData(Qt::UserRole, method);

    ui->listWidgetMethods->addItem(pItem);

    DATA _data = {};
    _data.bValid = (method == METHOD_NONE);

    g_mapData.insert(METHOD_NONE, _data);
}

void XDataConvertorWidget::showMethod(METHOD method)
{
    if (method != g_currentMethod) {
        g_currentMethod = method;

        DATA _data = g_mapData.value(method);

        if (_data.bValid) {
            ui->stackedWidgetHex->setCurrentIndex(1);
        } else {
            ui->stackedWidgetHex->setCurrentIndex(0);
        }
    }
}

void XDataConvertorWidget::on_listWidgetMethods_itemClicked(QListWidgetItem *pItem)
{
    METHOD method = (METHOD)(pItem->data(Qt::UserRole).toInt());

    showMethod(method);
}

void XDataConvertorWidget::on_listWidgetMethods_currentItemChanged(QListWidgetItem *pCurrent, QListWidgetItem *pPrevious)
{
    Q_UNUSED(pPrevious)

    METHOD method = (METHOD)(pCurrent->data(Qt::UserRole).toInt());

    showMethod(method);
}
