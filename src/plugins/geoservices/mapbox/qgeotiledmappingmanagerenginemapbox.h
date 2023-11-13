// Copyright (C) 2014 Canonical Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#ifndef QGEOTILEDMAPPINGMANAGERENGINEMAPBOX_H
#define QGEOTILEDMAPPINGMANAGERENGINEMAPBOX_H

#include <QtLocation/QGeoServiceProvider>

#include <QtLocation/private/qgeotiledmappingmanagerengine_p.h>

QT_BEGIN_NAMESPACE

class QGeoTiledMappingManagerEngineMapbox : public QGeoTiledMappingManagerEngine
{
    Q_OBJECT

public:
    QGeoTiledMappingManagerEngineMapbox(const QVariantMap &parameters,
                                        QGeoServiceProvider::Error *error, QString *errorString);
    ~QGeoTiledMappingManagerEngineMapbox();

    QGeoMap *createMap() override;

private:
    QString m_cacheDirectory;
};

QT_END_NAMESPACE

#endif // QGEOTILEDMAPPINGMANAGERENGINEMAPBOX_H
