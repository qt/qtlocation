// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QPLACEMANAGERENGINEOHOSMAPKIT_H
#define QPLACEMANAGERENGINEOHOSMAPKIT_H

#include <QtCore/QVariantMap>
#include <QtLocation/QGeoServiceProvider>
#include <QtLocation/QPlaceManagerEngine>

QT_BEGIN_NAMESPACE

QPlaceManagerEngine *makeQPlaceManagerEngineOhosMapKit(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString);

QT_END_NAMESPACE

#endif // QPLACEMANAGERENGINEOHOSMAPKIT_H
