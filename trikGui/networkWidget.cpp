#include "networkWidget.h"

#include <QWidget>
#include <QNetworkInterface>

#include <trikKernel/fileUtils.h>
#include <trikKernel/paths.h>

using namespace trikKernel;
using namespace trikGui;

NetworkWidget::NetworkWidget(QWidget *parent)
	: QWidget(parent)
{
	mHostnameLabel.setText("Name: ");
	mIPLabel.setText("IP: ");
	mNetworkLayout.addWidget(&mHostnameLabel);
	mNetworkLayout.addWidget(&mIPLabel);
	setLayout(&mNetworkLayout);

	mUpdateTimer.setInterval(5000);
	connect(&mUpdateTimer, SIGNAL(timeout()), this, SLOT(updateIP()));
	connect(&mUpdateTimer, SIGNAL(timeout()), this, SLOT(updateHostname()));
	mUpdateTimer.start();
}

void NetworkWidget::updateIP()
{
	const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
	for (const QNetworkInterface &interface : interfaces) {
		if (interface.name() == "wlan0") {
			const QList<QNetworkAddressEntry> entries = interface.addressEntries();
			for (const QNetworkAddressEntry &entry : entries) {
				const QHostAddress ip = entry.ip();
				if (ip.protocol() == QAbstractSocket::IPv4Protocol) {
					mIPLabel.setText(tr("IP: ") + ip.toString());
					break;
				}
			}

			break;
		}
	}
}

void NetworkWidget::updateHostname()
{
	const QString name = trikKernel::FileUtils::readFromFile(trikKernel::Paths::hostnameName()).trimmed();
	mHostnameLabel.setText(tr("Name: ") + name);
}
