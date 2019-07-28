#!/bin/sh
. /etc/profile.d/locale.sh
. /etc/profile.d/qws_display.sh

cd /home/root/trik
export LD_LIBRARY_PATH=. && ./trikGui -c configs/kernel-3.6 1>/dev/null 2>&1
