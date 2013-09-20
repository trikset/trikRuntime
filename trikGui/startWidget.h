#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtGui/QStandardItem>
#include <QtGui/QStandardItemModel>
#include <QtCore/QList>
#include <QtCore/QString>

namespace trikGui {

class StartWidget : public QWidget
{
	Q_OBJECT

public:
	explicit StartWidget(QWidget *parent = 0);
	~StartWidget();

protected:
	void keyPressEvent(QKeyEvent *event);

private:
	void launch();

	QString mExitItemTitle;
	QVBoxLayout mLayout;
	QLabel mTitleLabel;
	QListView mMenuView;
	QStandardItemModel mMenuModel;
	QList<QStandardItem *> mMenuItems;
};

}
