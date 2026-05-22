// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEOTILEDMAPPINGMANAGERENGINEOHOSMAPKIT_H
#define QGEOTILEDMAPPINGMANAGERENGINEOHOSMAPKIT_H

#include <QtCore/QVariantMap>
#include <QtLocation/QGeoServiceProvider>
#include <QtLocation/private/qgeotiledmappingmanagerengine_p.h>

QT_BEGIN_NAMESPACE

QGeoTiledMappingManagerEngine *makeQGeoTiledMappingManagerEngineOhosMapKit(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString);

QT_END_NAMESPACE

#endif // QGEOTILEDMAPPINGMANAGERENGINEOHOSMAPKIT_H
