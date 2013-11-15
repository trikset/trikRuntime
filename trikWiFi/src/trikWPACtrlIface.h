/*
 *  Copyright 2013 Roman Kurbatov
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <sys/un.h>
#include <QtCore/QObject>

#include "trikWiFi_global.h"

class TRIKWIFISHARED_EXPORT TrikWPACtrlIface : public QObject
{
	Q_OBJECT
public:
	TrikWPACtrlIface(const QString &interfaceFile, const QString &daemonFile, QObject *parent = 0);
	~TrikWPACtrlIface();
	int fileDescriptor();
	int attach();
	int detach();
	int request(const QString &command, QString &reply);
	bool isPending();
	int receive(QString &message);

private:
	int mSocket;
	struct sockaddr_un mLocal;
	struct sockaddr_un mDest;
};
