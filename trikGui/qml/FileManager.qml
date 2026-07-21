import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Rectangle {
    id: _fileManager
    property var fileManager: FileManagerServer
    property var idList: _listFiles
    color: activeTheme.backgroundColor
    Component.onCompleted: {
        fileManager.setQmlParent(_fileManager);
    }

    ConfirmAction {
        id: _confirm
        textAction: qsTr("Confirm deletion")
        descrAction: _confirm.deleteAll ? qsTr("Are you sure you want to delete all the files?") : qsTr("Are you sure you want to delete file?")
        property bool deleteAll: false
        parentWidth: _fileManager.width
        function noOnClick() {
            if (!_confirm.deleteAll) {
                _listFiles.focus = true;
            } else {
                _buttonDeleteAll.focus = true;
            }
            _confirm.visible = false;
        }
        function yesOnClick() {
            if (!_confirm.deleteAll) {
                fileManager.remove(_listFiles.currentIndex);
                _listFiles.focus = true;
            } else {
                fileManager.removeAll();
                _buttonDeleteAll.focus = true;
            }
            _confirm.visible = false;
            _confirm.focusButton = "No";
        }
    }
    ColumnLayout {
        id: _columMainView
        spacing: 5
        anchors.fill: parent
        anchors.margins: 15

        Text {
            text: fileManager.currentPath
            wrapMode: Text.Wrap
            Layout.fillWidth: true
            font.pointSize: fontSizes.small
            color: activeTheme.textColor
        }

        Rectangle {
            id: _mainView
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 13
            clip: true
            color: activeTheme.managersBackColor

            Connections {
                target: FileManagerServer
                function onCurrentPathChanged() {
                    _listFiles.currentIndex = 0;
                }
            }

            ColumnLayout {
                id: _columnListView
                anchors.fill: parent
                property bool focusListView: true
                ListView {
                    id: _listFiles
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 10
                    model: FileManagerServer
                    ScrollBar.vertical: ScrollBar {
                        id: _scroll
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.right: parent.right
                        contentItem: Rectangle {
                            implicitWidth: 5
                            radius: 10
                            color: "#B6B5B5"
                        }
                        policy: _listFiles.contentHeight > _listFiles.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
                    }
                    property real lineMargin: 8
                    function getFileIcon(isDirValue, filename) {
                        if (isDirValue) {
                            return iconsPath + "folder.png";
                        }
                        var parts = filename.split('.');
                        if (parts.length > 1) {
                            switch (parts[parts.length - 1]) {
                            case "py":
                                return iconsPath + "pyFile.png";
                            case "js":
                                return iconsPath + "jsFile.png";
                            default:
                                return iconsPath + "file.png";
                            }
                        } else {
                            return iconsPath + "file.png";
                        }
                    }
                    delegate: Item {
                        id: _delegateFiles
                        width: _listFiles.width
                        implicitHeight: (_textName.implicitHeight > _fileIcon.height ? _textName.implicitHeight : _fileIcon.height) + _listFiles.lineMargin
                        property var isCurrent: ListView.isCurrentItem
                        Keys.onPressed: {
                            switch (event.key) {
                            case Qt.Key_Return:
                                fileManager.open(index);
                                break;
                            case Qt.Key_Right:
                                _confirm.deleteAll = false;
                                _listFiles.focus = false;
                                _confirm.focus = true;
                                _confirm.visible = true;
                                _confirm.buttonNo.focus = true;
                                break;
                            case Qt.Key_Down:
                                if (_listFiles.count - 1 === index) {
                                    _buttonDeleteAll.focus = true;
                                }
                                break;
                            case Qt.Key_Up:
                                if (0 === index) {
                                    _buttonDeleteAll.focus = true;
                                }
                                break;
                            default:
                                break;
                            }
                        }
                        Rectangle {
                            id: _fileName
                            anchors.fill: parent
                            radius: 10
                            color: _delegateFiles.isCurrent && _listFiles.focus ? activeTheme.darkTrikColor : activeTheme.managersBackColor
                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 5
                                spacing: 5
                                Image {
                                    id: _fileIcon
                                    source: _listFiles.getFileIcon(isDir, fileName)
                                    Layout.preferredWidth: _fileManager.width < 400 ? _fileManager.width / 9 : _fileManager.width / 25
                                    Layout.preferredHeight: _fileManager.width < 400 ? _fileManager.width / 9 : _fileManager.width / 25
                                    Layout.alignment: Qt.AlignVCenter
                                }
                                Text {
                                    id: _textName
                                    text: fileName
                                    color: _delegateFiles.isCurrent && _listFiles.focus ? "white" : activeTheme.namesColor
                                    Layout.fillWidth: true
                                    Layout.rightMargin: 7
                                    Layout.alignment: Qt.AlignVCenter
                                    wrapMode: Text.Wrap
                                    font.pointSize: fontSizes.medium
                                }
                            }
                        }
                    }
                }
                Button {
                    id: _buttonDeleteAll
                    Layout.alignment: Qt.AlignBottom
                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height / 9

                    contentItem: Text {
                        text: qsTr("Delete all ...")
                        font.pointSize: fontSizes.medium
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onClicked: {
                        _confirm.deleteAll = true;
                        _buttonDeleteAll.focus = false;
                        _confirm.focus = true;
                        _confirm.visible = true;
                        _confirm.buttonNo.focus = true;
                    }
                    Keys.onPressed: {
                        switch (event.key) {
                        case Qt.Key_Return:
                            _buttonDeleteAll.clicked();
                            break;
                        case Qt.Key_Up:
                            _listFiles.currentIndex = _listFiles.count - 1;
                            _listFiles.focus = true;
                            _listFiles.positionViewAtIndex(_listFiles.count - 1, ListView.End);
                            break;
                        case Qt.Key_Down:
                            _listFiles.currentIndex = 0;
                            _listFiles.focus = true;
                            _listFiles.positionViewAtIndex(0, ListView.Beginning);
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
        }
    }
}
