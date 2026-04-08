import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls.Material 2.15
import com.trikGui 1.0

ApplicationWindow {
    id: root
    visible: true
    minimumHeight: 320
    minimumWidth: 240
    maximumHeight: 320
    maximumWidth: 240
    title: qsTr("TRIK")

    property string iconsPath: "../resourcesQml/"
    property var activeTheme: ThemeMode === "light" ? _lightTheme : _darkTheme
    property var fontSizes: _fontSizes

    LightTheme {
        id: _lightTheme
    }
    DarkTheme {
        id: _darkTheme
    }
    FontSizes {
        id: _fontSizes
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: -1

        StatusBar {
            id: _statusBar
            z: 10
            Layout.fillWidth: true
            Layout.preferredHeight: 0.08 * parent.height
            Layout.alignment: Qt.AlignTop
        }

        StackView {
            id: stack
            initialItem: _mainMenu
            Layout.fillWidth: true
            Layout.fillHeight: true
            focus: true

            Keys.onPressed: {
                switch (event.key) {
                case Qt.Key_Escape:
                    if (stack.depth > 1) {
                        stack.pop();
                        if (stack.currentItem && stack.currentItem.idList) {
                            stack.currentItem.idList.focus = true;
                        }
                    }
                    break;
                case Qt.Key_PowerOff:
                    if (!event.isAutoRepeat) {
                        stack.pop(null);
                        stack.currentItem.idList.currentIndex = 0;
                        stack.currentItem.idList.focus = true;
                    }
                    break;
                case Qt.Key_W:
                    if ((event.modifiers & Qt.ControlModifier) && !event.isAutoRepeat) {
                        stack.pop(null);
                        stack.currentItem.idList.currentIndex = 0;
                        stack.currentItem.idList.focus = true;
                    }
                    break;
                }
            }

            Connections {
                target: MainMenuManager
                function onShowPowerConfirm() {
                    if (stack.currentItem === _mainMenu) {
                        _mainMenu.confirmWindow.visible = true;
                        _mainMenu.confirmWindow.focus = true;
                    }
                }
                function onHidePowerConfirm() {
                    if (stack.currentItem === _mainMenu) {
                        _mainMenu.confirmWindow.visible = false;
                        _mainMenu.idList.focus = true;
                    }
                }
            }
            pushEnter: Transition {
                PropertyAnimation {
                    property: "opacity"
                    from: 0
                    to: 1
                    duration: 0
                }
            }
            pushExit: Transition {
                PropertyAnimation {
                    property: "opacity"
                    from: 1
                    to: 0
                    duration: 0
                }
            }
            popEnter: Transition {
                PropertyAnimation {
                    property: "opacity"
                    from: 0
                    to: 1
                    duration: 0
                }
            }
            popExit: Transition {
                PropertyAnimation {
                    property: "opacity"
                    from: 1
                    to: 0
                    duration: 0
                }
            }
            replaceEnter: Transition {
                PropertyAnimation {
                    property: "opacity"
                    from: 0
                    to: 1
                    duration: 0
                }
            }
            replaceExit: Transition {
                PropertyAnimation {
                    property: "opacity"
                    from: 1
                    to: 0
                    duration: 0
                }
            }
        }
        MainMenu {
            id: _mainMenu
            color: activeTheme.backgroundColor
        }
    }
}
