import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: _gyroscope
    property var sensors: Sensors
    property int maxGyroscopeVal: 100000
    property var coords: [0, 0, 0]
    property int xShift: 0
    property int yShift: 0
    property real zShift: -Math.PI / 2
    property var idList: _listSensors

    ListView {
        id: _listSensors
        anchors.fill: parent
        model: sensors
        delegate: Item {
            id: _listDelegateSensors
            anchors.fill: parent
            Column {
                Rectangle {
                    id: _coordsInfo
                    width: _gyroscope.width
                    height: _gyroscope.height * 0.4
                    color: activeTheme.backgroundColor
                    Column {
                        spacing: 7
                        anchors.fill: parent
                        anchors.margins: 10

                        Text {
                            text: qsTr("Gyroscope:")
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
                    width: _gyroscope.width
                    height: _gyroscope.height * 0.6
                    color: activeTheme.backgroundColor
                    property real circleRadius: _tester.width < _tester.height ? _tester.width / 2.5 : _tester.height / 2.5
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
                        Canvas {
                            id: _lines
                            anchors.fill: parent
                            onPaint: {
                                var ctx = getContext("2d");
                                ctx.clearRect(0, 0, width, height);
                                ctx.strokeStyle = "red";
                                ctx.lineWidth = 2;
                                ctx.beginPath();
                                ctx.moveTo(_circle.width / 2, _circle.height / 2);
                                ctx.lineTo(_circle.width / 2 + xShift, _circle.height / 2);
                                ctx.stroke();
                                ctx.beginPath();
                                ctx.strokeStyle = activeTheme.lightTrikColor;
                                ctx.moveTo(_circle.width / 2, _circle.height / 2);
                                ctx.lineTo(_circle.width / 2, _circle.height / 2 + yShift);
                                ctx.stroke();
                                if (zShift >= Math.PI / 2 && zShift < 3 * Math.PI / 2) {
                                    ctx.beginPath();
                                    ctx.strokeStyle = "blue";
                                    ctx.arc(_circle.width / 2, _circle.height / 2, _tester.circleRadius, 3 * Math.PI / 2, zShift, true);
                                    ctx.stroke();
                                } else if (zShift > 3 * Math.PI / 2 && zShift <= 5 * Math.PI / 2) {
                                    ctx.beginPath();
                                    ctx.strokeStyle = "blue";
                                    ctx.arc(_circle.width / 2, _circle.height / 2, _tester.circleRadius, 3 * Math.PI / 2, zShift);
                                    ctx.stroke();
                                }
                            }
                        }

                        Connections {
                            target: display
                            function constrain(value, max, min) {
                                return (value > max) ? max : (value < min) ? min : value;
                            }
                            function onCoordsChanged() {
                                coords = display.coords;
                                var x = coords[0];
                                var y = coords[1];
                                var z = -coords[2];
                                xShift = constrain(x, _gyroscope.maxGyroscopeVal, -_gyroscope.maxGyroscopeVal) / _gyroscope.maxGyroscopeVal * _tester.circleRadius;
                                yShift = constrain(y, _gyroscope.maxGyroscopeVal, -_gyroscope.maxGyroscopeVal) / _gyroscope.maxGyroscopeVal * _tester.circleRadius;
                                var constrainZ = constrain(z, _gyroscope.maxGyroscopeVal, -_gyroscope.maxGyroscopeVal);
                                zShift = ((constrainZ - (-_gyroscope.maxGyroscopeVal)) * (5 * Math.PI / 2 - Math.PI / 2)) / (_gyroscope.maxGyroscopeVal - (-_gyroscope.maxGyroscopeVal)) + Math.PI / 2;
                                _lines.requestPaint();
                            }
                        }
                    }
                }
            }
        }
    }
}
