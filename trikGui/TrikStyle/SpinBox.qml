import QtQuick 2.0
import QtQuick.Templates 2.0 as T

T.SpinBox {
    background: Rectangle {
        id: _counter
        border.color: activeTheme.spinBorderColor
        anchors.fill: parent
        radius: 10
    }

    contentItem: Text {
        text: _sb.value
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    up.indicator: Rectangle {
        id: _up
        height: parent.height
        width: parent.width / 3.1
        anchors.right: parent.right
        anchors.top: parent.top
        color: _sb.up.pressed ? "#e4e4e4" : "#f6f6f6"
        border.color: activeTheme.spinBorderColor
        radius: 10
        Text {
            text: '+'
            font.pointSize: 17
            anchors.centerIn: parent
            color: enabled ? "black" : "#9E9E9E"
        }
    }
    down.indicator: Rectangle {
        id: _down
        height: parent.height
        width: parent.width / 3.2
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        color: _sb.down.pressed ? "#e4e4e4" : "#f6f6f6"
        border.color: activeTheme.spinBorderColor
        radius: 10
        Text {
            text: '-'
            font.pointSize: 17
            anchors.centerIn: parent
            color: enabled ? "black" : "#9E9E9E"
        }
    }
}
