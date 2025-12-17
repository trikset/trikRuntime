#!/bin/bash
set -euxo pipefail

REQUIRED_PACKAGES="coreutils pkg-config"
export HOMEBREW_TEMP="$HOME/homebrew.tmp"
export HOMEBREW_LOGS="$HOMEBREW_TEMP"
#To turn autoupdate on use `unset HOMEBREW_NO_AUTO_UPDATE` in a sub-shell before `brew install`
export HOMEBREW_NO_INSTALL_CLEANUP=1
export HOMEBREW_NO_AUTO_UPDATE=1
#touch Brewfile
#brew bundle --force cleanup
#brew unlink python@2
#brew update
for pkg in $REQUIRED_PACKAGES ; do
    p="${pkg##*/}"
    p="${p%.*}"
    brew install "$pkg" || brew upgrade "$pkg" || brew link --force "$pkg" || echo "Failed to install/upgrade $pkg"
done

TRIK_PYTHON=python${TRIK_PYTHON_VERSION}
"$TRIK_PYTHON" -m pip install -U pip
"$TRIK_PYTHON" -m pip install aqtinstall
"$TRIK_PYTHON" -m aqt install-qt -m qtscript -O "$HOME/Qt" mac desktop "${TRIK_QT_VERSION}"
QT_DIR=$(ls -dv "$HOME"/Qt/${TRIK_QT_VERSION}*/*/bin | head -n 1)
echo "$QT_DIR" >> $GITHUB_PATH
            
sudo xcode-select -s /Applications/Xcode_15.3.app/Contents/Developer
xcodebuild -showsdks
xcrun -sdk macosx --show-sdk-path
