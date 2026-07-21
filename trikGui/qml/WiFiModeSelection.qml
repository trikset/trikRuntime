import QtQuick 2.15
import com.trikGui 1.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: _mainItem
    property var wiFiMode: WiFiModeServer
    property var idList: _listWiFiModes
    color: activeTheme.backgroundColor

    ListModel {
        id: dataModelModes
        ListElement {
            iconPath: "WiFiClient.png"
            text: qsTr("Wi-Fi client")
            mode: Mode.Client
        }
        ListElement {
            iconPath: "WiFiAP.png"
            text: qsTr("Wi-Fi access point")
            mode: Mode.AccessPoint
        }
    }

    ListView {
        id: _listWiFiModes
        anchors.fill: parent
        anchors.margins: 7
        model: dataModelModes
        spacing: 7
        delegate: Item {
            id: _delegateMode
            width: _listWiFiModes.width
            height: _listWiFiModes.height / 4.8
            property bool isCurrent: ListView.isCurrentItem

            Keys.onPressed: {
                switch (event.key) {
                case Qt.Key_Return:
                    wiFiMode.setMode(model.mode)
                    break
                case Qt.Key_Down:
                    if (_listWiFiModes.currentIndex === dataModelModes.count - 1) {
                        _listWiFiModes.currentIndex = 0
                        _listWiFiModes.positionViewAtIndex(0,
                                                           ListView.Beginning)
                        event.accepted = true
                    }
                    break
                case Qt.Key_Up:
                    if (_listWiFiModes.currentIndex === 0) {
                        _listWiFiModes.currentIndex = dataModelModes.count - 1
                        _listWiFiModes.positionViewAtIndex(
                                    dataModelModes.count - 1, ListView.End)
                        event.accepted = true
                    }
                    break
                default:
                    break
                }
            }
            Rectangle {
                id: _mode
                focus: isCurrent
                anchors.fill: parent
                radius: 10
                color: _delegateMode.isCurrent ? activeTheme.focusElementsOfListColor : activeTheme.elementsOfListColor
                Row {
                    width: parent.width
                    height: parent.height
                    anchors.left: parent.left
                    anchors.leftMargin: 7
                    spacing: parent.width < 400 ? 5 : 15
                    Image {
                        source: iconsPath + model.iconPath
                        width: parent.width < 400 ? parent.width / 7 : parent.width / 23
                        height: parent.width < 400 ? parent.width / 7 : parent.width / 23
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Text {
                        id: _textMode
                        text: model.text
                        anchors.verticalCenter: parent.verticalCenter
                        font.pointSize: fontSizes.medium
                        color: activeTheme.textColor
                    }
                }
            }
        }
    }
}
