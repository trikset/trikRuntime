#pragma once

#include <QVBoxLayout>
#include <QLabel>

namespace trikGui {

/// Widget, that shows current state of the network -- hostname and IP-address
class NetworkWidget : public QWidget
{
	Q_OBJECT
public:

	/// Constructor
	NetworkWidget(QWidget *parent = 0);

private slots:
	void updateIP();
	void updateHostname();

private:
	QVBoxLayout mNetworkLayout;

	QLabel mHostnameLabel;
	QLabel mIPLabel;

	QTimer mUpdateTimer;
};

}
