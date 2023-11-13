// Copyright (C) 2017 The Qt Company Ltd.
// Copyright (C) 2017 Mapbox, Inc.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#ifndef QGEOTILEDMAPPINGMANAGERENGINEMAPBOXGL_H
#define QGEOTILEDMAPPINGMANAGERENGINEMAPBOXGL_H

#include <QtLocation/QGeoServiceProvider>
#include <QtLocation/private/qgeomappingmanagerengine_p.h>

#include <QMapboxGL>

QT_BEGIN_NAMESPACE

class QGeoMappingManagerEngineMapboxGL : public QGeoMappingManagerEngine
{
    Q_OBJECT

public:
    QGeoMappingManagerEngineMapboxGL(const QVariantMap &parameters,
                                        QGeoServiceProvider::Error *error, QString *errorString);
    ~QGeoMappingManagerEngineMapboxGL();

    QGeoMap *createMap() override;

private:
    QMapboxGLSettings m_settings;
    bool m_useFBO = true;
    bool m_useChinaEndpoint = false;
    QString m_mapItemsBefore;
};

QT_END_NAMESPACE

#endif // QGEOTILEDMAPPINGMANAGERENGINEMAPBOXGL_H
