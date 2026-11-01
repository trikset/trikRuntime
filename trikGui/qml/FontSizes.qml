import QtQuick 2.15

QtObject {
    // The old UI was designed for a 240x320 display, the new one is 480x854.
    // font.pointSize is a physical unit, so it should not be scaled by the
    // pixel ratio (2x). Start with 1.5x and tune individual roles if needed.
    property real scale: 1.5

    property int microscopic: Math.round(6 * scale)  // info just to have it there, f.e. icon links
    property int tiny: Math.round(8 * scale)         // hints, minor notes
    property int small: Math.round(9 * scale)        // status bar, captions
    property int body: Math.round(11 * scale)        // dialog text, info labels
    property int medium: Math.round(12 * scale)      // general body text
    property int large: Math.round(14 * scale)       // section headers
    property int title: Math.round(19 * scale)       // main menu icon labels
    property int separator: Math.round(23 * scale)   // IP separator dot
}
