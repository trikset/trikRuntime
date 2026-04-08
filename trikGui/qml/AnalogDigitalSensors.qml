import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: _sensorsPanel
    property var sensors: Sensors
    property var sensorsCount: sensors.rowCount()
    property var idList: _listSensorsIndicators

    color: activeTheme.backgroundColor

    GridView {
        id: _listSensorsIndicators
        anchors.fill: parent
        cellWidth: parent.width / 2
        cellHeight: parent.height / 5
        model: sensors

        delegate: Item {
            id: _delegateIndicator
            width: _listSensorsIndicators.cellWidth
            height: _listSensorsIndicators.cellHeight
            Rectangle {
                id: _sensorIndicator
                anchors.fill: parent
                anchors.margins: 5
                radius: 10
                color: activeTheme.elementsOfListColor
                Text {
                    id: _nameLabel
                    text: display.nameLabel
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 10
                    font.pointSize: fontSizes.medium
                    color: activeTheme.textColor
                }
                Text {
                    id: _value
                    text: display.value
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.rightMargin: 10
                    font.pointSize: fontSizes.medium
                    color: activeTheme.textColor
                }
            }
        }
    }
}
