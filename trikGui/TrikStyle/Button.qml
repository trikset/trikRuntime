import QtQuick 2.0
import QtQuick.Templates 2.0 as T

T.Button {
    id: _button
    background: Rectangle {
        color: parent.focus ? activeTheme.darkTrikColor : activeTheme.buttonsColor
        radius: 10
        Text {
            text: _button.text
            color: "white"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors.centerIn: parent
        }
    }
}
