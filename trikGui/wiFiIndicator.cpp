#include "wiFiIndicator.h"

using namespace trikGui;

WiFiIndicator::WiFiIndicator(QWidget *parent)
	: QLabel(parent)
{}

void WiFiIndicator::setOn()
{
	if (mMode == WiFiModeWidget::client) {
		QPixmap icon("://resources/wifion.png");
		setPixmap(icon);
	}
}

void WiFiIndicator::setOff()
{
	QPixmap icon("://resources/wifioff.png");
	setPixmap(icon);
}

void WiFiIndicator::setAPOn()
{
	QPixmap icon("://resources/wifipoint.png");
	setPixmap(icon);
}

void WiFiIndicator::changeMode(WiFiModeWidget::Mode mode, bool connected)
{
	mMode = mode;

	switch (mode) {
		case WiFiModeWidget::client:
			if (connected) {
				setOn();
			} else {
				setOff();
			}
			break;
		case WiFiModeWidget::accessPoint:
			setAPOn();
			break;
		default:
			setOff();
	}
}
