#pragma once

#include <QtCore/qglobal.h>

#ifndef TRIKCONTROL_EXPORT
#  if defined(TRIKCONTROL_LIBRARY)
#    define TRIKCONTROL_EXPORT Q_DECL_EXPORT
#  else
#    define TRIKCONTROL_EXPORT Q_DECL_IMPORT
#  endif
#endif
