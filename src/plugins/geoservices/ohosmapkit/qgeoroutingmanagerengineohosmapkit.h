// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEOROUTINGMANAGERENGINEOHOSMAPKIT_H
#define QGEOROUTINGMANAGERENGINEOHOSMAPKIT_H

#include <QtCore/QVariantMap>
#include <QtLocation/QGeoRoutingManagerEngine>
#include <QtLocation/QGeoServiceProvider>

QT_BEGIN_NAMESPACE

QGeoRoutingManagerEngine *makeQGeoRoutingManagerEngineOhosMapKit(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString);

QT_END_NAMESPACE

#endif // QGEOROUTINGMANAGERENGINEOHOSMAPKIT_H
