#!/bin/bash
export TRIK_QT_INSTALL_DIR="${1:-$HOME/Qt}"
echo "Install Qt to $TRIK_QT_INSTALL_DIR"
curl -L -o /tmp/qt_installer.run http://download.qt-project.org/official_releases/online_installers/qt-unified-linux-x64-online.run
chmod +x /tmp/qt_installer.run
/tmp/qt_installer.run ${@:2} --script $(dirname $0)/qt_scriptinstall.qs
rm -f /tmp/qt_installer.run
