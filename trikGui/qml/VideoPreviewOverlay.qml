import QtQuick 2.15
import com.trikGui 1.0

Item {
    id: _previewOverlay
    visible: VideoPreviewHub.activeIds.length > 0
    width: parent ? parent.width : 0
    height: previewItem.contentHeight

    VideoPreviewOverlayItem {
        id: previewItem
        anchors.fill: parent
        source: VideoPreviewHub
    }
}
