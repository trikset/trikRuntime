#include "fileManagerMessageBox.h"

#include <QtGui/QKeyEvent>

using namespace trikGui;

FileManagerMessageBox::FileManagerMessageBox(QWidget *parent)
	: QMessageBox(parent)
{
	init();
}

FileManagerMessageBox::~FileManagerMessageBox()
{
	delete mDeleteButton;
	delete mOpenButton;
}

void FileManagerMessageBox::init()
{
	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);
	setIcon(QMessageBox::Question);
	setText(tr("Do you want to open or delete the file?"));

	mOpenButton = addButton(tr("Open"), QMessageBox::AcceptRole);
	mDeleteButton = addButton(tr("Delete"), QMessageBox::DestructiveRole);
}

void FileManagerMessageBox::showMessage()
{
	mEscStatus = false;
	setDefaultButton(mOpenButton);
	exec();
}

FileManagerMessageBox::FileState FileManagerMessageBox::userAnswer()
{
	if (!mEscStatus) {
		QAbstractButton const* const button = clickedButton();
		if (button == mOpenButton) {
			return FileState::Open;
		} else if (button == mDeleteButton) {
			return FileState::Delete;
		}
	}

	return FileState::None;
}

void FileManagerMessageBox::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_Escape: {
			mEscStatus = true;
			reject();
			break;
		}
		case Qt::Key_Left:
		case Qt::Key_Right: {
			changeDefaultButton();
			break;
		}
		default: {
			QMessageBox::keyPressEvent(event);
			break;
		}
	}
}

void FileManagerMessageBox::changeDefaultButton()
{
	QPushButton* current = defaultButton();
	if (current == mOpenButton) {
		setDefaultButton(mDeleteButton);
	} else {
		setDefaultButton(mOpenButton);
	}
}
