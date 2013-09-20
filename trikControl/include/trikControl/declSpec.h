/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <QtCore/qglobal.h>

#ifndef TRIKCONTROL_EXPORT
#  if defined(TRIKCONTROL_LIBRARY)
#    define TRIKCONTROL_EXPORT Q_DECL_EXPORT
#  else
#    define TRIKCONTROL_EXPORT Q_DECL_IMPORT
#  endif
#endif
