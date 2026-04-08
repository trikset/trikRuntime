import QtQuick 2.15
import com.trikGui 1.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: _mainItem
    property var wiFiMode: WiFiModeServer
    property var idList: pageLoader
    property var wiFiSelectionComponent: null
    color: activeTheme.backgroundColor
    Component.onCompleted: {
        wiFiMode.setQmlParent(_mainItem)

        var currentMode = wiFiMode.currentMode()
        if (currentMode === Mode.Client) {
            wiFiMode.setMode(Mode.Client)
        } else if (currentMode === Mode.AccessPoint) {
            wiFiMode.setMode(Mode.AccessPoint)
        } else {
            pageLoader.setSource("WiFiModeSelection.qml")
        }
    }
    Loader {
        id: pageLoader
        anchors.fill: parent
        onLoaded: {
            if (pageLoader.item.idList) {
                pageLoader.item.idList.focus = true
            } else {
                pageLoader.item.focus = true
            }
        }
    }
    Connections {
        target: wiFiMode
        property var wiFiInitComponent: null
        function onInitStatusChanged() {
            switch (wiFiMode.initStatus) {
            case "start":
                pageLoader.setSource("WiFiInit.qml")
                break
            case "WiFiClient":
                pageLoader.setSource("WiFiClient.qml")
                break
            case "WiFiAP":
                pageLoader.setSource("WiFiAP.qml")
                break
            case "error":
                stack.currentItem.destroy()
                stack.pop()
                stack.currentItem.idList.focus = true
                break
            }
        }
    }
}
