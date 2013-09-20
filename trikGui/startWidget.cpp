#include "startWidget.h"

#include <QtGui/QKeyEvent>

//#include "fileManagerWidget.h"
//#include "showNetConfigWidget.h"

using namespace trikGui;

StartWidget::StartWidget(QWidget *parent)
	: QWidget(parent)
{
	mExitItemTitle = "Exit";

	setWindowState(Qt::WindowFullScreen);

	mTitleLabel.setText("TRIK");

//	mMenuItems.append(new QStandardItem(FileManagerWidget::getTitle()));
//	mMenuItems.append(new QStandardItem(ShowNetConfigWidget::getTitle()));
	mMenuItems.append(new QStandardItem(mExitItemTitle));

	mMenuModel.appendColumn(mMenuItems);

	mMenuView.setModel(&mMenuModel);

	mLayout.addWidget(&mTitleLabel);
	mLayout.addWidget(&mMenuView);

	setLayout(&mLayout);
}

StartWidget::~StartWidget()
{
}

void StartWidget::launch()
{
	QString const &currentItemText = mMenuModel.itemFromIndex(mMenuView.currentIndex())->text();
//	if (currentItemText == FileManagerWidget::getTitle()) {
//		FileManagerWidget *fileManagerWidget = new FileManagerWidget;
//		fileManagerWidget->show();
//	}
//	else if (currentItemText == ShowNetConfigWidget::getTitle()) {
//		ShowNetConfigWidget *showNetConfigWidget = new ShowNetConfigWidget;
//		showNetConfigWidget->show();
//	} else
		if (currentItemText == mExitItemTitle)
			close();
}

void StartWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_Meta: {
			break;
		}
		case Qt::Key_Enter: {
			launch();
			break;
		}
		default: {
			QWidget::keyPressEvent(event);
			break;
		}
	}
}
