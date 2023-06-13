#!/bin/bash
set -euxo pipefail

REQUIRED_PACKAGES="ccache coreutils python@3.9 pkg-config qt@5"
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
