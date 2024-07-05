#!/bin/bash
set -euxo pipefail

REQUIRED_PACKAGES="ccache coreutils python@3.${TRIK_PYTHON3_VERSION_MINOR} pkg-config"
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

mkdir -p "$BUILDDIR"
cd "$BUILDDIR"
TRIK_PYTHON=python3.${TRIK_PYTHON3_VERSION_MINOR}
"$TRIK_PYTHON" -m pip install -U pip
"$TRIK_PYTHON" -m pip install aqtinstall
"$TRIK_PYTHON" -m aqt install-qt -m qtscript -O "$HOME/Qt" mac desktop "${TRIK_QT_VERSION}" \
            --archives qtbase qtmultimedia qtsvg qttools qtserialport qtimageformats
# Force SDK version compatible with Qt 5.12
system_profiler -json SPDeveloperToolsDataType | jq '.SPDeveloperToolsDataType[].spdevtools_path' || :
sudo xcode-select -s /Applications/Xcode_13.1.app/Contents/Developer
xcodebuild -showsdks
xcrun -sdk macosx --show-sdk-path
