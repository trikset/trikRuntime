/* Copyright 2014 CyberTech Labs Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#include "wiFiAPWidget.h"

#include <QtCore/QList>

#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QNetworkAddressEntry>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QAbstractSocket>

#include <trikKernel/paths.h>
#include <trikKernel/rcReader.h>
#include <trikKernel/fileUtils.h>
#include "qrcodegen.h"

using namespace trikGui;

WiFiAPWidget::WiFiAPWidget(QWidget *parent)
    : TrikGuiDialog(parent)
{
    mTitle.setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    mParametersLayout.setAlignment(Qt::AlignCenter);
    mNetworkLabel.setAlignment(Qt::AlignCenter);
    mKeyLabel.setAlignment(Qt::AlignCenter);
    mIpLabel.setAlignment(Qt::AlignCenter);
    mQRLabel.setAlignment(Qt::AlignCenter);

    mTitle.setText(tr("Network parameters"));
    mNetworkLabel.setText(tr("Name not found"));
    mKeyLabel.setText(tr("Password not found"));
    mIpLabel.setText(tr("IP address not found"));
    QImage qrImage(150, 150, QImage::Format_RGB888);
    qrImage.fill(QColor(Qt::white).rgb());
    mQRLabel.setPixmap(QPixmap::fromImage(qrImage));

    getParameters();

    mMainLayout.addWidget(&mTitle);
    mMainLayout.addStretch(2);
    mMainLayout.addLayout(&mParametersLayout);
    mMainLayout.addStretch(1);

    mParametersLayout.addWidget(&mNetworkLabel);
    mParametersLayout.addWidget(&mKeyLabel);
    mParametersLayout.addWidget(&mIpLabel);
    mParametersLayout.addWidget(&mQRLabel);
    setLayout(&mMainLayout);
}

void WiFiAPWidget::renewFocus()
{
    setFocus();
}

void WiFiAPWidget::getParameters()
{
    const QString name = trikKernel::FileUtils::readFromFile(trikKernel::Paths::hostnameName()).trimmed();
    if (!name.isEmpty()) {
        mNetworkLabel.setText(tr("Name: ") + name);
    }
    const trikKernel::RcReader rcReader(trikKernel::Paths::trikRcName());

    const QString passphrase = rcReader.value("trik_wifi_ap_passphrase");
    if (!passphrase.isEmpty()) {
        mKeyLabel.setText(tr("Password: ") + passphrase);
    }

    const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &interface : interfaces) {
        if (interface.name() == "wlan0") {
            const QList<QNetworkAddressEntry> entries = interface.addressEntries();
            for (const QNetworkAddressEntry &entry : entries) {
                const QHostAddress ip = entry.ip();
                if (ip.protocol() == QAbstractSocket::IPv4Protocol) {
                    mIpLabel.setText(tr("IP address: ") + ip.toString());
                    break;
                }
            }

            break;
        }
    }

    std::string wiFiDataString = "WIFI:S:" + name.toStdString() +";T:WPA;P:" + passphrase.toStdString() + ";;";
    const char *wiFiData = wiFiDataString.c_str();
    enum qrcodegen_Ecc errCorLvl = qrcodegen_Ecc_MEDIUM;
    uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
    uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];
    bool ok = qrcodegen_encodeText(wiFiData, tempBuffer, qrcode, errCorLvl,
                                   qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX, qrcodegen_Mask_AUTO, true);
    if (ok){
        int pixelSize = qrcodegen_getSize(qrcode);
        int scale = 150 / pixelSize;
        int sclaledPixelSize = pixelSize*scale;
        QImage qrImage(sclaledPixelSize, sclaledPixelSize, QImage::Format_RGB888);
        qrImage.fill(QColor(Qt::white).rgb());

        for (int x = 0; x < sclaledPixelSize; ++x) {
            for (int y = 0; y < sclaledPixelSize; ++y) {
                if (qrcodegen_getModule(qrcode, x/scale, y/scale))
                {
                    qrImage.setPixel(x, y, qRgb(0, 0, 0));
                }
            }
        }
        mQRLabel.setPixmap(QPixmap::fromImage(qrImage));
    }
}
