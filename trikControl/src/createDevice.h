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
#include <QsLog.h>

#include "communicatorProvider.h"
#include "encoder.h"
#include "analogSensor.h"
#include "powerMotor.h"
#include "servoMotor.h"
#include "pwmCapture.h"
#include "transports/transportType.h"

namespace trikControl {

namespace detail {

template<template<typename> class D, typename I>
typename std::enable_if<SupportsCommunicator<D>::value, I *>::type createCommunicator(const QString &port,
	const trikKernel::Configurer &configurer, CommunicatorProvider &provider)
{
	return new D<CommunicatorPolicy>(port, configurer, provider.communicator());
}

template<template<typename> class D, typename I>
typename std::enable_if<!SupportsCommunicator<D>::value, I *>::type createCommunicator(const QString &port,
	const trikKernel::Configurer &, CommunicatorProvider &)
{
	QLOG_ERROR() << "Device on" << port << "does not support the communicator transport";
	return nullptr;
}

template<template<typename> class D, typename I>
typename std::enable_if<SupportsInternal<D>::value, I *>::type createInternal(const QString &port,
	const trikKernel::Configurer &configurer, CommunicatorProvider &provider)
{
	return new D<InternalPolicy>(port, configurer, provider.hw());
}

template<template<typename> class D, typename I>
typename std::enable_if<!SupportsInternal<D>::value, I *>::type createInternal(const QString &port,
	const trikKernel::Configurer &, CommunicatorProvider &)
{
	QLOG_ERROR() << "Device on" << port << "does not support the internal transport";
	return nullptr;
}

} // namespace detail

/// Unified factory for Encoder, AnalogSensor, PowerMotor and similar devices.
/// Looks up the transport policy via parseTransport, selects the right Policy tag
/// via SFINAE, and instantiates the corresponding specialization.
///
/// Example:
///   auto *sensor = makeDevice<AnalogSensor, SensorInterface>(port, configurer, *mCommunicatorProvider);
template<template<typename> class D, typename I>
I *makeDevice(const QString &port, const trikKernel::Configurer &configurer, CommunicatorProvider &provider)
{
	const TransportType tr = parseTransport(port);

	switch (tr) {
	case TransportType::Communicator:
		return detail::createCommunicator<D, I>(port, configurer, provider);
	case TransportType::Internal:
		return detail::createInternal<D, I>(port, configurer, provider);
	}

	QLOG_ERROR() << "Unsupported transport for device on port" << port;
	return nullptr;
}

} // namespace trikControl