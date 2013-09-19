/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "fileManagerWidget.h"
#include <QCoreApplication>
#include <QKeyEvent>
#include <QProcess>

FileManagerWidget::FileManagerWidget(QWidget *parent) :
    QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowState(Qt::WindowFullScreen);

    fileSystemModel.setRootPath("/");
    fileSystemModel.setFilter(QDir::AllEntries | QDir::System | QDir::NoDot);

    fileSystemView.setModel(&fileSystemModel);

    layout.addWidget(&currentPathLabel);
    layout.addWidget(&fileSystemView);
    setLayout(&layout);

    showCurrentDir();
}

FileManagerWidget::~FileManagerWidget()
{
}

const QString &FileManagerWidget::getTitle()
{
    static const QString title("File Manager");
    return title;
}

void FileManagerWidget::open()
{
     const QModelIndex &index = fileSystemView.currentIndex();
     if (fileSystemModel.isDir(index))
     {
         QDir::setCurrent(fileSystemModel.filePath(index));
         showCurrentDir();
     }
     else
         QProcess::startDetached(fileSystemModel.filePath(index));
}

void FileManagerWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Meta:
        {
            close();
            break;
        }
        case Qt::Key_Enter:
        {
            open();
            break;
        }
        default:
        {
            QWidget::keyPressEvent(event);
            break;
        }
    }
}

void FileManagerWidget::showCurrentDir()
{
    currentPathLabel.setText(QDir::currentPath());
    fileSystemView.setRootIndex(fileSystemModel.index(QDir::currentPath()));
}
