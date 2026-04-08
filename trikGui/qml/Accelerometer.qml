import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: _accelerometer
    property var sensors: Sensors
    property var coords: [0, 0, 0]
    property var idList: _listSensors

    ListView {
        id: _listSensors
        anchors.fill: parent
        model: sensors
        delegate: Item {
            id: _listDelegateSensors
            Column {
                Rectangle {
                    id: _coordsInfo
                    width: _accelerometer.width
                    height: _accelerometer.height * 0.4
                    color: activeTheme.backgroundColor
                    Column {
                        spacing: 7
                        anchors.fill: parent
                        anchors.margins: 10

                        Text {
                            text: qsTr("Accelerometer:")
                            anchors.left: parent.left
                            font.pointSize: fontSizes.large
                            color: activeTheme.textColor
                        }
                        Text {
                            text: "x: " + coords[0].toString()
                            anchors.left: parent.left
                            font.pointSize: fontSizes.medium
                            color: activeTheme.textColor
                        }
                        Text {
                            text: "y: " + coords[1].toString()
                            anchors.left: parent.left
                            font.pointSize: fontSizes.medium
                            color: activeTheme.textColor
                        }
                        Text {
                            text: "z: " + coords[2].toString()
                            anchors.left: parent.left
                            font.pointSize: fontSizes.medium
                            color: activeTheme.textColor
                        }
                    }
                }
                Rectangle {
                    id: _tester
                    width: _accelerometer.width
                    height: _accelerometer.height * 0.6
                    color: activeTheme.backgroundColor
                    property real circleRadius: _tester.width < _tester.height ? _tester.width / 2.5 : _tester.height / 2.5
                    property real innerCircleRadius: _tester.width < _tester.height ? _tester.width / 21 : _tester.height / 21
                    Rectangle {
                        id: _circle
                        width: _tester.circleRadius * 2
                        height: _tester.circleRadius * 2
                        radius: _tester.circleRadius
                        border.color: activeTheme.gyroAccelBordersColor
                        color: activeTheme.gyroAccelBackColor
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                            verticalCenter: parent.verticalCenter
                        }
                        Rectangle {
                            width: _circle.height
                            height: 1
                            color: activeTheme.gyroAccelBordersColor
                            x: 0
                            y: _circle.height / 2
                        }
                        Rectangle {
                            width: _circle.height
                            height: 1
                            color: activeTheme.gyroAccelBordersColor
                            x: _circle.width / 2
                            y: 0
                            transform: Rotation {
                                angle: 90
                            }
                        }

                        Rectangle {
                            id: innerCircle
                            width: _tester.innerCircleRadius * 2
                            height: _tester.innerCircleRadius * 2
                            color: activeTheme.lightTrikColor
                            radius: _tester.innerCircleRadius
                            x: _tester.circleRadius - _tester.innerCircleRadius
                            y: _tester.circleRadius - _tester.innerCircleRadius

                            Connections {
                                target: display
                                function onCoordsChanged() {
                                    coords = display.coords;
                                    var x = coords[0];
                                    var y = coords[1];
                                    var z = coords[2];
                                    var norm = Math.sqrt(x * x + y * y + z * z);
                                    var acc = {
                                        "x": y * _circle.width / norm,
                                        "y": x * _circle.width / norm
                                    };
                                    var radius = (_circle.width - innerCircle.width) / 2;
                                    var smallNorm = Math.sqrt(acc.x * acc.x + acc.y * acc.y);
                                    var x2 = smallNorm > radius ? acc.x * radius / smallNorm : acc.x;
                                    var y2 = smallNorm > radius ? acc.y * radius / smallNorm : acc.y;
                                    innerCircle.x = x2 + _tester.circleRadius - _tester.innerCircleRadius;
                                    innerCircle.y = y2 + _tester.circleRadius - _tester.innerCircleRadius;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
