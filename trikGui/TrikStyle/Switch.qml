import QtQuick 2.0
import QtQuick.Templates 2.0 as T

T.Switch {
    indicator: Rectangle {
        width: parent.width
        height: parent.height
        radius: 13
        color: display.isOn ? activeTheme.trikColor : activeTheme.switchBorderColor3
        border.color: display.isOn ? activeTheme.trikColor : activeTheme.switchBorderColor1
        Rectangle {
            x: display.isOn ? parent.width - width : 0
            width: parent.width / 2
            height: parent.height
            radius: 13
            color: control.down ? "#CCCCCC" : activeTheme.switchBorderColor3
            border.color: display.isOn ? activeTheme.trikColor : activeTheme.switchBorderColor2
        }
    }
}
