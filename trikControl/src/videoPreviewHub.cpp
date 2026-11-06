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

#include <trikControl/videoPreviewHub.h>

#include <algorithm>

#include <QtCore/QMutexLocker>

using namespace trikControl;

VideoPreviewHub::VideoPreviewHub(QObject *parent)
	: VideoPreviewSource(parent)
{
}

void VideoPreviewHub::rebuildActiveIdsLocked()
{
	auto ids = mEntries.keys();
	std::sort(ids.begin(), ids.end());

	mActiveIds.clear();
	mActiveIds.reserve(ids.size());
	for (int id : ids) {
		mActiveIds.append(id);
	}
}

int VideoPreviewHub::openPreview()
{
	auto id = -1;
	{
		QMutexLocker locker(&mMutex);
		id = mNextId++;
		mEntries.insert(id, Entry {});
		rebuildActiveIdsLocked();
	}

	Q_EMIT activeChanged();
	return id;
}

void VideoPreviewHub::closePreview(int id)
{
	auto changed = false;
	{
		QMutexLocker locker(&mMutex);
		if (mEntries.remove(id) > 0) {
			rebuildActiveIdsLocked();
			changed = true;
		}
	}

	if (changed) {
		Q_EMIT activeChanged();
	}
}

void VideoPreviewHub::publish(int id, const VideoPreviewFrame &frame)
{
	{
		QMutexLocker locker(&mMutex);
		auto it = mEntries.find(id);
		if (it == mEntries.end()) {
			return;
		}

		it->frame = frame;
		++it->publishSequence;
	}

	Q_EMIT frameReady(id);
}

VideoPreviewFrame VideoPreviewHub::latest(int id, quint64 *sequence) const
{
	QMutexLocker locker(&mMutex);
	auto it = mEntries.constFind(id);
	if (it == mEntries.constEnd()) {
		if (sequence) {
			*sequence = 0;
		}
		return {};
	}

	if (sequence) {
		*sequence = it->publishSequence;
	}
	return it->frame;
}

QVariantList VideoPreviewHub::activeIds() const
{
	QMutexLocker locker(&mMutex);
	return mActiveIds;
}
