import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: _sensorsPanel
    property var sensors: Sensors
    property var sensorsCount: sensors.rowCount()
    property var idList: _listSensorsIndicators
    color: activeTheme.backgroundColor

    ListView {
        id: _listSensorsIndicators
        anchors.fill: parent
        model: sensors
        spacing: 4
        anchors.topMargin: 5
        anchors.bottomMargin: 5
        delegate: Item {
            id: _delegateIndicator
            width: _listSensorsIndicators.width
            height: (_listSensorsIndicators.height / sensorsCount) - _listSensorsIndicators.spacing

            Rectangle {
                id: _sensorIndicator
                anchors.fill: parent
                anchors.leftMargin: 6
                anchors.rightMargin: 6
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
                Item {
                    width: parent.height
                    height: parent.height
                    anchors.horizontalCenter: parent.horizontalCenter
                    Rectangle {
                        id: _dial
                        anchors.fill: parent
                        anchors.margins: 8
                        color: "lightgrey"
                        border.color: "#353535"
                        radius: width / 2

                        Rectangle {
                            id: _arrow
                            x: _dial.width / 2
                            y: _dial.height / 2
                            height: _dial.height / 2
                            transformOrigin: Item.Top
                            rotation: display.value
                            Rectangle {
                                width: _dial.width / 6
                                height: _dial.width / 6
                                color: activeTheme.lightOrStandartGreenColor
                                radius: _dial.width / 2
                                anchors.bottom: parent.bottom
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.margins: 4
                            }
                        }
                    }
                }
            }
        }
    }
}
