import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Rectangle {
    id: _rectConfirm
    width: parent.width / 1.08
    height: _columnConfirm.height
    z: 1
    color: activeTheme.confirmWindowColor
    radius: 10
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter
    border.color: activeTheme.delimeterLineColor
    border.width: 1
    visible: false
    property string focusButton: "No"
    property string textAction: ""
    property string descrAction: ""
    property var buttonNo: _buttonNo
    property real parentWidth: 0

    Keys.onPressed: {
        switch (event.key) {
        case Qt.Key_Right:
            if (focusButton === "No") {
                focusButton = "Yes";
                _buttonYes.focus = true;
            }
            break;
        case Qt.Key_Left:
            if (focusButton === "Yes") {
                focusButton = "No";
                _buttonNo.focus = true;
            }
            break;
        case Qt.Key_Return:
            if (focusButton === "Yes") {
                _buttonYes.clicked();
            } else if (focusButton === "No") {
                _buttonNo.clicked();
            }
            break;
        case Qt.Key_Escape:
            _confirm.visible = false;
            break;
        case Qt.Key_PowerOff:
            if (!event.isAutoRepeat) {
                _confirm.visible = false;
            }
            break;
        case Qt.Key_W:
            if (!event.isAutoRepeat) {
                _confirm.visible = false;
            }
            break;
        default:
            break;
        }
    }
    ColumnLayout {
        id: _columnConfirm
        width: parent.width
        Text {
            text: textAction
            wrapMode: Text.Wrap
            Layout.fillWidth: true
            font.pointSize: fontSizes.body
            horizontalAlignment: Text.AlignHCenter
            Layout.topMargin: 6
            color: activeTheme.textColor
        }
        Rectangle {
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 1
            color: activeTheme.delimeterLineColor
        }
        RowLayout {
            Image {
                id: _icon
                source: iconsPath + "warningDel.png"
                Layout.preferredWidth: parentWidth < 400 ? parentWidth / 7 : parentWidth / 25
                Layout.preferredHeight: parentWidth < 400 ? parentWidth / 7 : parentWidth / 25
                Layout.leftMargin: 5
                Layout.rightMargin: 5
            }

            Text {
                text: descrAction
                wrapMode: Text.Wrap
                Layout.fillWidth: true
                Layout.maximumWidth: _rectConfirm.width - _icon.width
                font.pointSize: fontSizes.body
                Layout.rightMargin: 5
                color: activeTheme.textColor
            }
        }
        RowLayout {
            Layout.alignment: Qt.AlignRight
            Layout.bottomMargin: 6
            Layout.rightMargin: 9
            Layout.topMargin: 8
            Button {
                id: _buttonNo
                Layout.preferredHeight: _columnConfirm.width / 6.8
                Layout.preferredWidth: _columnConfirm.width / 3.8
                onClicked: {
                    noOnClick();
                }
                contentItem: Text {
                    text: qsTr("No")
                    font.pointSize: fontSizes.medium
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
            Button {
                id: _buttonYes
                Layout.preferredHeight: _columnConfirm.width / 6.8
                Layout.preferredWidth: _columnConfirm.width / 3.8
                onClicked: {
                    yesOnClick();
                }
                contentItem: Text {
                    text: qsTr("Yes")
                    font.pointSize: fontSizes.medium
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}
