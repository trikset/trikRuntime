/* Copyright 2016 Mikhail Kita
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

#include "scriptHolder.h"

#include <trikKernel/paths.h>
#include <QsLog.h>

using namespace trikGui;

ScriptHolder* ScriptHolder::instance()
{
	static ScriptHolder* holder = new ScriptHolder();
	return holder;
}

const QStringList &ScriptHolder::titles() const
{
	return mTitles;
}

const QStringList &ScriptHolder::commands() const
{
	return mCommands;
}

int ScriptHolder::size() const
{
	return mTitles.length();
}

void ScriptHolder::setData(const QString &title, const QString &command)
{
	mTitles.push_back(title);
	mCommands.push_back(command);
}

void ScriptHolder::clear()
{
	mTitles.clear();
	mCommands.clear();
}
