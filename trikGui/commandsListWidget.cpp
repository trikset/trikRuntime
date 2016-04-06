/* Copyright 2016 Mikhail Kita
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#include "commandsListWidget.h"

#include <QtGui/QKeyEvent>
#include <trikKernel/paths.h>
#include <QsLog.h>

#include "commandSettingsWidget.h"

using namespace trikGui;

CommandsListWidget::CommandsListWidget(const QString &name, QWidget *parent)
    : TrikGuiDialog(parent)
    , mTitle(tr("Choose command:"))
    , mData(0)
    , mValue(name)
{
    mCommands.addItem(tr("Play Tone"));
    mCommands.addItem(tr("Timer"));
    mCommands.addItem(tr("Clear"));

    mLayout.addWidget(&mTitle);
    mLayout.addWidget(&mCommands);

    mCommands.selectionModel()->select(
        mCommands.model()->index(0, 0)
        , QItemSelectionModel::ClearAndSelect
    );

    setLayout(&mLayout);
}

QString CommandsListWidget::menuEntry()
{
    return tr("Commands List");
}

void CommandsListWidget::renewFocus()
{
    mCommands.setFocus();
}

void CommandsListWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Return: {
            switch (mCommands.currentRow()) {
                case 0: {
                    mValue = QString("Play Tone");
                    break;
                }
                case 1: {
                    char title[] = "Choose waiting time (ms):";
                    CommandSettingsWidget commandSettingsWidget(title);
                    emit newWidget(commandSettingsWidget);
                    commandSettingsWidget.exec();

                    mData = commandSettingsWidget.getValue();
                    std::string res = "Delay " + std::to_string(mData) + " ms";
                    mValue = QString(res.c_str());
                    break;
                }
                default: {
                    mValue = QString("< add command >");
                    break;
                }
            }
            exit();
            break;
        }
        default: {
            TrikGuiDialog::keyPressEvent(event);
            break;
        }
    }
}

QString CommandsListWidget::getValue() {
    return mValue;
}

int CommandsListWidget::getData() {
    return mData;
}
