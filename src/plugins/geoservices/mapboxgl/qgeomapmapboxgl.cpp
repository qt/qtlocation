/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Copyright (C) 2017 Mapbox, Inc.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeomapmapboxgl.h"
#include "qsgmapboxglnode.h"

#include <QtCore/QByteArray>
#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtLocation/private/qgeomap_p_p.h>
#include <QtLocation/private/qgeoprojection_p.h>
#include <QtLocation/private/qdeclarativegeomapitembase_p.h>
#include <QtLocation/private/qdeclarativepolylinemapitem_p.h>

#include "qgeopath.h"

#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGImageNode>
#include <QtQuick/private/qsgtexture_p.h>

#include <QMapboxGL>

#include <cmath>

// FIXME: Expose from Mapbox GL constants
#define MBGL_TILE_SIZE 512.0

namespace {

// WARNING! The development token is subject to Mapbox Terms of Services
// and must not be used in production.
static char developmentToken[] =
    "pk.eyJ1IjoicXRzZGsiLCJhIjoiY2l5azV5MHh5MDAwdTMybzBybjUzZnhxYSJ9.9rfbeqPjX2BusLRDXHCOBA";

static const double invLog2 = 1.0 / std::log(2.0);

static double zoomLevelFrom256(double zoomLevelFor256, double tileSize)
{
    return std::log(std::pow(2.0, zoomLevelFor256) * 256.0 / tileSize) * invLog2;
}

} // namespace

class QGeoMapMapboxGLPrivate : public QGeoMapPrivate
{
    Q_DECLARE_PUBLIC(QGeoMapMapboxGL)

public:
    QGeoMapMapboxGLPrivate(QGeoMappingManagerEngineMapboxGL *engine);

    ~QGeoMapMapboxGLPrivate();

    QSGNode *updateSceneGraph(QSGNode *oldNode, QQuickWindow *window);

    QGeoMap::ItemTypes supportedMapItemTypes() const Q_DECL_OVERRIDE;
    void addMapItem(QDeclarativeGeoMapItemBase *item) Q_DECL_OVERRIDE;
    void removeMapItem(QDeclarativeGeoMapItemBase *item) Q_DECL_OVERRIDE;

    /* Data members */
    enum SyncState : int {
        NoSync = 0,
        ViewportSync   = 1 << 0,
        CameraDataSync = 1 << 1,
        MapTypeSync    = 1 << 2
    };
    Q_DECLARE_FLAGS(SyncStates, SyncState);

    QMapboxGLSettings m_settings;
    bool m_developmentMode = false;

    QTimer m_refresh;
    bool m_shouldRefresh = true;
    bool m_warned = false;

    QHash<QDeclarativeGeoMapItemBase *, QMapbox::AnnotationID> m_managedMapItems;
    QList<QDeclarativeGeoMapItemBase *> m_mapItemsToAdd;
    QList<QDeclarativeGeoMapItemBase *> m_mapItemsToUpdate;
    QList<QDeclarativeGeoMapItemBase *> m_mapItemsToRemove;

    SyncStates m_syncState = NoSync;

protected:
    void changeViewportSize(const QSize &size) Q_DECL_OVERRIDE;
    void changeCameraData(const QGeoCameraData &oldCameraData) Q_DECL_OVERRIDE;
    void changeActiveMapType(const QGeoMapType mapType) Q_DECL_OVERRIDE;

private:
    Q_DISABLE_COPY(QGeoMapMapboxGLPrivate)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGeoMapMapboxGLPrivate::SyncStates)

QGeoMapMapboxGLPrivate::QGeoMapMapboxGLPrivate(QGeoMappingManagerEngineMapboxGL *engine)
    : QGeoMapPrivate(engine, new QGeoProjectionWebMercator)
{
}

QGeoMapMapboxGLPrivate::~QGeoMapMapboxGLPrivate()
{
}

