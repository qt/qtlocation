// Copyright (C) 2014 Canonical Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#ifndef QGEOMAPPINGMANAGERENGINEITEMSOVERLAY_H
#define QGEOMAPPINGMANAGERENGINEITEMSOVERLAY_H

#include <QtLocation/QGeoServiceProvider>
#include <QtLocation/private/qgeomappingmanagerengine_p.h>

QT_BEGIN_NAMESPACE

class QGeoMappingManagerEngineItemsOverlay : public QGeoMappingManagerEngine
{
    Q_OBJECT

public:
    QGeoMappingManagerEngineItemsOverlay(const QVariantMap &parameters,
                                        QGeoServiceProvider::Error *error, QString *errorString);
    ~QGeoMappingManagerEngineItemsOverlay();

    QGeoMap *createMap() override;
};

QT_END_NAMESPACE

#endif // QGEOMAPPINGMANAGERENGINENOMAP_H
