import QtQuick 2.15
import com.trikGui 1.0

Item {
    id: _previewList
    visible: VideoPreviewHub.activeIds.length > 0

    Flickable {
        id: _flickable
        anchors.fill: parent
        clip: true
        contentWidth: width
        contentHeight: _column.height
        boundsBehavior: Flickable.StopAtBounds
        flickableDirection: Flickable.VerticalFlick

        Column {
            id: _column
            width: _flickable.width
            spacing: 4

            Repeater {
                model: VideoPreviewHub.previewModel

                delegate: VideoPreviewStreamItem {
                    width: _column.width
                    height: width / frameAspect
                    source: VideoPreviewHub
                    previewId: streamId
                }
            }
        }
    }
}
