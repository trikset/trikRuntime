/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <QApplication>

class TrikDEApplication : public QApplication
{
    Q_OBJECT
public:
    explicit TrikDEApplication(int &argc, char **argv);
    bool notify(QObject *receiver, QEvent *event);
};
