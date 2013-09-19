/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <QVBoxLayout>
#include <QLabel>
#include <QListView>
#include <QFileSystemModel>
#include <QString>

class FileManagerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileManagerWidget(QWidget *parent = 0);
    ~FileManagerWidget();
    static const QString &getTitle();
    
private:
    QVBoxLayout layout;
    QLabel currentPathLabel;
    QListView fileSystemView;
    QFileSystemModel fileSystemModel;

    void showCurrentDir();
    void open();

protected:
    void keyPressEvent(QKeyEvent *event);
};
