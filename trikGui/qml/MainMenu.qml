import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import com.trikGui 1.0

Rectangle {
    id: _mainMenuView
    property var network: Network
    property var idList: _gridView
    property var confirmWindow: _confirm
    // Shared state: accessible from both Connections blocks
    property var graphicsWidgetObject: null

    Connections {
        target: RunningCode
        property var runningCodeObject: null

        // Pop GraphicsWidgetComponent first if it sits above RunningCodeComponent.
        // This is needed because scriptExecutionCompleted() emits hideRunningWidget
        // before calling brick.reset() (which triggers GraphicsWidget.hide), so
        // onHideRunningCodeComponent always fires while GraphicsWidget is still on top.
        function _popGraphicsIfNeeded() {
            if (_mainMenuView.graphicsWidgetObject !== null && _mainMenuView.graphicsWidgetObject === stack.currentItem) {
                stack.pop();
                _mainMenuView.graphicsWidgetObject = null;
            }
        }

        function onShowRunningCodeComponent(programName) {
            var page = stack.push("RunningCodeComponent.qml");
            if (page) {
                if (programName === "direct command") {
                    page.idText.text = qsTr("Running direct command");
                } else {
                    page.idText.text = qsTr("Running ") + programName;
                }
                runningCodeObject = page;
                page.focus = true;
            } else if (RunningCodeComponent.status === Component.Error) {
                console.error("[MainMenu] component load error:", RunningCodeComponent.errorString());
            }
        }
        function onHideRunningCodeComponent() {
            _popGraphicsIfNeeded();
            if (runningCodeObject !== null && runningCodeObject === stack.currentItem) {
                stack.pop();
                runningCodeObject = null;
                if (stack.currentItem && stack.currentItem.idList) {
                    stack.currentItem.idList.focus = true;
                }
            }
        }
        function onHideScriptComponent() {
            _popGraphicsIfNeeded();
            if (runningCodeObject !== null && runningCodeObject === stack.currentItem) {
                stack.pop();
                runningCodeObject = null;
                if (stack.currentItem && stack.currentItem.idList) {
                    stack.currentItem.idList.focus = true;
                }
            }
        }
        function onShowErrorRunningCodeComponent(error) {
            if (runningCodeObject !== null && runningCodeObject === stack.currentItem) {
                stack.currentItem.idText.text = error;
                stack.currentItem.idText.color = "red";
            }
        }
    }

    Connections {
        target: GraphicsWidget

        function onShown() {
            if (_mainMenuView.graphicsWidgetObject !== null) {
                return;
            }
            var page = stack.push("GraphicsWidgetComponent.qml");
            if (page) {
                _mainMenuView.graphicsWidgetObject = page;
                page.focus = true;
            }
        }

        function onHidden() {
            if (_mainMenuView.graphicsWidgetObject !== null && _mainMenuView.graphicsWidgetObject === stack.currentItem) {
                // Defer pop out of GraphicsWidget's own signal emission to avoid
                // "Object destroyed while signal handler is in progress" crash.
                var saved = _mainMenuView.graphicsWidgetObject;
                _mainMenuView.graphicsWidgetObject = null;
                Qt.callLater(function () {
                    if (saved === stack.currentItem) {
                        stack.pop();
                        if (stack.currentItem && stack.currentItem.idList) {
                            stack.currentItem.idList.focus = true;
                        }
                    }
                });
            } else {
                _mainMenuView.graphicsWidgetObject = null;
            }
        }
    }

    Rectangle {
        id: _confirm
        width: parent.width / 1.05
        height: parent.height / 3.8
        z: 1
        color: activeTheme.confirmWindowColor
        radius: 10
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        border.color: activeTheme.delimeterLineColor
        border.width: 1
        visible: false

        RowLayout {
            id: _rowConfrim
            anchors.fill: parent
            anchors.margins: 10
            Image {
                source: iconsPath + "warningDel.png"
                Layout.preferredWidth: _mainMenuView.width < 400 ? _mainMenuView.width / 7 : _mainMenuView.width / 25
                Layout.preferredHeight: _mainMenuView.width < 400 ? _mainMenuView.width / 7 : _mainMenuView.width / 25
                Layout.leftMargin: 5
                Layout.rightMargin: 5
            }

            Text {
                text: qsTr("Keep holding to turn off the controller.")
                wrapMode: Text.Wrap
                Layout.fillWidth: true
                font.pointSize: fontSizes.body
                Layout.rightMargin: 5
                color: activeTheme.textColor
            }
        }
    }

    ListModel {
        id: _menuItems
        ListElement {
            text: qsTr("Files")
            iconPath: "files.png"
            filePath: "FileManager.qml"
            appType: AppType.Files
        }
        ListElement {
            text: qsTr("Testing")
            iconPath: "testing.png"
            filePath: "Testing.qml"
            appType: AppType.Testing
        }
        ListElement {
            iconPath: "network.png"
            filePath: "WiFiMode.qml"
            text: qsTr("Network")
            appType: AppType.Network
        }
        ListElement {
            text: qsTr("Settings")
            iconPath: "settings.png"
            filePath: "Settings.qml"
            appType: AppType.Settings
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        anchors.topMargin: 7
        Text {
            text: qsTr("Name: ") + (network !== null ? network.hostName : "")
            Layout.fillWidth: true
            font.pointSize: fontSizes.body
            wrapMode: Text.Wrap
            color: activeTheme.textColor
        }
        Text {
            text: qsTr("IP: ") + (network !== null ? (network.ip !== "" ? network.ip : qsTr("no connection")) : qsTr("no connection"))
            Layout.fillWidth: true
            font.pointSize: fontSizes.body
            wrapMode: Text.Wrap
            color: activeTheme.textColor
        }
        Text {
            text: qsTr("Hull number: ") + (network !== null ? network.hullNumber : "")
            Layout.fillWidth: true
            font.pointSize: fontSizes.body
            wrapMode: Text.Wrap
            color: activeTheme.textColor
        }
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            GridView {
                id: _gridView
                anchors.fill: parent
                anchors.topMargin: 15
                cellWidth: parent.width / 2
                model: _menuItems
                property real maxTextLen: 12
                Component.onCompleted: {
                    focus = true;
                }
                Keys.onPressed: {
                    if (event.key === Qt.Key_Right && _gridView.currentIndex === _menuItems.count - 1) {
                        _gridView.currentIndex = 0;
                        event.accepted = true;
                    }
                    if (event.key === Qt.Key_Left && _gridView.currentIndex === 0) {
                        _gridView.currentIndex = _menuItems.count - 1;
                        event.accepted = true;
                    }
                }

                delegate: ColumnLayout {
                    id: _delegate
                    property bool isCurrent: GridView.isCurrentItem
                    spacing: 0

                    width: _gridView.cellWidth
                    Keys.onPressed: {
                        switch (event.key) {
                        case Qt.Key_Return:
                            MainMenuManager.createApp(model.appType);
                            var page = stack.push(model.filePath);
                            if (page && page.idList) {
                                page.idList.focus = true;
                            }
                            // var component = Qt.createComponent(model.filePath);
                            // if (component.status === Component.Ready) {
                            //     var object = component.createObject(_mainMenuView);
                            //     if (object === null) {
                            //         console.error("[MainMenu] createObject returned null for:", model.filePath, component.errorString());
                            //         break;
                            //     }
                            //     stack.push(object);
                            //     if (object.idList) {
                            //         object.idList.focus = true;
                            //     }
                            // } else if (component.status === Component.Error) {
                            //     console.error("[MainMenu] component load error for:", model.filePath, component.errorString());
                            // }
                            break;
                        default:
                            break;
                        }
                    }

                    Item {
                        property real iconMargin: 12
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredWidth: _iconMenu.width + iconMargin
                        Layout.preferredHeight: _iconMenu.height + iconMargin
                        Rectangle {
                            id: _iconWrapper
                            anchors.fill: parent
                            radius: 10
                            border.color: _delegate.isCurrent ? activeTheme.lightOrStandartGreenColor : "transparent"
                            border.width: _delegate.isCurrent ? 4 : 0
                            color: activeTheme.elementsOfGridColor
                            Image {
                                id: _iconMenu
                                source: iconsPath + model.iconPath
                                anchors.centerIn: parent
                                width: _mainMenuView.width < 400 ? _mainMenuView.width / 5 : _mainMenuView.width / 23
                                height: _mainMenuView.width < 400 ? _mainMenuView.width / 5 : _mainMenuView.width / 23
                            }
                        }
                    }
                    Text {
                        id: _text
                        text: model.text.length > _gridView.maxTextLen ? model.text.substring(0, _gridView.maxTextLen) + "..." : model.text
                        Layout.alignment: Qt.AlignHCenter
                        wrapMode: Text.Wrap
                        font.pointSize: fontSizes.medium
                        Layout.maximumWidth: parent.width
                        color: activeTheme.textColor
                    }
                }
            }
        }
    }
}
