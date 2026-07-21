import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: _motorsPanel
    property var motors: MotorsManager
    property var idList: _listMotorsLevers
    color: activeTheme.backgroundColor

    ListView {
        id: _listMotorsLevers
        anchors.fill: parent
        model: motors
        spacing: 4
        anchors.topMargin: 4
        anchors.bottomMargin: 4
        Keys.onPressed: {
            if (event.key === Qt.Key_Down
                    && _listMotorsLevers.currentIndex === _listMotorsLevers.count - 1) {
                _listMotorsLevers.currentIndex = 0
                _listMotorsLevers.positionViewAtIndex(0, ListView.Beginning)
                event.accepted = true
            } else if (event.key === Qt.Key_Up
                       && _listMotorsLevers.currentIndex === 0) {
                _listMotorsLevers.currentIndex = _listMotorsLevers.count - 1
                _listMotorsLevers.positionViewAtIndex(
                            _listMotorsLevers.count - 1, ListView.End)
                event.accepted = true
            }
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
            policy: _listMotorsLevers.contentHeight
                    > _listMotorsLevers.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
        }
        delegate: Item {
            id: _delegateLever
            width: _listMotorsLevers.width
            height: _listMotorsLevers.height / 4.2
            property var isCurrent: ListView.isCurrentItem
            Keys.onPressed: {
                switch (event.key) {
                case Qt.Key_Enter:
                    display.isOn = !display.isOn
                    break
                case Qt.Key_Return:
                    display.isOn = !display.isOn
                    break
                case Qt.Key_Right:
                    _sb.increase()
                    break
                case Qt.Key_Left:
                    _sb.decrease()
                    break
                default:
                    break
                }
            }
            Rectangle {
                id: _motorLever
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                radius: 10
                border {
                    color: activeTheme.trikColor
                    width: _delegateLever.isCurrent ? 3 : 0
                }
                color: activeTheme.elementsOfListColor
                Text {
                    text: display.nameLabel
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 10
                    anchors.topMargin: 5
                    font.pointSize: fontSizes.medium
                    color: activeTheme.textColor
                }

                Switch {
                    id: control
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.rightMargin: 8
                    anchors.topMargin: 6
                    width: parent.width / 6
                    height: parent.height / 3.7

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            control.down = false
                            display.isOn = !display.isOn
                        }
                        onPressed: {
                            control.down = true
                        }
                    }
                }

                SpinBox {
                    id: _sb
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width / 1.8
                    height: parent.height / 2.3
                    value: display.power
                    from: display.minPower
                    to: display.maxPower
                    stepSize: display.powerStep

                    onValueChanged: {
                        // Check against binding loop
                        if (display.power !== value) {
                            display.power = value
                        }
                    }
                }
            }
        }
    }
}
