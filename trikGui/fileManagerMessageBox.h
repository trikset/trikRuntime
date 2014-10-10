#pragma once

#include <QtCore/qglobal.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QMessageBox>
	#include <QtGui/QPushButton>
#else
	#include <QtWidgets/QMessageBox>
	#include <QtWidgets/QPushButton>
#endif

#include <QtCore/QString>

namespace trikGui {

class FileManagerMessageBox : public QMessageBox
{
	Q_OBJECT
public:
	enum class FileState {
		None,
		Open,
		Delete
	};

	explicit FileManagerMessageBox(QWidget *parent = 0);
	~FileManagerMessageBox();

	void showMessage();
	FileState userAnswer();

protected:
	void keyPressEvent(QKeyEvent *event) override;

private:
	void init();

	QPushButton* mOpenButton;
	QPushButton* mDeleteButton;
	bool mEscStatus;
};

}

