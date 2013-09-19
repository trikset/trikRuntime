/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QListView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QList>
#include <QString>

class StartWidget : public QWidget
{
    Q_OBJECT
    
public:
    StartWidget(QWidget *parent = 0);
    ~StartWidget();

private:
    QString exitItemTitle;
    QVBoxLayout layout;
    QLabel titleLabel;
    QListView menuView;
    QStandardItemModel menuModel;
    QList<QStandardItem *> menuItems;

    void launch();

protected:
    void keyPressEvent(QKeyEvent *event);
};
