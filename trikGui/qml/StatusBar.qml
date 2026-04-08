import QtQuick 2.15
import QtQuick.Layouts 1.15
import com.trikGui 1.0

Rectangle {
    color: activeTheme.statusBarColor
    property var network: Network

    Rectangle {
        anchors.bottom: parent.bottom
        width: parent.width
        height: 1
        color: activeTheme.delimeterLineColor
    }

    Connections {
        target: WiFiIndicator
        function onModeChanged() {
            switch (WiFiIndicator.mode) {
            case Mode.Client:
                if (WiFiIndicator.isConnected()) {
                    _wiFiIndicator.source = iconsPath + "wiFiIndicator.png";
                    _wiFiIndicator.visible = true;
                } else {
                    _wiFiIndicator.source = "";
                    _wiFiIndicator.visible = false;
                }
                break;
            case Mode.AccessPoint:
                _wiFiIndicator.source = iconsPath + "accessPoint.png";
                _wiFiIndicator.visible = true;
                break;
            case Mode.Unknown:
                _wiFiIndicator.source = "";
                _wiFiIndicator.visible = false;
                break;
            }
        }
    }

    RowLayout {
        id: _rowLayout
        anchors.fill: parent
        spacing: 7
        anchors.rightMargin: 5
        anchors.leftMargin: 5
        Text {
            id: _voltageValue
            text: BatteryIndicator ? BatteryIndicator.voltageValue : ""
            Layout.fillWidth: true
            font.pointSize: fontSizes.small
            color: activeTheme.textColor
        }

        Image {
            id: _communicatorIndicator
            source: CommunicatorIndicator.isConnected ? iconsPath + "communicatorConnected.png" : ""
            // source: iconsPath + "communicatorConnected.png"
            Layout.preferredHeight: parent.height * 0.75
            Layout.preferredWidth: 20
            visible: CommunicatorIndicator.isConnected
        }
        Image {
            id: _gamepadIndicator
            source: GamepadIndicator.isConnected ? iconsPath + "gamepad.png" : ""
            // source: iconsPath + "gamepad.png"
            Layout.preferredHeight: parent.height * 0.75
            Layout.preferredWidth: 20
            visible: GamepadIndicator.isConnected
        }
        Image {
            id: _mailboxIndicator
            source: MailboxIndicator.isConnected ? iconsPath + "mailbox.png" : ""
            // source: iconsPath + "mailbox.png"
            Layout.preferredHeight: parent.height * 0.75
            Layout.preferredWidth: 20
            visible: MailboxIndicator.isConnected
        }
        Image {
            id: _wiFiIndicator
            source: ""
            // source: iconsPath + "wiFiIndicator.png"
            Layout.preferredHeight: parent.height * 0.75
            Layout.preferredWidth: 20
            visible: false
        }
        Text {
            id: _hullNumber
            text: network !== null ? network.hullNumber : ""
            font.pointSize: fontSizes.small
            color: activeTheme.textColor
            visible: network !== null ? (network.hullNumber !== "" ? true : false) : false
        }
    }
    // Rectangle {
    //     anchors.bottom: parent.bottom
    //     anchors.right: parent.right
    //     anchors.left: parent.left
    //     height: 2
    //     color: activeTheme.delimeterLineColor
    //     anchors.bottomMargin: -1
    // }
}
