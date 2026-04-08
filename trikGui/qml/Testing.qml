import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import com.trikGui 1.0
import QtQml.Models 2.15

Rectangle {
    id: _mainItem
    property var testingManager: TestingManager
    property var idList: _listTesting
    color: activeTheme.backgroundColor
    Component.onCompleted: {
        // testingManager.configureVideoModule();
        testingManager.setQmlParent(_mainItem);
    }

    ListModel {
        id: dataModelTesting

        // ListElement {
        //     iconPath: "analog.png"
        //     filePath: "AnalogDigitalSensors.qml"
        //     text: qsTr("Analog sensors")
        //     type: Type.AnalogSensor
        // }
        // ListElement {
        //     iconPath: "digital.png"
        //     filePath: "AnalogDigitalSensors.qml"
        //     text: qsTr("Digital sensors")
        //     type: Type.DigitalSensor
        // }
        // Component.onCompleted: {
        //     if (testingManager.checkPwmCapture()) {
        //         dataModelTesting.insert(1, {
        //             "iconPath": "pwmcapture.png",
        //             "filePath": "AnalogDigitalSensors.qml",
        //             "text": qsTr("PWM capture"),
        //             "type": Type.PwmCapture
        //         });
        //     }
        // }

        ListElement {
            iconPath: "digital.png"
            filePath: "AnalogDigitalSensors.qml"
            text: qsTr("Sensors")
            type: Type.Sensors
        }
        ListElement {
            iconPath: "servoMotor.png"
            filePath: "Motors.qml"
            text: qsTr("Servo motors")
            type: Type.ServoMotor
        }
        ListElement {
            iconPath: "powerMotor.png"
            filePath: "Motors.qml"
            text: qsTr("Power motors")
            type: Type.PowerMotor
        }
        ListElement {
            iconPath: "encoder.png"
            filePath: "Encoders.qml"
            text: qsTr("Encoders")
            type: Type.Encoder
        }
        ListElement {
            iconPath: "accelerometer.png"
            filePath: "Accelerometer.qml"
            text: qsTr("Accelerometer")
            type: Type.Accelerometer
        }
        ListElement {
            iconPath: "gyroscope.png"
            filePath: "Gyroscope.qml"
            text: qsTr("Gyroscope")
            type: Type.Gyroscope
        }
        ListElement {
            iconPath: "camera.png"
            filePath: "Camera.qml"
            text: qsTr("Camera")
            type: Type.Camera
        }
    }

    ListView {
        id: _listTesting
        anchors.fill: parent
        model: dataModelTesting
        spacing: 5
        anchors.topMargin: 4
        anchors.bottomMargin: 4

        Keys.onPressed: {
            if (event.key === Qt.Key_Down && _listTesting.currentIndex === dataModelTesting.count - 1) {
                _listTesting.currentIndex = 0;
                _listTesting.positionViewAtIndex(0, ListView.Beginning);
                event.accepted = true;
            } else if (event.key === Qt.Key_Up && _listTesting.currentIndex === 0) {
                _listTesting.currentIndex = dataModelTesting.count - 1;
                _listTesting.positionViewAtIndex(dataModelTesting.count - 1, ListView.End);
                event.accepted = true;
            }
        }
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
            policy: _listTesting.contentHeight > _listTesting.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
        }
        delegate: Item {
            id: _delegateMode
            width: _listTesting.width
            height: _listTesting.height / 5.6
            property bool isCurrent: ListView.isCurrentItem
            function chooseTestingType() {
                if (model.text === qsTr("Servo motors") || model.text === qsTr("Power motors")) {
                    testingManager.createMotors(model.type);
                } else {
                    testingManager.createSensors(model.type);
                }
                var page = stack.push(model.filePath);
                if (page) {
                    page.idList.focus = true;
                    if (model.text === qsTr("Servo motors") || model.text === qsTr("Power motors")) {
                        page.motors.setQmlParent(page);
                    } else {
                        page.sensors.setQmlParent(page);
                    }
                }
            }

            Keys.onPressed: {
                switch (event.key) {
                case Qt.Key_Return:
                    _delegateMode.chooseTestingType();
                    break;
                default:
                    break;
                }
            }
            MouseArea {
                anchors.fill: parent
                onPressed: {
                    _listTesting.currentIndex = model.index;
                }

                onClicked: {
                    _delegateMode.chooseTestingType();
                }
            }
            Rectangle {
                id: _mode
                focus: isCurrent
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                radius: 10
                color: _delegateMode.isCurrent ? activeTheme.focusElementsOfListColor : activeTheme.elementsOfListColor
                RowLayout {
                    id: _row
                    anchors.fill: parent
                    anchors.leftMargin: 7
                    spacing: parent.width < 400 ? 10 : 15
                    Image {
                        id: _iconDevice
                        source: iconsPath + model.iconPath
                        Layout.preferredWidth: parent.width < 400 ? _mainItem.width / 5.5 : _mainItem.width / 23
                        Layout.preferredHeight: parent.width < 400 ? _mainItem.width / 5.5 : _mainItem.width / 23
                        Layout.alignment: Qt.AlignVCenter
                    }
                    Text {
                        id: _textMode
                        text: model.text
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
