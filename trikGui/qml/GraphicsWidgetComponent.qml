import QtQuick 2.15
import com.trikGui 1.0

Rectangle {
    id: _graphicsContainer
    color: "transparent"

    Binding {
        target: GraphicsWidget
        property: "width"
        value: _graphicsContainer.width
    }
    Binding {
        target: GraphicsWidget
        property: "height"
        value: _graphicsContainer.height
    }

    Component.onCompleted: {
        GraphicsWidget.parent = _graphicsContainer;
    }

    // Note: no Component.onDestruction needed — QQuickItem::~QQuickItem() automatically
    // calls setParentItem(null) on all visual children, including the GraphicsWidget singleton.
    // Explicitly setting it here while inside GraphicsWidget's own hide() signal causes a crash.

    Keys.onPressed: {
        switch (event.key) {
        case Qt.Key_Escape:
            event.accepted = true;
            break;
        case Qt.Key_PowerOff:
            RunningCode.abortScript();
            event.accepted = true;
            break;
        case Qt.Key_W:
            if (event.modifiers & Qt.ControlModifier) {
                RunningCode.abortScript();
                event.accepted = true;
            }
            break;
        }
    }
}
