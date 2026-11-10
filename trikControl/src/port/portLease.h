/* Copyright 2026 CyberTech Labs Ltd.
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

#include <QtCore/QString>
#include <QtGlobal>

namespace trikControl {

class Port;
class PortManager;

/// RAII handle to an acquired port.
///
/// Devices hold a lease instead of touching the Port directly: the port is
/// released automatically when the lease is destroyed, and the owner token
/// prevents an old lease from releasing a port already acquired by someone
/// else.
///
/// A lease is movable but not copyable, so exactly one object owns the
/// acquired port at any time.
class PortLease
{
	Q_DISABLE_COPY(PortLease)

public:
	/// Creates an empty lease that does not own a port.
	PortLease() = default;

	/// Takes the acquired port from @p other; @p other becomes empty.
	PortLease(PortLease &&other) noexcept;

	/// Releases the currently held port, then takes it from @p other.
	PortLease &operator=(PortLease &&other) noexcept;

	/// Releases the held port, if any.
	~PortLease();

	/// Returns true when the lease holds an acquired port.
	explicit operator bool() const;

	/// Returns the mode-specific device reference: PWM controller name for
	/// Pwm, serial device for Uart and so on. Empty when the lease is empty.
	QString device() const;

	/// Releases the port early. The lease becomes empty; calling it on an
	/// already empty lease is a no-op.
	void release();

private:
	friend class PortManager;

	/// Creates a lease for an already acquired @p port. Used by PortManager.
	/// @param port - acquired port, must stay alive while the lease exists.
	/// @param token - owner token returned by Port::start().
	/// @param device - mode-specific device reference stored in the lease.
	PortLease(Port *port, quint64 token, QString device);

	/// Clears the lease fields without calling Port::stop(). Used after the
	/// port has been released or transferred to another lease.
	void clear();

	/// Acquired port, null when the lease is empty.
	Port *mPort = nullptr;

	/// Owner token returned by Port::start() for this lease.
	quint64 mToken = 0;

	/// Mode-specific device reference returned by device().
	QString mDevice;
};

}
