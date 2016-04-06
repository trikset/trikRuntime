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

#include "programmingWidget.h"

#include <QtGui/QKeyEvent>
#include <trikKernel/paths.h>
#include <QsLog.h>

#include "commandsListWidget.h"

using namespace trikGui;

ProgrammingWidget::ProgrammingWidget(Controller &controller, QWidget *parent)
    : TrikGuiDialog(parent)
    , mTitle(tr("Add commands to list"))
    , mRunButton(nullptr)
    , mController(controller)
    , mEmptyCommandsCounter(1)
    , mScript(script.c_str())
{
    const auto scrollArea = new QScrollArea(this);
    scrollArea->setFrameStyle(QFrame::NoFrame);
    scrollArea->setWidgetResizable(true);
    scrollArea->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    const auto leversWidget = new QWidget(this);
    const auto innerLayout = new QVBoxLayout();
    const auto outerLayout = new QHBoxLayout();

    mCommands.addItem(tr("< add command >"));

    innerLayout->addWidget(&mTitle);
    innerLayout->addWidget(&mCommands);

    mCommands.selectionModel()->select(
        mCommands.model()->index(0, 0)
        , QItemSelectionModel::ClearAndSelect
    );

    mRunButton = new QPushButton(tr("Run program"));
    innerLayout->addWidget(mRunButton);
    mRunButton->setDefault(false);

    leversWidget->setLayout(innerLayout);
    outerLayout->addWidget(scrollArea);
    setLayout(outerLayout);
    scrollArea->setWidget(leversWidget);
}

ProgrammingWidget::~ProgrammingWidget() {
    delete mRunButton;
}

QString ProgrammingWidget::menuEntry()
{
    return tr("Programming");
}

void ProgrammingWidget::renewFocus()
{
    mCommands.setFocus();
}

void ProgrammingWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Return: {
            if (mCommands.hasFocus()) {
                QString text(mCommands.currentItem()->text());
                CommandsListWidget commandsListWidget(text);
                emit newWidget(commandsListWidget);
                commandsListWidget.exec();

                QString value(commandsListWidget.getValue());
                mCommands.currentItem()->setText(value);

                if (value == "< add command >") {
                    mEmptyCommandsCounter++;
                } else if (text == "< add command >") {
                    mEmptyCommandsCounter--;
                }

                if (mEmptyCommandsCounter == 0) {
                    mCommands.addItem(tr("< add command >"));
                    mEmptyCommandsCounter++;
                    addToScript(value, commandsListWidget.getData());
                }
            } else if (mRunButton->hasFocus()) {
                mScript.append(QString("    return;\n}"));
                mController.runScript(mScript);
            }
            break;
        }
        default: {
            TrikGuiDialog::keyPressEvent(event);
            break;
        }
    }
}

void ProgrammingWidget::addToScript(QString command, int data) {
    if (command.startsWith("Play Tone")) {
        mScript.append(QString("    brick.playSound(\"media/beep.wav\");\n"));
    } else if (command.startsWith("Delay")) {
        std::string res("    script.wait(" + std::to_string(data) + ");\n");
        mScript.append(QString(res.c_str()));
    }
}
