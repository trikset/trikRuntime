/* Copyright 2015 CyberTech Labs Ltd.
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

#include "lazyMainWidgetWrapper.h"

#include <QsLog.h>

using namespace trikGui;

LazyMainWidgetWrapper::LazyMainWidgetWrapper(trikControl::DisplayWidgetInterface *wrappedWidget, QWidget *parent)
	: LazyMainWidget(parent)
	, mWrappedWidget(wrappedWidget)
	, mLayout(new QHBoxLayout(this))
{
	connect(wrappedWidget, SIGNAL(shown()), this, SLOT(emitShowMe()));
	connect(wrappedWidget, SIGNAL(hidden()), this, SIGNAL(hideMe()));

	mLayout->addWidget(mWrappedWidget);
}

void LazyMainWidgetWrapper::renewFocus()
{
	mWrappedWidget->setFocus();
}

LazyMainWidgetWrapper::~LazyMainWidgetWrapper()
{
	mLayout->removeWidget(mWrappedWidget);
	mWrappedWidget->setParent(nullptr);
}

void LazyMainWidgetWrapper::emitShowMe()
{
	emit showMe(*this);
}