QSGNode *QGeoMapMapboxGLPrivate::updateSceneGraph(QSGNode *oldNode, QQuickWindow *window)
{
    Q_Q(QGeoMapMapboxGL);

    if (!m_warned) {
        if (window->openglContext()->thread() != QCoreApplication::instance()->thread()) {
            qWarning() << "Threaded rendering is not supported by Mapbox GL plugin.";
            QMetaObject::invokeMethod(&m_refresh, "start", Qt::QueuedConnection);
        }

        m_warned = true;
    }

    QSGMapboxGLNode *mbglNode = static_cast<QSGMapboxGLNode *>(oldNode);
    if (!mbglNode) {
        mbglNode = new QSGMapboxGLNode(m_settings, m_viewportSize, window->devicePixelRatio(), q);
        m_syncState = MapTypeSync | CameraDataSync | ViewportSync;
    }

    QMapboxGL *map = mbglNode->map();

    if (m_syncState & MapTypeSync) {
        m_developmentMode = m_activeMapType.name().startsWith("mapbox://")
            && m_settings.accessToken() == developmentToken;

        map->setStyleUrl(m_activeMapType.name());
    }

    for (QDeclarativeGeoMapItemBase *mapItem : m_mapItemsToRemove) {
        Q_ASSERT(m_managedMapItems.contains(mapItem));
        q->disconnect(mapItem);

        QDeclarativePolylineMapItem *polyline = qobject_cast<QDeclarativePolylineMapItem *>(mapItem);
        QDeclarativeMapLineProperties *lineStyle = polyline->line();
        q->disconnect(lineStyle);

        map->removeAnnotation(m_managedMapItems.take(mapItem));
        if (m_mapItemsToUpdate.contains(mapItem)) {
            m_mapItemsToUpdate.removeAll(mapItem);
        }
    }
    m_mapItemsToRemove.clear();

    for (QDeclarativeGeoMapItemBase *mapItem : m_mapItemsToAdd) {
        Q_ASSERT(!m_managedMapItems.contains(mapItem));

        QMapbox::Coordinates coordinates;
        const QGeoPath *path = static_cast<const QGeoPath *>(&mapItem->geoShape());
        for (const QGeoCoordinate &coordinate : path->path()) {
            coordinates << QMapbox::Coordinate { coordinate.latitude(), coordinate.longitude() };
        }

        QDeclarativePolylineMapItem *polyline = qobject_cast<QDeclarativePolylineMapItem *>(mapItem);
        QObject::connect(polyline, &QDeclarativePolylineMapItem::pathChanged, q, &QGeoMapMapboxGL::onMapItemLinePathChange);
        QObject::connect(polyline, &QDeclarativeGeoMapItemBase::mapItemOpacityChanged, q, &QGeoMapMapboxGL::onMapItemLineOpacityChange);

        QDeclarativeMapLineProperties *lineStyle = polyline->line();
        QObject::connect(lineStyle, &QDeclarativeMapLineProperties::widthChanged, q, &QGeoMapMapboxGL::onMapItemLineWidthChange);
        QObject::connect(lineStyle, &QDeclarativeMapLineProperties::colorChanged, q, &QGeoMapMapboxGL::onMapItemLineColorChange);

        const float opacity = mapItem->mapItemOpacity();
        const float width = lineStyle->width();
        const QColor color = lineStyle->color();

        QMapbox::CoordinatesCollections geometry { { coordinates } };
        QMapbox::LineAnnotation lineAnnotation { { QMapbox::ShapeAnnotationGeometry::Type::LineStringType, geometry }, opacity, width, color };
        QMapbox::Annotation annotation { QVariant::fromValue<QMapbox::LineAnnotation>(lineAnnotation) };

        m_managedMapItems.insert(mapItem, map->addAnnotation(annotation));
    }
    m_mapItemsToAdd.clear();

    for (QDeclarativeGeoMapItemBase *mapItem: m_mapItemsToUpdate) {
        Q_ASSERT(m_managedMapItems.contains(mapItem));
        QMapbox::Coordinates coordinates;
        const QGeoPath *path = static_cast<const QGeoPath *>(&mapItem->geoShape());
        for (const QGeoCoordinate &coordinate : path->path()) {
            coordinates << QMapbox::Coordinate { coordinate.latitude(), coordinate.longitude() };
        }

        QDeclarativePolylineMapItem *polyline = qobject_cast<QDeclarativePolylineMapItem *>(mapItem);
        QDeclarativeMapLineProperties *lineStyle = polyline->line();

        const float opacity = mapItem->mapItemOpacity();
        const float width = lineStyle->width();
        const QColor color = lineStyle->color();

        QMapbox::CoordinatesCollections geometry { { coordinates } };
        QMapbox::LineAnnotation lineAnnotation { { QMapbox::ShapeAnnotationGeometry::Type::LineStringType, geometry }, opacity, width, color };
        QMapbox::Annotation annotation { QVariant::fromValue<QMapbox::LineAnnotation>(lineAnnotation) };

        map->updateAnnotation(m_managedMapItems.value(mapItem), annotation);
    }
    m_mapItemsToUpdate.clear();

    if (m_syncState & CameraDataSync) {
        map->setZoom(zoomLevelFrom256(m_cameraData.zoomLevel() , MBGL_TILE_SIZE));
        map->setBearing(m_cameraData.bearing());
        map->setPitch(m_cameraData.tilt());

        QGeoCoordinate coordinate = m_cameraData.center();
        map->setCoordinate(QMapbox::Coordinate(coordinate.latitude(), coordinate.longitude()));
    }

    if (m_syncState & ViewportSync) {
        mbglNode->resize(m_viewportSize, window->devicePixelRatio());
    }

    mbglNode->render();

    m_syncState = NoSync;

    return mbglNode;
}

QGeoMap::ItemTypes QGeoMapMapboxGLPrivate::supportedMapItemTypes() const
{
    return QGeoMap::MapPolyline;
}

void QGeoMapMapboxGLPrivate::addMapItem(QDeclarativeGeoMapItemBase *item)
{
    Q_Q(QGeoMapMapboxGL);

    switch (item->itemType()) {
    case QGeoMap::NoItem:
    case QGeoMap::MapRectangle:
    case QGeoMap::MapCircle:
    case QGeoMap::MapPolygon:
    case QGeoMap::MapQuickItem:
    case QGeoMap::CustomMapItem:
        return;
    case QGeoMap::MapPolyline:
        m_mapItemsToAdd << item;
        break;
    }

    emit q->sgNodeChanged();
}

