import QtQuick 2.15

Rectangle {
    id: _camera
    property var sensors: Sensors
    property var idList: _listSensors
    property var cameraObject: null
    property int photoCounter: 0
    color: activeTheme.backgroundColor

    ListView {
        id: _listSensors
        anchors.fill: parent
        model: sensors

        Keys.onPressed: {
            if (event.key === Qt.Key_Return && _camera.cameraObject) {
                _camera.cameraObject.doPhoto()
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
                display.doPhoto()
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
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
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
}
