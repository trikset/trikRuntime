import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls.Material 2.15
import com.trikGui 1.0

ApplicationWindow {
    id: root
    flags: Qt.Window | Qt.BypassWindowManagerHint
    x: 0
    y: 0
    width: 480
    height: 854
    visible: false
    title: qsTr("TRIK")

    Component.onCompleted: {
        root.visible = true
        root.requestActivate()
    }

    property string iconsPath: "../resourcesQml/"
    property var activeTheme: ThemeMode === "light" ? _lightTheme : _darkTheme
    property var fontSizes: _fontSizes
    readonly property real statusBarHeightRatio: 0.07

    function navigateBack() {
        if (stack.depth > 1) {
            stack.pop();
            if (stack.currentItem && stack.currentItem.idList) {
                stack.currentItem.idList.focus = true;
            }
        }
    }

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
            Layout.preferredHeight: root.statusBarHeightRatio * parent.height
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
        VirtualKeyPanel {
            id: _virtualKeyPanel
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            z: 10
        }
    }

    // Status bar and the virtual key panel are non-active areas: previews live
    // strictly between them and never overlap either.
    VideoPreviewList {
        anchors.top: parent.top
        anchors.topMargin: _statusBar.height
        anchors.bottom: parent.bottom
        anchors.bottomMargin: _virtualKeyPanel.height
        anchors.left: parent.left
        anchors.right: parent.right
        z: 100
    }
}
