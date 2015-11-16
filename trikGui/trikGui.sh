#!/bin/sh
. /etc/profile.d/locale.sh
. /etc/profile.d/qws_display.sh
export LD_LIBRARY_PATH=/home/root/trik/ && /home/root/trik/trikGui -qws 2>&1 1> /dev/null
