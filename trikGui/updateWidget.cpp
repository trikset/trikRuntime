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

#include "updateWidget.h"

using namespace trikGui;

UpdateWidget::UpdateWidget(QWidget *parent)
	: TrikGuiDialog(parent)
{
	QLabel* const startLabel = new QLabel(tr("Update is started..."));
	startLabel->setAlignment(Qt::AlignCenter);
	mLayout.addWidget(startLabel);

	setLayout(&mLayout);
	setFocusPolicy(Qt::StrongFocus);
}

UpdateWidget::~UpdateWidget()
{
}

void UpdateWidget::renewFocus()
{
}

QString UpdateWidget::menuEntry()
{
	return QString(tr("Update"));
}
