#pragma once

#include <QVBoxLayout>
#include <QLabel>

namespace trikGui {

class NetworkWidget : public QWidget
{
	Q_OBJECT
public:
	NetworkWidget(QWidget *parent = 0);

private slots:
	void updateIP();
	void updateHostname();

private:
	QVBoxLayout mNetworkLayout;
	QHBoxLayout mNameLayout;
	QHBoxLayout mIPLayout;

	QLabel mHostnameLabel;
	QLabel mIPLabel;

	QTimer mUpdateTimer;
};

}
