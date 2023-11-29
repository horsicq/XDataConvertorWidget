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
#ifndef XDATACONVERTORWIDGET_H
#define XDATACONVERTORWIDGET_H

#include <QWidget>
#include <QResizeEvent>

#include "xbinary.h"
#include "xoptions.h"
#include "xhexview.h"

namespace Ui {
class XDataConvertorWidget;
}

class XDataConvertorWidget : public QWidget
{
    Q_OBJECT

    enum METHOD {
        METHOD_UNKNOWN,
        METHOD_NONE,
        METHOD_XOR
    };

    struct DATA {
        bool bValid;
    };

public:
    explicit XDataConvertorWidget(QWidget *pParent = nullptr);
    ~XDataConvertorWidget();

    void setData(QIODevice *pDevice);

private slots:
    void on_listWidgetMethods_itemClicked(QListWidgetItem *pItem);
    void on_listWidgetMethods_currentItemChanged(QListWidgetItem *pCurrent, QListWidgetItem *pPrevious);

private:
    void _addMethod(QString sName, METHOD method);
    void showMethod(METHOD method);

private:
    Ui::XDataConvertorWidget *ui;
    METHOD g_currentMethod;
    QIODevice *g_pDevice;
    XHexView::OPTIONS g_hexOptions;
    QMap<METHOD, DATA> g_mapData;
};

#endif // XDATACONVERTORWIDGET_H
