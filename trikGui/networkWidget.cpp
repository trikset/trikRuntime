#include "networkWidget.h"

#include <QWidget>

NetworkWidget::NetworkWidget(QWidget *parent)
	: QWidget(parent)
{
	mHostnameLabel.setText("Name: ");
	mIPLabel.setText("IP: ");
	mNetworkLayout.addWidget(&mHostnameLabel);
	mNetworkLayout.addWidget(&mIPLabel);
	setLayout(&mNetworkLayout);
}
