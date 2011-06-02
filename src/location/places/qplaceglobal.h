#ifndef QPLACEGLOBAL_H
#define QPLACEGLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QT_BUILD_PLACES_LIB)
#  define Q_PLACES_EXPORT Q_DECL_EXPORT
#else
#  define Q_PLACES_EXPORT Q_DECL_IMPORT
#endif

#define QT_PLACES_NAMESPACE QtPlaces

#endif // QPLACEGLOBAL_H

