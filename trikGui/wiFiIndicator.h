#pragma once

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QLabel>
#else
	#include <QtWidgets/QLabel>
#endif

#include <wiFiModeWidget.h>

namespace trikGui {

class WiFiIndicator : public QLabel
{
public:
	explicit WiFiIndicator(QWidget *parent = 0);

signals:

public slots:
	void setOn();

	void setOff();

	void setAPOn();

	void changeMode(WiFiModeWidget::Mode mode, bool connected);

private:
	WiFiModeWidget::Mode mMode;
};

}
