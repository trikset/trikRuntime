#!/bin/bash -x
TRIK_QT_INSTALL_DIR=${TRIK_QT_INSTALL_DIR:-$HOME/TRIK_Qt}
SCRIPT_DIR=$(dirname ${BASH_SOURCE[0]})
set -ueo pipefail
D=/tmp/qt-installer.dmg
test -r "$D" || curl -L -o "$D" http://download.qt-project.org/official_releases/online_installers/qt-unified-mac-x64-online.dmg
P=$(hdiutil attach "$D" -noverify -noautofsck | grep -o '/Volumes/.*$')
I=$(find "$P" -type f -name '*qt-unified-*' -path '*Contents/MacOS/*' -print)
env TRIK_QT_INSTALL_DIR="$TRIK_QT_INSTALL_DIR" "$I" --verbose --no-force-installations --show-virtual-components --script "$SCRIPT_DIR/../docker/qt_scriptinstall.qs"

#remove garbage. No way to deselect this in installer script.
rm -rf "$TRIK_QT_INSTALL_DIR"/{Docs,Examples,"Qt Creator.app",MaintenanceTool.app,MaintenanceTool.dat}

du -csh "$TRIK_QT_INSTALL_DIR"/* | sort -h
