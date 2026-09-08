import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Rectangle {
    id: _camera
    property var sensors: Sensors
    property var idList: _portsList
    property var cameraObject: null
    property int photoCounter: 0
    property string port: "video1"
    property bool showPhoto: false
    color: activeTheme.backgroundColor

    function takePhoto() {
        if (_camera.cameraObject) {
            _camera.cameraObject.doPhoto(_camera.port)
        }
    }

    ListModel {
        id: _portsModel
        ListElement { portName: "video2" }
        ListElement { portName: "video1" }
        ListElement { portName: "usb-camera" }
    }
    ListView {
        id: _listSensors
        anchors.fill: parent
        model: sensors

        Keys.onPressed: {
            if (event.key === Qt.Key_Return && _camera.cameraObject) {
                _camera.takePhoto()
                event.accepted = true
            }
        }

        delegate: Item {
            id: _item
            height: _listSensors.height
            width: _listSensors.width
            property string src: ""

            Component.onCompleted: {
                _camera.cameraObject = display
            }

            Loader {
                id: _loader
                anchors.fill: parent
                sourceComponent: null
                Connections {
                    target: display
                    function onImageChanged() {
                        _camera.photoCounter++
                        _item.src = "image://cameraImageProvider/" + _camera.photoCounter
                        _loader.sourceComponent = _imageComponent
                    }
                    function onCameraUnavailable() {
                        _loader.sourceComponent = _txtComponent
                    }
                }
            }
            Component {
                id: _txtComponent
                Text {
                    id: _txtNotify
                    text: qsTr("Camera is not available")
                    anchors.centerIn: parent
                    font.pointSize: fontSizes.medium
                    color: activeTheme.textColor
                }
            }
            Component {
                id: _imageComponent
                Image {
                    id: _imageView
                    source: _item.src
                    width: parent.width
                    height: parent.height
                    fillMode: Image.PreserveAspectFit
                    cache: false
                }
            }
        }
    }

    ListView {
        id: _portsList
        anchors.fill: parent
        visible: !_camera.showPhoto
        model: _portsModel
        spacing: 5
        currentIndex: 1
        clip: true

        Keys.onPressed: {
            switch (event.key) {
            case Qt.Key_Down:
                _portsList.currentIndex = (_portsList.currentIndex + 1) % _portsModel.count
                event.accepted = true
                break
            case Qt.Key_Up:
                _portsList.currentIndex = (_portsList.currentIndex - 1 + _portsModel.count) % _portsModel.count
                event.accepted = true
                break
            case Qt.Key_Return:
                _camera.port = _portsModel.get(_portsList.currentIndex).portName
                _camera.showPhoto = true
                _listSensors.focus = true
                _camera.takePhoto()
                event.accepted = true
                break
            default:
                break
            }
        }

        delegate: Item {
            id: _portItem
            width: _portsList.width
            height: _portsList.height / 5.6
            property bool isCurrent: ListView.isCurrentItem

            MouseArea {
                anchors.fill: parent
                onPressed: { _portsList.currentIndex = model.index }
                onClicked: {
                    _camera.port = model.portName
                    _camera.showPhoto = true
                    _listSensors.focus = true
                    _camera.takePhoto()
                }
            }
            Rectangle {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                radius: 10
                color: _portItem.isCurrent ? activeTheme.focusElementsOfListColor : activeTheme.elementsOfListColor
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 7
                    spacing: parent.width < 400 ? 10 : 15
                    Image {
                        source: iconsPath + "camera.png"
                        Layout.preferredWidth: parent.width < 400 ? _camera.width / 5.5 : _camera.width / 23
                        Layout.preferredHeight: parent.width < 400 ? _camera.width / 5.5 : _camera.width / 23
                        Layout.alignment: Qt.AlignVCenter
                    }
                    Text {
                        text: model.portName
                        wrapMode: Text.Wrap
                        Layout.fillWidth: true
                        Layout.rightMargin: 7
                        Layout.alignment: Qt.AlignVCenter
                        font.pointSize: fontSizes.medium
                        color: activeTheme.textColor
                    }
                }
            }
        }
    }
}
