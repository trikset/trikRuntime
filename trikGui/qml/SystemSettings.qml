import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import SystemSettings 1.0

Rectangle {
    id: _settingsSelection
    property var systemSettings: SystemSettingsComponent
    property var idList: _listSettings
    color: activeTheme.backgroundColor
    Component.onCompleted: {
        systemSettings.setQmlParent(_settingsSelection)
    }
    ColumnLayout {
        id: _columMainView
        spacing: 5
        anchors.fill: parent
        anchors.topMargin: 7
        anchors.bottomMargin: 15
        anchors.rightMargin: 15
        anchors.leftMargin: 15
        Text {
            text: qsTr("Directory 'Files' is ...")
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
                id: dataModel
                ListElement {
                    text: qsTr("only 'scripts' directory")
                    type: RootType.ScriptsDir
                }
                ListElement {
                    text: qsTr("full the system")
                    type: RootType.AllFS
                }
            }

            ListView {
                id: _listSettings
                anchors.fill: parent
                spacing: 10
                focus: true
                property real lineMargin: 8
                model: dataModel
                function getCurrentFileRoot() {
                    for (var i = 0; i < dataModel.count; i++) {
                        if (dataModel.get(
                                    i).type === systemSettings.fileManagerRootType) {
                            return i
                        }
                    }
                    return 0
                }
                currentIndex: getCurrentFileRoot()
                delegate: Item {
                    id: _delegateSettings
                    implicitWidth: _listSettings.width
                    height: _row.implicitHeight + _listSettings.lineMargin
                    property var isCurrent: ListView.isCurrentItem
                    Keys.onPressed: {
                        switch (event.key) {
                        case Qt.Key_Up:
                            if (_listSettings.currentIndex > 0) {
                                systemSettings.fileManagerRootType = RootType.ScriptsDir
                            }
                            break
                        case Qt.Key_Down:
                            if (_listSettings.currentIndex < _listSettings.count - 1) {
                                systemSettings.fileManagerRootType = RootType.AllFS
                            }
                            break
                        default:
                            break
                        }
                    }

                    Rectangle {
                        id: _settingsName
                        anchors.fill: parent
                        radius: 10
                        color: _delegateSettings.isCurrent ? activeTheme.darkTrikColor : activeTheme.managersBackColor
                        RowLayout {
                            id: _row
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.leftMargin: 3
                            anchors.rightMargin: 3
                            spacing: 5
                            Rectangle {
                                id: _radioButton
                                Layout.preferredHeight: _settingsSelection.width
                                                        < 400 ? _settingsSelection.width
                                                                / 12 : _settingsSelection.width / 25
                                Layout.preferredWidth: _settingsSelection.width
                                                       < 400 ? _settingsSelection.width
                                                               / 12 : _settingsSelection.width / 25
                                Layout.alignment: Qt.AlignVCenter
                                Layout.leftMargin: 5
                                border.color: "black"
                                radius: 13

                                Rectangle {
                                    width: _settingsSelection.width
                                           < 400 ? _settingsSelection.width
                                                   / 19 : _settingsSelection.width / 32
                                    height: _settingsSelection.width
                                            < 400 ? _settingsSelection.width
                                                    / 19 : _settingsSelection.width / 32
                                    radius: 7
                                    color: "black"
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    visible: model.type === systemSettings.fileManagerRootType
                                }
                            }
                            Text {
                                id: _textName
                                text: model.text
                                color: _delegateSettings.isCurrent ? "white" : activeTheme.namesColor
                                Layout.alignment: Qt.AlignVCenter
                                Layout.fillWidth: true
                                wrapMode: Text.Wrap
                                font.pointSize: fontSizes.medium
                            }
                        }
                    }
                }
            }
        }
    }
}
