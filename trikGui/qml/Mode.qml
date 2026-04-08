import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Rectangle {
    id: _modeSelection
    property var modeSelection: ModeManager
    property var idList: _listModes
    color: activeTheme.backgroundColor
    Component.onCompleted: {
        modeSelection.setQmlParent(_modeSelection)
    }

    ColumnLayout {
        id: _columMainView
        spacing: 5
        anchors.fill: parent
        anchors.margins: 15

        Text {
            text: qsTr("Select mode:")
            wrapMode: Text.Wrap
            font.pointSize: fontSizes.medium
            color: activeTheme.textColor
        }

        Rectangle {
            id: _mainView
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 10
            clip: true
            color: activeTheme.managersBackColor
            ListModel {
                id: _dataMode
                ListElement {
                    iconPath: "sun.png"
                    mode: "light"
                    text: qsTr("Light")
                }
                ListElement {
                    iconPath: "moon.png"
                    mode: "dark"
                    text: qsTr("Dark")
                }
            }

            ListView {
                id: _listModes
                anchors.fill: parent
                spacing: 10
                property real lineMargin: 8
                model: _dataMode
                function getCurrentMode() {
                    var currentMode = modeSelection.getCurrentMode()
                    if (currentMode === "light") {
                        return 0
                    } else {
                        return 1
                    }
                }

                currentIndex: getCurrentMode()

                delegate: Item {
                    id: _delegateModes
                    width: _listModes.width
                    implicitHeight: (_textName.implicitHeight > _modeIcon.height ? _textName.implicitHeight : _modeIcon.height) + _listModes.lineMargin
                    property var isCurrent: ListView.isCurrentItem
                    Keys.onPressed: {
                        switch (event.key) {
                        case Qt.Key_Up:
                            if (_listModes.currentIndex > 0) {
                                modeSelection.switchMode("light")
                            }
                            break
                        case Qt.Key_Down:
                            if (_listModes.currentIndex < _listModes.count - 1) {
                                modeSelection.switchMode("dark")
                            }
                            break
                        default:
                            break
                        }
                    }
                    Rectangle {
                        id: _modeName
                        anchors.fill: parent
                        radius: 10
                        color: _delegateModes.isCurrent ? activeTheme.darkTrikColor : activeTheme.managersBackColor
                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 5
                            spacing: 5
                            Image {
                                id: _modeIcon
                                source: iconsPath + model.iconPath
                                Layout.preferredWidth: _modeSelection.width
                                                       < 400 ? _modeSelection.width
                                                               / 7 : _modeSelection.width / 25
                                Layout.preferredHeight: _modeSelection.width
                                                        < 400 ? _modeSelection.width
                                                                / 7 : _modeSelection.width / 25
                                Layout.alignment: Qt.AlignVCenter
                            }
                            Text {
                                id: _textName
                                text: model.text
                                Layout.alignment: Qt.AlignVCenter
                                color: _delegateModes.isCurrent ? "white" : activeTheme.namesColor
                                wrapMode: Text.Wrap
                                Layout.fillWidth: true
                                font.pointSize: fontSizes.medium
                            }
                        }
                    }
                }
            }
        }
    }
}
