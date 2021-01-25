/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Copyright (C) 2017 Mapbox, Inc.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef QGEOMAPMAPBOXGL_H
#define QGEOMAPMAPBOXGL_H

#include "qgeomappingmanagerenginemapboxgl.h"
#include <QtLocation/private/qgeomap_p.h>
#include <QtLocation/private/qgeomapparameter_p.h>

class QGeoMapMapboxGLPrivate;

class QGeoMapMapboxGL : public QGeoMap
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGeoMapMapboxGL)

public:
    QGeoMapMapboxGL(QGeoMappingManagerEngineMapboxGL *engine, QObject *parent);
    virtual ~QGeoMapMapboxGL();

    QString copyrightsStyleSheet() const override;
    void setMapboxGLSettings(const QMapboxGLSettings &, bool useChinaEndpoint);
    void setUseFBO(bool);
    void setMapItemsBefore(const QString &);
    Capabilities capabilities() const override;

private Q_SLOTS:
    // QMapboxGL
    void onMapChanged(QMapboxGL::MapChange);

    // QDeclarativeGeoMapItemBase
    void onMapItemPropertyChanged();
    void onMapItemSubPropertyChanged();
    void onMapItemUnsupportedPropertyChanged();
    void onMapItemGeometryChanged();

    // QGeoMapParameter
    void onParameterPropertyUpdated(QGeoMapParameter *param, const char *propertyName);

public Q_SLOTS:
    void copyrightsChanged(const QString &copyrightsHtml);

private:
    QSGNode *updateSceneGraph(QSGNode *oldNode, QQuickWindow *window) override;

    QGeoMappingManagerEngineMapboxGL *m_engine;
};

#endif // QGEOMAPMAPBOXGL_H
