// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEOFILETILECACHEOHOSMAPKIT_H
#define QGEOFILETILECACHEOHOSMAPKIT_H

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtLocation/private/qgeofiletilecache_p.h>

QT_BEGIN_NAMESPACE

QGeoFileTileCache *makeGeoFileTileCacheOhosMapKit(
    int scaleFactor, const QString &directory, QObject *parent);

QT_END_NAMESPACE

#endif // QGEOFILETILECACHEOHOSMAPKIT_H
