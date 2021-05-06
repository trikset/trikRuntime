#!/bin/bash
set -xuevo pipefail
TRIK_LINTER_OK=true
if $VERA && which vera++ >/dev/null 2>&1 ; then scripts/azure/runVera++.sh || TRIK_LINTER_OK=false ; fi
LUPDATE=lupdate
which lupdate-pro >/dev/null 2>&1 && LUPDATE=lupdate-pro
if $TRANSLATIONS ; then $LUPDATE trikRuntime.pro || TRIK_LINTER_OK=false ; fi
scripts/azure/checkStatus.sh || TRIK_LINTER_OK=false
"$TRIK_LINTER_OK"