void QGeoMapMapboxGLPrivate::removeMapItem(QDeclarativeGeoMapItemBase *item)
{
    Q_Q(QGeoMapMapboxGL);

    switch (item->itemType()) {
    case QGeoMap::NoItem:
    case QGeoMap::MapRectangle:
    case QGeoMap::MapCircle:
    case QGeoMap::MapPolygon:
    case QGeoMap::MapQuickItem:
    case QGeoMap::CustomMapItem:
        return;
    case QGeoMap::MapPolyline:
        m_mapItemsToRemove << item;
        break;
    }

    emit q->sgNodeChanged();
}

void QGeoMapMapboxGLPrivate::changeViewportSize(const QSize &)
{
    Q_Q(QGeoMapMapboxGL);

    m_syncState = m_syncState | ViewportSync;
    emit q->sgNodeChanged();
}

void QGeoMapMapboxGLPrivate::changeCameraData(const QGeoCameraData &)
{
    Q_Q(QGeoMapMapboxGL);

    m_syncState = m_syncState | CameraDataSync;
    emit q->sgNodeChanged();
}

void QGeoMapMapboxGLPrivate::changeActiveMapType(const QGeoMapType)
{
    Q_Q(QGeoMapMapboxGL);

    m_syncState = m_syncState | MapTypeSync;
    emit q->sgNodeChanged();
}

/*
 * QGeoMapMapboxGL implementation
 */

QGeoMapMapboxGL::QGeoMapMapboxGL(QGeoMappingManagerEngineMapboxGL *engine, QObject *parent)
    :   QGeoMap(*new QGeoMapMapboxGLPrivate(engine), parent), m_engine(engine)
{
    Q_D(QGeoMapMapboxGL);

    connect(&d->m_refresh, &QTimer::timeout, this, &QGeoMap::sgNodeChanged);
    d->m_refresh.setInterval(250);
}

QGeoMapMapboxGL::~QGeoMapMapboxGL()
{
}

void QGeoMapMapboxGL::setMapboxGLSettings(const QMapboxGLSettings& settings)
{
    Q_D(QGeoMapMapboxGL);

    d->m_settings = settings;

    // If the access token is not set, use the development access token.
    // This will only affect mapbox:// styles.
    if (d->m_settings.accessToken().isEmpty()) {
        d->m_settings.setAccessToken(developmentToken);
    }
}

QSGNode *QGeoMapMapboxGL::updateSceneGraph(QSGNode *oldNode, QQuickWindow *window)
{
    Q_D(QGeoMapMapboxGL);
    return d->updateSceneGraph(oldNode, window);
}

void QGeoMapMapboxGL::onMapItemLinePathChange()
{
    Q_D(QGeoMapMapboxGL);
    QDeclarativeGeoMapItemBase *mapItem = static_cast<QDeclarativeGeoMapItemBase *>(sender());
    if (mapItem && !d->m_mapItemsToUpdate.contains(mapItem)) {
        d->m_mapItemsToUpdate << mapItem;
    }
}

void QGeoMapMapboxGL::onMapItemLineOpacityChange()
{
    Q_D(QGeoMapMapboxGL);
    QDeclarativeGeoMapItemBase *mapItem = static_cast<QDeclarativeGeoMapItemBase *>(sender());
    if (mapItem && !d->m_mapItemsToUpdate.contains(mapItem)) {
        d->m_mapItemsToUpdate << mapItem;
    }
}

void QGeoMapMapboxGL::onMapItemLineWidthChange(qreal)
{
    Q_D(QGeoMapMapboxGL);
    QDeclarativeGeoMapItemBase *mapItem = static_cast<QDeclarativeGeoMapItemBase *>(sender()->parent());
    if (mapItem && !d->m_mapItemsToUpdate.contains(mapItem)) {
        d->m_mapItemsToUpdate << mapItem;
    }
}

void QGeoMapMapboxGL::onMapItemLineColorChange(const QColor &)
{
    Q_D(QGeoMapMapboxGL);
    QDeclarativeGeoMapItemBase *mapItem = static_cast<QDeclarativeGeoMapItemBase *>(sender()->parent());
    if (mapItem && !d->m_mapItemsToUpdate.contains(mapItem)) {
        d->m_mapItemsToUpdate << mapItem;
    }
}

void QGeoMapMapboxGL::copyrightsChanged(const QString &copyrightsHtml)
{
    Q_D(QGeoMapMapboxGL);

    if (d->m_developmentMode) {
        QString copyrightsHtmlDev = "<a href=\"https://www.mapbox.com/pricing/\">"
            + tr("Development access token, do not use in production!") + "</a> - "
            + copyrightsHtml;
        QGeoMap::copyrightsChanged(copyrightsHtmlDev);
    } else {
        QGeoMap::copyrightsChanged(copyrightsHtml);
    }
}
