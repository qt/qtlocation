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

#ifndef QGEOMAPMAPBOXGL_P_H
#define QGEOMAPMAPBOXGL_P_H

#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QSharedPointer>
#include <QtCore/QTimer>
#include <QtCore/QVariant>
#include <QtCore/QRectF>
#include <QtLocation/private/qgeomap_p_p.h>
#include <QtLocation/private/qgeomapparameter_p.h>

class QMapboxGL;
class QMapboxGLStyleChange;

class QGeoMapMapboxGLPrivate : public QGeoMapPrivate
{
    Q_DECLARE_PUBLIC(QGeoMapMapboxGL)

public:
    QGeoMapMapboxGLPrivate(QGeoMappingManagerEngineMapboxGL *engine);

    ~QGeoMapMapboxGLPrivate();

    QSGNode *updateSceneGraph(QSGNode *oldNode, QQuickWindow *window);

    void addParameter(QGeoMapParameter *param) override;
    void removeParameter(QGeoMapParameter *param) override;

    QGeoMap::ItemTypes supportedMapItemTypes() const override;
    void addMapItem(QDeclarativeGeoMapItemBase *item) override;
    void removeMapItem(QDeclarativeGeoMapItemBase *item) override;

    /* Data members */
    enum SyncState : int {
        NoSync = 0,
        ViewportSync    = 1 << 0,
        CameraDataSync  = 1 << 1,
        MapTypeSync     = 1 << 2,
        VisibleAreaSync = 1 << 3
    };
    Q_DECLARE_FLAGS(SyncStates, SyncState);

    QMapboxGLSettings m_settings;
    bool m_useFBO = true;
    bool m_developmentMode = false;
    QString m_mapItemsBefore;

    QTimer m_refresh;
    bool m_shouldRefresh = true;
    bool m_warned = false;
    bool m_threadedRendering = false;
    bool m_styleLoaded = false;

    SyncStates m_syncState = NoSync;

    QList<QSharedPointer<QMapboxGLStyleChange>> m_styleChanges;

protected:
    void changeViewportSize(const QSize &size) override;
    void changeCameraData(const QGeoCameraData &oldCameraData) override;
    void changeActiveMapType(const QGeoMapType mapType) override;

    void setVisibleArea(const QRectF &visibleArea) override;
    QRectF visibleArea() const override;

private:
    Q_DISABLE_COPY(QGeoMapMapboxGLPrivate);

    void syncStyleChanges(QMapboxGL *map);
    void threadedRenderingHack(QQuickWindow *window, QMapboxGL *map);

    QRectF m_visibleArea;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGeoMapMapboxGLPrivate::SyncStates)

#endif // QGEOMAPMAPBOXGL_P_H
