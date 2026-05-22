// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEOTILEFETCHEROHOSMAPKIT_H
#define QGEOTILEFETCHEROHOSMAPKIT_H

#include <QtCore/qstring.h>
#include <QtLocation/private/qgeotiledmappingmanagerengine_p.h>
#include <QtLocation/private/qgeotilefetcher_p.h>

QT_BEGIN_NAMESPACE

QGeoTileFetcher *makeGeoTileFetcherOhosMapKit(
    const QString &authenticationKey, const QString &userAgent, int scaleFactor,
    QGeoTiledMappingManagerEngine *parent);

QT_END_NAMESPACE

#endif // QGEOTILEFETCHEROHOSMAPKIT_H
