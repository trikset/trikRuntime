#include "fileSystemFilter.h"

FileSystemFilter::FileSystemFilter(QObject *parent)
	: QSortFilterProxyModel(parent)
{
}

bool FileSystemFilter::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	QVariant leftData = sourceModel()->data(left);
	QVariant rightData = sourceModel()->data(right);
	if (leftData.toString() == tr("Delete all")) {
		return false;
	} else if (rightData.toString() == tr("Delete all")) {
		return true;
	} else {
		return QSortFilterProxyModel::lessThan(left, right);
	}
}
