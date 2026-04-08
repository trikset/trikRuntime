import QtQuick 2.15
import WiFiClient 1.0
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import com.trikGui 1.0

Rectangle {
    id: _mainWiFiClient
    property var wiFiClient: WiFiClientServer
    property string ipValue: ""
    property string connectionStateIconPath: ""
    property bool isScanning: wiFiClient.rowCount() === 0 ? true : false
    color: activeTheme.backgroundColor
    property var idList: _button
    Component.onCompleted: {
        wiFiClient.setQmlParent(_mainWiFiClient);
    }
    ConfirmAction {
        id: _confirm
        textAction: qsTr("Confirm connection")
        descrAction: qsTr("Are you sure you want to connect to open Wi-Fi network?")
        parentWidth: _mainWiFiClient.width
        function noOnClick() {
            _confirm.visible = false;
            _listNetworks.focus = true;
        }
        function yesOnClick() {
            _confirm.visible = false;
            _confirm.focusButton = "No";
            _listNetworks.focus = true;
            wiFiClient.connectToSelectedNetwork(_listNetworks.currentItem.wiFiSsid);
        }
    }

    Connections {
        target: wiFiClient
        function onModelReset() {
            _mainWiFiClient.isScanning = false;
        }
        function onConnectionStateChanged() {
            switch (wiFiClient.connectionState) {
            case ConnectionState.Connected:
                connectionStateIconPath = iconsPath + "ok.png";
                ipValue = wiFiClient.ipValue;
                break;
            case ConnectionState.Connecting:
                connectionStateIconPath = iconsPath + "warning.png";
                ipValue = qsTr("connecting...");
                break;
            case ConnectionState.NotConnected:
                connectionStateIconPath = iconsPath + "error.png";
                ipValue = qsTr("no connection");
                break;
            case ConnectionState.Errored:
                connectionStateIconPath = iconsPath + "error.png";
                ipValue = qsTr("error");
                break;
            }
        }
    }

    ColumnLayout {
        spacing: 5
        anchors.fill: parent
        anchors.margins: 10
        Button {
            id: _button
            Layout.preferredHeight: parent.height / 10
            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true

            contentItem: Text {
                text: qsTr("Enable access point")
                font.pointSize: fontSizes.medium
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            Keys.onPressed: {
                switch (event.key) {
                case Qt.Key_Down:
                    if (_listNetworks.count > 0) {
                        _listNetworks.focus = true;
                        _listNetworks.currentIndex = 0;
                        _listNetworks.positionViewAtIndex(0, ListView.Beginning);
                    }
                    break;
                case Qt.Key_Up:
                    if (_listNetworks.count > 0) {
                        _listNetworks.focus = true;
                        _listNetworks.currentIndex = _listNetworks.count - 1;
                        _listNetworks.positionViewAtIndex(_listNetworks.count - 1, ListView.End);
                    }
                    break;
                case Qt.Key_Return:
                    WiFiModeServer.setMode(Mode.AccessPoint);
                    break;
                default:
                    break;
                }
            }
        }
        Row {
            width: parent.width
            height: parent.height
            Layout.alignment: Qt.AlignLeft
            spacing: connectionStateIconPath !== "" ? (_mainWiFiClient.width < 400 ? 5 : 15) : 0
            Image {
                id: _connectionStatus
                source: connectionStateIconPath
                width: connectionStateIconPath !== "" ? _mainWiFiClient.width < 400 ? _mainWiFiClient.width / 10 : _mainWiFiClient.width / 25 : 0
                height: connectionStateIconPath !== "" ? _mainWiFiClient.width < 400 ? _mainWiFiClient.width / 10 : _mainWiFiClient.width / 25 : 0
                anchors.verticalCenter: parent.verticalCenter
            }

            Text {
                id: _ipValue
                text: qsTr("IP: ") + ipValue
                anchors.verticalCenter: parent.verticalCenter
                font.pointSize: fontSizes.medium
                color: activeTheme.textColor
            }
        }
        Text {
            id: _availableNet
            text: qsTr("Available networks:")
            font.pointSize: fontSizes.medium
            color: activeTheme.textColor
        }

        Rectangle {
            id: _mainView
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 10
            clip: true
            color: activeTheme.managersBackColor
            ColumnLayout {
                anchors.right: parent.right
                anchors.left: parent.left
                anchors.margins: 20
                spacing: 15
                visible: isScanning
                anchors.verticalCenter: parent.verticalCenter
                Text {
                    text: qsTr("Scanning...")
                    wrapMode: Text.Wrap
                    width: parent.width / 1.2
                    Layout.alignment: Qt.AlignHCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pointSize: fontSizes.medium
                    color: activeTheme.textColor
                }
                ProgressBar {
                    indeterminate: true
                    palette.dark: activeTheme.trikColor
                    Layout.fillWidth: true
                    background: Rectangle {
                        color: activeTheme.backgroundColor
                        radius: 3
                    }
                }
            }
            ListView {
                id: _listNetworks
                anchors.fill: parent
                anchors.topMargin: 4
                anchors.bottomMargin: 4
                model: wiFiClient
                visible: !isScanning
                spacing: 7
                property string currentSsid: wiFiClient.currentSsid
                property real lineMargin: 8
                function getWiFiStateIconPath(displaySsid, displayIsKnown, displaySecurity) {
                    if (displaySsid === currentSsid) {
                        return iconsPath + "connectedWiFi.png";
                    } else if (displayIsKnown) {
                        return iconsPath + "knownWiFi.png";
                    } else if (displaySecurity === "none") {
                        return iconsPath + "openWiFiBigLock.png";
                    } else {
                        return iconsPath + "passwordedWiFi.png";
                    }
                }
                Component.onCompleted: {
                    currentIndex = 0;
                }
                ScrollBar.vertical: ScrollBar {
                    id: _scroll
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    contentItem: Rectangle {
                        implicitWidth: 5
                        radius: 10
                        color: "#B6B5B5"
                    }
                    policy: _listNetworks.contentHeight > _listNetworks.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
                }
                delegate: Item {
                    id: _delegateNetworks
                    width: _listNetworks.width
                    implicitHeight: (_textSsid.implicitHeight > _wiFiStatus.height ? _textSsid.implicitHeight : _wiFiStatus.height) + _listNetworks.lineMargin
                    property string wiFiSsid: display.ssid
                    property var isCurrent: ListView.isCurrentItem
                    property string connectionWiFiStateIconPath: _listNetworks.getWiFiStateIconPath(display.ssid, display.isKnown, display.security)
                    Keys.onPressed: {
                        switch (event.key) {
                        case Qt.Key_Return:
                            if (display.ssid !== _listNetworks.currentSsid) {
                                if (display.isKnown) {
                                    wiFiClient.connectToSelectedNetwork(display.ssid);
                                } else if (display.security === "none") {
                                    _listNetworks.focus = false;
                                    _confirm.focus = true;
                                    _confirm.visible = true;
                                    _confirm.buttonNo.focus = true;
                                }
                            }
                            break;
                        case Qt.Key_Up:
                            if (index === 0) {
                                _button.focus = true;
                            }
                            break;
                        case Qt.Key_Down:
                            if (_listNetworks.count - 1 === index) {
                                _button.focus = true;
                            }
                            break;
                        default:
                            break;
                        }
                    }
                    Rectangle {
                        id: _wiFiSsid
                        anchors.fill: parent
                        radius: 10
                        color: _delegateNetworks.isCurrent && _listNetworks.focus ? activeTheme.focusElementsOfListNetworksColor : activeTheme.managersBackColor
                        anchors.leftMargin: 9
                        anchors.rightMargin: 9
                        RowLayout {
                            width: parent.width
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            anchors.leftMargin: 3
                            spacing: connectionWiFiStateIconPath ? (_mainWiFiClient.width < 400 ? 5 : 15) : 0
                            Image {
                                id: _wiFiStatus
                                source: connectionWiFiStateIconPath ? connectionWiFiStateIconPath : ""
                                Layout.preferredWidth: connectionWiFiStateIconPath ? _mainWiFiClient.width < 400 ? _mainWiFiClient.width / 9 : _mainWiFiClient.width / 25 : 0
                                Layout.preferredHeight: connectionWiFiStateIconPath ? _mainWiFiClient.width < 400 ? _mainWiFiClient.width / 9 : _mainWiFiClient.width / 25 : 0
                                Layout.alignment: Qt.AlignVCenter
                            }
                            Text {
                                id: _textSsid
                                text: display.ssid
                                font.bold: display.ssid === _listNetworks.currentSsid
                                font.pointSize: fontSizes.medium
                                Layout.fillWidth: true
                                Layout.rightMargin: 7
                                Layout.alignment: Qt.AlignVCenter
                                wrapMode: Text.Wrap
                                color: activeTheme.textColor
                            }
                        }
                    }
                }
            }
        }
    }
}
