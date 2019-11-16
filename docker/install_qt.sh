#!/bin/bash
set -ueo pipefail
export TRIK_QT_INSTALL_DIR="${1:-$HOME/Qt}"
echo "Install Qt to $TRIK_QT_INSTALL_DIR"
INSTALLER=/tmp/qt_installer.run
[ -e "$INSTALLER" ] || curl -L -o "$INSTALLER" http://download.qt-project.org/official_releases/online_installers/qt-unified-linux-x64-online.run \
&& chmod +x "$INSTALLER"
"$INSTALLER" --verbose --platform minimal --no-force-installations --script $(dirname $0)/qt_scriptinstall.qs "${@:2}" \
&& rm -f /tmp/qt_installer.run
