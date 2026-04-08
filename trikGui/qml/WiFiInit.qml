import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    color: activeTheme.backgroundColor
    property var wiFiInit: WiFiInitServer

    Keys.onPressed: {
        switch (event.key) {
        case Qt.Key_Escape:
            wiFiInit.exit()
            event.accepted = true
            break
        case Qt.Key_PowerOff:
            event.accepted = true
            break
        case Qt.Key_W:
            if (event.modifiers & Qt.ControlModifier) {
                event.accepted = true
            }
            break
        default:
            break
        }
    }
    ColumnLayout {
        id: _columnInfo
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        Text {
            text: qsTr("Network initialization in process")
            wrapMode: Text.Wrap
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: fontSizes.medium
            color: activeTheme.textColor
        }
        Text {
            text: qsTr("Please wait")
            wrapMode: Text.Wrap
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: fontSizes.medium
            color: activeTheme.textColor
        }
        ProgressBar {
            indeterminate: true
            palette.dark: activeTheme.trikColor
            Layout.fillWidth: true
            background: Rectangle {
                color: activeTheme.progressBarWiFiInitColor
                radius: 3
            }
        }

        Text {
            text: qsTr("Press Escape for break")
            wrapMode: Text.Wrap
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: fontSizes.medium
            color: activeTheme.textColor
        }
    }
}
