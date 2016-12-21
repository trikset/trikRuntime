/* Copyright 2014 Roman Kurbatov
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

#include "motorsWidget.h"

#include <QtCore/QTimer>
#include <QtGui/QKeyEvent>
#include <QDebug>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QVBoxLayout>
	#include <QtGui/QScrollArea>
#else
	#include <QtWidgets/QVBoxLayout>
	#include <QtWidgets/QScrollArea>
#endif

#include <QApplication> 

#include "motorLever.h"

using namespace trikGui;

MotorsWidget::MotorsWidget(trikControl::BrickInterface &brick
		, trikControl::MotorInterface::Type type
		, QWidget *parent)
	: TrikGuiDialog(parent)
	, mBrick(brick)
	, mPorts(mBrick.motorPorts(type))
	, mLevers(mPorts.size())
	, mScrollArea(new QScrollArea(this))
{
	mScrollArea->setFrameStyle(QFrame::NoFrame);
	mScrollArea->setWidgetResizable(true);
	mScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	mScrollArea->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	mScrollArea->installEventFilter(this);

	const auto leversWidget = new QWidget(mScrollArea);
	const auto innerLayout = new QVBoxLayout();
	const auto outerLayout = new QHBoxLayout();

	mPorts.sort();

	int i = 0;
	for (const QString &port : mPorts) {
		MotorLever *lever = new MotorLever(port, *mBrick.motor(port), leversWidget);
		innerLayout->addWidget(lever);
		mLevers[i] = lever;
		++i;
	}
	leversWidget->setLayout(innerLayout);
	outerLayout->addWidget(mScrollArea);
	this->setLayout(outerLayout);
	mScrollArea->setWidget(leversWidget);

	QTimer::singleShot(0, this, SLOT(fixLeversPosition()));
}

MotorsWidget::~MotorsWidget()
{
	qDeleteAll(mLevers);
}

QString MotorsWidget::menuEntry(trikControl::MotorInterface::Type type)
{
	switch (type) {
	case trikControl::MotorInterface::Type::powerMotor: {
		return tr("Test power motors");
	}
	case trikControl::MotorInterface::Type::servoMotor: {
		return tr("Test servo motors");
	}
	}

	return QString();
}

void MotorsWidget::renewFocus()
{
	if (!mLevers.isEmpty()) {
		mLevers.first()->setFocus();
	}
}

bool MotorsWidget::eventFilter(QObject *, QEvent *event)
{
	if(event->type() == QEvent::KeyPress)
	{
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

		switch (keyEvent->key()) {
			case Qt::Key_Up: {
				focusPreviousChild();
				ensureFocusedWidgetVisible();
				break;
			}
			case Qt::Key_Down: {
				focusNextChild();
				ensureFocusedWidgetVisible();
				break;
			}
		}
	}

	return false;
}


void MotorsWidget::keyPressEvent(QKeyEvent *event)
{
	TrikGuiDialog::keyPressEvent(event);
}

void MotorsWidget::fixLeversPosition()
{
	focusNextChild();
	focusPreviousChild();
}

void MotorsWidget::ensureFocusedWidgetVisible()
{
	QWidget *focusedWidget = qApp->focusWidget();
	mScrollArea->ensureWidgetVisible(focusedWidget);
}
