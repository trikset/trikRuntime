/* Copyright 2017 CyberTech Labs Ltd.
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

#include <QtCore/QObject>

#include "deviceInterface.h"

#include <trikControl/trikControlDeclSpec.h>

namespace trikControl {

/// Controls marker.
/// It is used only for 2d model in TS.
class TRIKCONTROL_EXPORT MarkerInterface : public QObject, public DeviceInterface
{
	Q_OBJECT

public slots:
	/// Moves the marker down to the floor.
	virtual void down(const QString &color) = 0;

	/// Lifts the robots marker up.
	/// The robot stops drawing its trace on the floor after that.
	virtual void up() = 0;

	/// Returns true if marker is currently active or false if not.
	virtual bool isDown() const = 0;

	/// Calls down() with black color if \a isDown is true or up() otherwise.
	virtual void setDown(bool isDown) = 0;
};

}

Q_DECLARE_METATYPE(trikControl::MarkerInterface *)
