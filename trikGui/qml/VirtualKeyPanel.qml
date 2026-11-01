import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: _virtualKeys
    implicitWidth: _layout.implicitWidth + 6
    implicitHeight: _layout.implicitHeight + 6
    color: activeTheme.backgroundColor
    opacity: 0.95
    border.color: activeTheme.delimeterLineColor
    border.width: 1

    RowLayout {
        id: _layout
        spacing: 4
        anchors.centerIn: parent

        VKeyButton { text: "▲"; key: Qt.Key_Up }
        VKeyButton { text: "▼"; key: Qt.Key_Down }
        VKeyButton { text: "◄"; key: Qt.Key_Left }
        VKeyButton { text: "►"; key: Qt.Key_Right }
        Rectangle {
            Layout.preferredWidth: 4
            Layout.preferredHeight: 1
            color: "transparent"
        }
        VKeyButton { text: "OK"; key: Qt.Key_Return }
        VKeyButton { text: "←"; key: Qt.Key_Escape }
        VKeyButton { key: Qt.Key_PowerOff; powerIcon: true }
    }

    component VKeyButton: Rectangle {
        id: _btn
        property string text
        property int key: Qt.Key_A
        property bool powerIcon: false
        implicitWidth: 48
        implicitHeight: 48
        radius: 8
        color: _ma.containsPress ? activeTheme.darkTrikColor : activeTheme.elementsOfListColor
        border.color: activeTheme.delimeterLineColor
        border.width: 1

        Text {
            visible: !_btn.powerIcon
            text: _btn.text
            anchors.centerIn: parent
            font.pointSize: fontSizes.medium
            color: activeTheme.textColor
        }

        Canvas {
            id: _powerCanvas
            visible: _btn.powerIcon
            width: 26
            height: 26
            anchors.centerIn: parent
            onPaint: {
                var ctx = getContext("2d");
                ctx.reset();
                ctx.strokeStyle = activeTheme.textColor;
                ctx.lineWidth = 2.5;
                ctx.lineCap = "round";
                var cx = width / 2;
                var cy = height / 2 + 1;
                var r = width * 0.32;
                ctx.beginPath();
                ctx.arc(cx, cy, r, -Math.PI / 2 + 0.6, Math.PI * 1.5 - 0.6, false);
                ctx.stroke();
                ctx.beginPath();
                ctx.moveTo(cx, cy - r - 3);
                ctx.lineTo(cx, cy - 1);
                ctx.stroke();
            }
            onVisibleChanged: if (visible) requestPaint()
        }

        Connections {
            target: activeTheme
            function onTextColorChanged() { _powerCanvas.requestPaint(); }
        }

        MouseArea {
            id: _ma
            anchors.fill: parent
            onClicked: {
                VirtualKeySender.sendKeyPress(_btn.key);
                if (typeof KeysController !== "undefined" && KeysController.emulateKeyPress) {
                    KeysController.emulateKeyPress(_btn.key);
                }
            }
        }
    }
}
