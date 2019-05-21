#pragma once

#include <QVBoxLayout>
#include <QLabel>

class NetworkWidget : public QWidget
{
public:
	NetworkWidget(QWidget *parent = 0);

private:
	QVBoxLayout mNetworkLayout;
	QLabel mHostnameLabel;
	QLabel mIPLabel;
};
