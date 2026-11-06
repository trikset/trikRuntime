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

#include "videoPreviewModel.h"

using namespace trikControl;

VideoPreviewModel::VideoPreviewModel(QObject *parent)
	: QAbstractListModel(parent)
{
}

int VideoPreviewModel::rowCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : mIds.size();
}

QVariant VideoPreviewModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= mIds.size()) {
		return {};
	}

	if (role == PreviewIdRole || role == Qt::DisplayRole) {
		return mIds.at(index.row());
	}

	return {};
}

QHash<int, QByteArray> VideoPreviewModel::roleNames() const
{
	return {{PreviewIdRole, QByteArrayLiteral("streamId")}};
}

void VideoPreviewModel::addId(int id)
{
	const auto row = mIds.size();
	beginInsertRows(QModelIndex(), row, row);
	mIds.append(id);
	endInsertRows();
}

void VideoPreviewModel::removeId(int id)
{
	const auto row = mIds.indexOf(id);
	if (row < 0) {
		return;
	}

	beginRemoveRows(QModelIndex(), row, row);
	mIds.removeAt(row);
	endRemoveRows();
}
