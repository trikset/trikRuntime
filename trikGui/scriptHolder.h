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

#pragma once

#include <QtCore/qglobal.h>

#include <QtCore/QString>
#include <QtCore/QStringList>
namespace trikGui {

/// Class which contains program from programming widget.
class ScriptHolder
{
public:
	/// Creates instance of ScriptHolder.
	static ScriptHolder* instance();

	/// Returns list which contains titles of commands.
	const QStringList &titles() const;

	/// Returns list which contains scripts for commands.
	const QStringList &commands() const;

	/// Returns number of stored commands.
	int size() const;

	/// Adds given strings to appropriate list.
	/// @param title - new string for titles list.
	/// @param command - new string for commands list.
	void setData(const QString &title, const QString &command);

	/// Clears all stored data.
	void clear();

private:
	/// Constructor.
	ScriptHolder() {}

	ScriptHolder(ScriptHolder const&) = delete;
	ScriptHolder& operator=(ScriptHolder const&) = delete;

	QStringList mTitles;
	QStringList mCommands;
};

}
