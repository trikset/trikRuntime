/* Copyright 2014 CyberTech Labs Ltd.
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

#include "systemSettingsWidget.h"

#include <QtGui/QKeyEvent>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QLabel>
#else
	#include <QtWidgets/QButtonGroup>
	#include <QtWidgets/QLabel>
#endif

using namespace trikGui;

SystemSettingsWidget::SystemSettingsWidget(MainWidget::FileManagerRootType fileManagerRoot, QWidget *parent)
	: TrikGuiDialog(parent)
{
	QLabel* const allFilesLabel = new QLabel(tr("Directory 'Files' is ..."));
	allFilesLabel->setAlignment(Qt::AlignCenter);

	mAllFSButton = new QRadioButton(tr("full file system"));
	mOnlyScriptsButton = new QRadioButton(tr("only 'scripts' directory"));

	setCurrentFilesRootButton(fileManagerRoot);

	mButtonGroup.addButton(mAllFSButton);
	mButtonGroup.addButton(mOnlyScriptsButton);

	mLayout.addWidget(allFilesLabel);
	mLayout.addWidget(mOnlyScriptsButton);
	mLayout.addWidget(mAllFSButton);
	mLayout.addStretch(1);

	setLayout(&mLayout);
	setFocusPolicy(Qt::StrongFocus);
}

SystemSettingsWidget::~SystemSettingsWidget()
{
	delete mAllFSButton;
	delete mOnlyScriptsButton;
}

void SystemSettingsWidget::setCurrentFilesRootButton(MainWidget::FileManagerRootType fileManagerRoot)
{
	if (fileManagerRoot == MainWidget::FileManagerRootType::allFS) {
		mAllFSButton->setChecked(true);
		mOnlyScriptsButton->setChecked(false);
	} else {
		mAllFSButton->setChecked(false);
		mOnlyScriptsButton->setChecked(true);
	}
}

void SystemSettingsWidget::renewFocus()
{
}

QString SystemSettingsWidget::menuEntry()
{
	return QString(tr("System settings"));
}

void SystemSettingsWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_Escape: {
			emitCheckedDirPath();
			exit();
			break;
		}
		case Qt::Key_PowerOff: {
			emitCheckedDirPath();
			goHome();
			break;
		}
		case Qt::Key_Left:
		case Qt::Key_Up:
		case Qt::Key_Down:
		case Qt::Key_Right: {
			changeDefaultButton();
			break;
		}
		default: {
			MainWidget::keyPressEvent(event);
			break;
		}
	}
}

void SystemSettingsWidget::changeDefaultButton()
{
	QRadioButton const* const current = dynamic_cast<QRadioButton const*>(mButtonGroup.checkedButton());
	if (current == mOnlyScriptsButton) {
		mAllFSButton->setChecked(true);
		mOnlyScriptsButton->setChecked(false);
	} else {
		mOnlyScriptsButton->setChecked(true);
		mAllFSButton->setChecked(false);
	}
}

void SystemSettingsWidget::emitCheckedDirPath()
{
	QRadioButton const* const current = dynamic_cast<QRadioButton const*>(mButtonGroup.checkedButton());
	if (current == mOnlyScriptsButton) {
		emit currentFilesDirPath(MainWidget::FileManagerRootType::scriptsDir);
	} else {
		emit currentFilesDirPath(MainWidget::FileManagerRootType::allFS);
	}
}
