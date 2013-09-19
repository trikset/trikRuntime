/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "startWidget.h"
#include <QKeyEvent>
#include <QCoreApplication>
#include "fileManagerWidget.h"
#include "showNetConfigWidget.h"

StartWidget::StartWidget(QWidget *parent)
    : QWidget(parent)
{
    exitItemTitle = "Exit";

    setWindowState(Qt::WindowFullScreen);

    titleLabel.setText("TrikDE");

    menuItems.append(new QStandardItem(FileManagerWidget::getTitle()));
    menuItems.append(new QStandardItem(ShowNetConfigWidget::getTitle()));
    menuItems.append(new QStandardItem(exitItemTitle));

    menuModel.appendColumn(menuItems);

    menuView.setModel(&menuModel);

    layout.addWidget(&titleLabel);
    layout.addWidget(&menuView);

    setLayout(&layout);
}

StartWidget::~StartWidget()
{
}

void StartWidget::launch()
{
    const QString &currentItemText = menuModel.itemFromIndex(menuView.currentIndex())->text();
    if (currentItemText == FileManagerWidget::getTitle())
    {
        FileManagerWidget *fileManagerWidget = new FileManagerWidget;
        fileManagerWidget->show();
    }
    else if (currentItemText == ShowNetConfigWidget::getTitle())
    {
        ShowNetConfigWidget *showNetConfigWidget = new ShowNetConfigWidget;
        showNetConfigWidget->show();
    }
    else if (currentItemText == exitItemTitle)
        close();
}

void StartWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Meta:
        {
            break;
        }
        case Qt::Key_Enter:
        {
            launch();
            break;
        }
        default:
        {
            QWidget::keyPressEvent(event);
            break;
        }
    }
}
