// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEOCODINGMANAGERENGINEOHOSMAPKIT_H
#define QGEOCODINGMANAGERENGINEOHOSMAPKIT_H

#include <QtCore/QVariantMap>
#include <QtLocation/QGeoCodingManagerEngine>
#include <QtLocation/QGeoServiceProvider>

QT_BEGIN_NAMESPACE

QGeoCodingManagerEngine *makeQGeoCodingManagerEngineOhosMapKit(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString);

QT_END_NAMESPACE

#endif // QGEOCODINGMANAGERENGINEOHOSMAPKIT_H
