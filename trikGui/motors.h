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

#pragma once

#include <QtCore/qglobal.h>

#include <QAbstractListModel>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVector>

#include <trikControl/brickInterface.h>

#include <motorLever.h>

namespace trikGui {

class MotorLever;

/// Widget that allows to test motors connected to TRIK controller
class Motors : public QAbstractListModel
{
	Q_OBJECT

public:
	enum class MotorType {
		PowerMotor,
		ServoMotor,
	};

	/// Constructor
	/// @param brick - reference to trikControl::Brick instance.
	/// @param type - type of motors we want to test.
	/// @param parent - pointer to a parent widget.
	Motors(trikControl::BrickInterface &brick
				, QStringList ports
				, QObject *parent = nullptr);

	/// Destructor.
	~Motors() override;

	int rowCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	Q_INVOKABLE void setQmlParent(QObject *parent);

private:
	trikControl::BrickInterface &mBrick;
	QStringList mPorts;
	QVector<MotorLever *> mLevers; // Has ownership.
};

}
