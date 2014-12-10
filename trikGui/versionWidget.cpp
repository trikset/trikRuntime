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

#include "versionWidget.h"
#include <trikKernel/version.h>

using namespace trikGui;

VersionWidget::VersionWidget(QWidget *parent)
	: TrikGuiDialog(parent)
{
	QLabel* versionLabel = new QLabel(tr("Current version") + " : \n" + trikKernel::version);
	versionLabel->setAlignment(Qt::AlignCenter);
	mLayout.addWidget(versionLabel);

	setLayout(&mLayout);
	setFocusPolicy(Qt::StrongFocus);
}

VersionWidget::~VersionWidget()
{
}

void VersionWidget::renewFocus()
{
	//setFocus();
}

QString VersionWidget::menuEntry()
{
	return QString(tr("Version"));
}
