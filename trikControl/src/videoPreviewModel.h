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

#include <QtCore/QAbstractListModel>
#include <QtCore/QVector>

namespace trikControl {

/// Incremental list model over the ids of the currently open previews.
///
/// VideoPreviewHub owns one instance and feeds it through its previewOpened()/
/// previewClosed() signals. The model therefore only performs row insertions and
/// removals, so views such as Repeater keep untouched delegates alive instead of
/// rebuilding the whole list when a sensor is opened or stopped.
///
/// The model lives in the hub's thread (normally the GUI thread). When the hub
/// methods are called from a worker thread, Qt delivers the signals to the model
/// through a queued connection, so begin/endInsertRows always run in the model's
/// thread.
class VideoPreviewModel : public QAbstractListModel
{
	Q_OBJECT

public:
	enum Roles {
		/// Opaque id returned by VideoPreviewSink::openPreview().
		PreviewIdRole = Qt::UserRole + 1
	};

	explicit VideoPreviewModel(QObject *parent = nullptr);

	int rowCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	QHash<int, QByteArray> roleNames() const override;

public Q_SLOTS:
	/// Appends @p id to the model.
	void addId(int id);

	/// Removes @p id from the model if present.
	void removeId(int id);

private:
	QVector<int> mIds;
};

} // namespace trikControl
