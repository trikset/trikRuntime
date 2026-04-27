import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQml.Models 2.15

Rectangle {
    id: _communication
    color: activeTheme.backgroundColor
    property var communicationSettings: CommunicationSettingsServer
    property var objectForQmlParentSetting: communicationSettings
    property var idList: _listDigitsHull

    property bool showIpError: false

    Connections {
        target: communicationSettings
        function onInvalidIpEntered() {
            _communication.showIpError = true;
        }
    }

    Component.onCompleted: {
        communicationSettings.setQmlParent(_communication);
    }

    Column {
        id: _listsDigits
        spacing: 11
        anchors.fill: parent
        anchors.topMargin: 10
        property string whatFocused: "hull"

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Comm settings")
            font.pointSize: fontSizes.large
            font.bold: true
            color: activeTheme.textColor
            width: parent.width
            wrapMode: Text.Wrap
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("(Press 'Enter' to edit)")
            font.pointSize: fontSizes.small
            color: activeTheme.textColor
            width: parent.width
            wrapMode: Text.WordWrap
        }
        Text {
            anchors.left: parent.left
            anchors.leftMargin: 10
            text: qsTr("Hull number:")
            font.pointSize: fontSizes.large
            color: activeTheme.textColor
        }
        Item {
            width: parent.width / 3.5
            height: parent.height / 8
            anchors.horizontalCenter: parent.horizontalCenter
            Row {
                width: parent.width
                height: parent.height
                spacing: 2
                Repeater {
                    id: _listDigitsHull
                    property int currentIndex: 0
                    property bool focusDigit: false
                    property int countOfHullElems: 2
                    property string hullNumberStr: communicationSettings.hullNumber
                    model: countOfHullElems
                    width: parent.width
                    height: parent.height

                    function applyHullNumber() {
                        var result = 0;
                        for (var i = 0; i < count; i++)
                            result = result * 10 + itemAt(i).currentValue;
                        _communication.communicationSettings.setHullNumber(result);
                    }

                    function moveTo(idx) {
                        currentIndex = idx;
                        itemAt(idx).focus = true;
                    }

                    delegate: Rectangle {
                        id: _digitHull
                        readonly property bool isActive: _listsDigits.whatFocused === "hull" && _listDigitsHull.currentIndex === index
                        readonly property bool isEditing: isActive && _listDigitsHull.focusDigit
                        property int currentValue: parseInt(_listDigitsHull.hullNumberStr[index])

                        width: parent.width / 2
                        height: parent.height
                        radius: 5
                        color: activeTheme.cellsColor
                        border.color: isEditing ? "red" : isActive ? activeTheme.lightOrStandartGreenColor : activeTheme.commSettingsBorderColor
                        border.width: (isEditing || isActive) ? 3 : 1

                        Text {
                            anchors.centerIn: parent
                            text: currentValue
                            font.pointSize: fontSizes.medium
                            color: activeTheme.textColor
                        }

                        Keys.onPressed: {
                            switch (event.key) {
                            case Qt.Key_Return:
                                _listDigitsHull.focusDigit = !_listDigitsHull.focusDigit;
                                break;
                            case Qt.Key_Up:
                                if (_listDigitsHull.focusDigit) {
                                    currentValue = (currentValue + 1) % 10;
                                    _listDigitsHull.applyHullNumber();
                                } else {
                                    _listsDigits.whatFocused = "button";
                                    _button.focus = true;
                                }
                                break;
                            case Qt.Key_Down:
                                if (_listDigitsHull.focusDigit) {
                                    currentValue = (currentValue - 1 + 10) % 10;
                                    _listDigitsHull.applyHullNumber();
                                } else {
                                    _listsDigits.whatFocused = "ip";
                                    _listDigitsIp.moveTo(_listDigitsIp.currentIndex);
                                }
                                break;
                            case Qt.Key_Right:
                                if (index < _listDigitsHull.countOfHullElems - 1)
                                    _listDigitsHull.moveTo(index + 1);
                                break;
                            case Qt.Key_Left:
                                if (index > 0)
                                    _listDigitsHull.moveTo(index - 1);
                                break;
                            default:
                                break;
                            }
                        }
                    }
                }
            }
        }
        Text {
            anchors.left: parent.left
            anchors.leftMargin: 10
            text: qsTr("Leader IP:")
            font.pointSize: fontSizes.large
            color: activeTheme.textColor
        }
        Text {
            anchors.left: parent.left
            anchors.leftMargin: 10
            text: qsTr("(last two numbers)")
            font.pointSize: fontSizes.small
            color: activeTheme.textColor
        }
        Item {
            width: parent.width / 1.5
            height: parent.height / 8
            anchors.horizontalCenter: parent.horizontalCenter
            Row {
                width: parent.width
                height: parent.height
                spacing: 2

                Repeater {
                    id: _listDigitsIp
                    property int currentIndex: 0
                    property bool focusDigit: false
                    property int countOfIpElems: 7
                    property int indexOfSeparator: 3
                    property string ip: communicationSettings.thirdFourthIpPart
                    model: countOfIpElems
                    width: parent.width
                    height: parent.height

                    function nextDigitIndex(from, delta) {
                        var next = from + delta;
                        if (next === indexOfSeparator)
                            next += delta;
                        return next;
                    }

                    function moveTo(idx) {
                        currentIndex = idx;
                        itemAt(idx).focus = true;
                    }

                    delegate: Rectangle {
                        id: _digitIp
                        readonly property bool isSeparator: index === _listDigitsIp.indexOfSeparator
                        readonly property bool isActive: !isSeparator && _listsDigits.whatFocused === "ip" && _listDigitsIp.currentIndex === index
                        readonly property bool isEditing: isActive && _listDigitsIp.focusDigit
                        // First digit of each octet: 0–2 (octet max is 255).
                        // Other digits: 0–9, full range; server validates the combination.
                        readonly property int maxValue: (index === 0 || index === 4) ? 3 : 10
                        property int currentValue: isSeparator ? 0 : parseInt(_listDigitsIp.ip[index])

                        width: isSeparator ? parent.width / 11 : parent.width / 7
                        height: parent.height
                        radius: 5
                        color: isSeparator ? activeTheme.backgroundColor : activeTheme.cellsColor
                        border.color: isEditing ? "red" : isActive ? activeTheme.lightOrStandartGreenColor : isSeparator ? "transparent" : activeTheme.commSettingsBorderColor
                        border.width: (isEditing || isActive) ? 3 : 1

                        Text {
                            anchors.centerIn: parent
                            text: isSeparator ? "." : currentValue
                            font.pointSize: isSeparator ? fontSizes.separator : fontSizes.medium
                            color: activeTheme.textColor
                        }

                        Keys.onPressed: {
                            if (isSeparator)
                                return;
                            switch (event.key) {
                            case Qt.Key_Return:
                                _listDigitsIp.focusDigit = !_listDigitsIp.focusDigit;
                                break;
                            case Qt.Key_Up:
                                if (_listDigitsIp.focusDigit) {
                                    currentValue = (currentValue + 1) % maxValue;
                                } else {
                                    _listsDigits.whatFocused = "hull";
                                    _listDigitsHull.moveTo(_listDigitsHull.currentIndex);
                                }
                                break;
                            case Qt.Key_Down:
                                if (_listDigitsIp.focusDigit) {
                                    currentValue = (currentValue - 1 + maxValue) % maxValue;
                                } else {
                                    _listsDigits.whatFocused = "button";
                                    _button.focus = true;
                                }
                                break;
                            case Qt.Key_Right:
                                {
                                    var next = _listDigitsIp.nextDigitIndex(index, 1);
                                    if (next < _listDigitsIp.countOfIpElems)
                                        _listDigitsIp.moveTo(next);
                                    break;
                                }
                            case Qt.Key_Left:
                                {
                                    var prev = _listDigitsIp.nextDigitIndex(index, -1);
                                    if (prev >= 0)
                                        _listDigitsIp.moveTo(prev);
                                    break;
                                }
                            default:
                                break;
                            }
                        }
                    }
                }
            }
        }
        Button {
            id: _button
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 8
            height: parent.height / 10
            background: Rectangle {
                radius: 10
                color: {
                    if (_communication.showIpError)
                        return _button.focus ? activeTheme.errorColor : activeTheme.errorColorLight;
                    return _button.focus ? activeTheme.darkTrikColor : activeTheme.buttonsColor;
                }
            }

            contentItem: Text {
                text: _communication.showIpError ? qsTr("Invalid IP") : qsTr("Connect")
                font.pointSize: fontSizes.medium
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            function applyConnect() {
                _communication.showIpError = false;
                var result = "";
                for (var i = 0; i < _listDigitsIp.count; i++) {
                    var d = _listDigitsIp.itemAt(i);
                    result += d.isSeparator ? "." : d.currentValue;
                }
                _communication.communicationSettings.connectToLeader(result);
            }

            Keys.onPressed: {
                switch (event.key) {
                case Qt.Key_Up:
                    _listsDigits.whatFocused = "ip";
                    _listDigitsIp.moveTo(_listDigitsIp.currentIndex);
                    break;
                case Qt.Key_Return:
                    _button.applyConnect();
                    break;
                case Qt.Key_Down:
                    _listsDigits.whatFocused = "hull";
                    _listDigitsHull.moveTo(_listDigitsHull.currentIndex);
                    break;
                default:
                    break;
                }
            }
        }
    }
}
