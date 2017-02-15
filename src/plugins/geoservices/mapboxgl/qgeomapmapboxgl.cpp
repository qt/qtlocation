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
#include <QtLocation/private/qdeclarativerectanglemapitem_p.h>
#include <QtLocation/private/qdeclarativecirclemapitem_p.h>
#include <QtLocation/private/qdeclarativepolygonmapitem_p.h>
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

QString getId(QDeclarativeGeoMapItemBase *mapItem) {
    return QString::number(quint64(mapItem));
}

QMapbox::Feature fromMapRectangle(QDeclarativeRectangleMapItem *mapItem)
{
    const QGeoRectangle *rect = static_cast<const QGeoRectangle *>(&mapItem->geoShape());
    QMapbox::Coordinate bottomLeft { rect->bottomLeft().latitude(), rect->bottomLeft().longitude() };
    QMapbox::Coordinate bottomRight { rect->bottomRight().latitude(), rect->bottomRight().longitude() };
    QMapbox::Coordinate topLeft { rect->topLeft().latitude(), rect->topLeft().longitude() };
    QMapbox::Coordinate topRight { rect->topRight().latitude(), rect->topRight().longitude() };
    QMapbox::CoordinatesCollections geometry { { { bottomLeft, bottomRight, topRight, topLeft, bottomLeft } } };

    return QMapbox::Feature(QMapbox::Feature::PolygonType, geometry, {}, getId(mapItem));
}

QMapbox::Feature fromMapPolygon(QDeclarativePolygonMapItem *mapItem)
{
    const QGeoPath *path = static_cast<const QGeoPath *>(&mapItem->geoShape());
    QMapbox::Coordinates coordinates;
    for (const QGeoCoordinate &coordinate : path->path()) {
        coordinates << QMapbox::Coordinate { coordinate.latitude(), coordinate.longitude() };
    }
    coordinates.append(coordinates.first());
    QMapbox::CoordinatesCollections geometry { { coordinates } };

    return QMapbox::Feature(QMapbox::Feature::PolygonType, geometry, {}, getId(mapItem));
}

QMapbox::Feature fromMapPolyline(QDeclarativePolylineMapItem *mapItem)
{
    const QGeoPath *path = static_cast<const QGeoPath *>(&mapItem->geoShape());
    QMapbox::Coordinates coordinates;
    for (const QGeoCoordinate &coordinate : path->path()) {
        coordinates << QMapbox::Coordinate { coordinate.latitude(), coordinate.longitude() };
    }
    QMapbox::CoordinatesCollections geometry { { coordinates } };

    return QMapbox::Feature(QMapbox::Feature::LineStringType, geometry, {}, getId(mapItem));
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
    bool m_styleLoaded = false;

    QList<QDeclarativeGeoMapItemBase *> m_managedMapItems;
    QList<QDeclarativeGeoMapItemBase *> m_mapItemsToAdd;
    QList<QDeclarativeGeoMapItemBase *> m_mapItemsToRemove;

    QHash<QString, QPair<QString, QVariant>> m_paintProperties;
    QHash<QString, QPair<QString, QVariant>> m_layoutProperties;
    QList<QMapbox::Feature> m_sources;

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
        QObject::connect(mbglNode->map(), &QMapboxGL::mapChanged, q, &QGeoMapMapboxGL::onMapChanged);
        m_syncState = MapTypeSync | CameraDataSync | ViewportSync;
    }

    QMapboxGL *map = mbglNode->map();

    if (m_syncState & MapTypeSync) {
        m_developmentMode = m_activeMapType.name().startsWith("mapbox://")
            && m_settings.accessToken() == developmentToken;

        map->setStyleUrl(m_activeMapType.name());
    }

    if (m_styleLoaded) {
        for (QDeclarativeGeoMapItemBase *mapItem : m_mapItemsToRemove) {
            const QString id = getId(mapItem);
            if (map->layerExists(id)) {
                map->removeLayer(id);
            }
            if (map->sourceExists(id)) {
                map->removeSource(id);
            }
            m_mapItemsToAdd.removeAll(mapItem);
        }
        m_mapItemsToRemove.clear();

        auto addSourceAndLayer = [&](const QMapbox::Feature &feature) {
            const QString id = feature.id.toString();
            Q_ASSERT(!map->sourceExists(id));
            Q_ASSERT(!map->layerExists(id));

            QVariantMap source;
            source["type"] = QStringLiteral("geojson");
            source["data"] = QVariant::fromValue<QMapbox::Feature>(feature);
            map->addSource(id, source);

            QVariantMap layer;
            layer["id"] = feature.id;
            switch (feature.type) {
            case QMapbox::Feature::PointType:
                layer["type"] = QStringLiteral("circle");
                break;
            case QMapbox::Feature::LineStringType:
                layer["type"] = QStringLiteral("line");
                break;
            case QMapbox::Feature::PolygonType:
                layer["type"] = QStringLiteral("fill");
                break;
            }
            layer["source"] = feature.id;
            map->addLayer(layer);
        };

        for (QDeclarativeGeoMapItemBase *item : m_mapItemsToAdd) {
            const QString id = getId(item);

            switch (item->itemType()) {
            case QGeoMap::MapRectangle: {
                QDeclarativeRectangleMapItem *mapItem = static_cast<QDeclarativeRectangleMapItem *>(item);
                addSourceAndLayer(fromMapRectangle(mapItem));
                map->setPaintProperty(id, "fill-opacity", mapItem->mapItemOpacity());
                map->setPaintProperty(id, "fill-color", mapItem->color());
                map->setPaintProperty(id, "fill-outline-color", mapItem->border()->color());
            } break;
            case QGeoMap::MapPolygon: {
                QDeclarativePolygonMapItem *mapItem = static_cast<QDeclarativePolygonMapItem *>(item);
                addSourceAndLayer(fromMapPolygon(mapItem));
                map->setPaintProperty(id, "fill-opacity", mapItem->mapItemOpacity());
                map->setPaintProperty(id, "fill-color", mapItem->color());
                map->setPaintProperty(id, "fill-outline-color", mapItem->border()->color());
            } break;
            case QGeoMap::MapPolyline: {
                QDeclarativePolylineMapItem *mapItem = static_cast<QDeclarativePolylineMapItem *>(item);
                addSourceAndLayer(fromMapPolyline(mapItem));
                map->setPaintProperty(id, "line-opacity", mapItem->mapItemOpacity());
                map->setPaintProperty(id, "line-color", mapItem->line()->color());
                map->setPaintProperty(id, "line-width", mapItem->line()->width());
            } break;
            default:
                qWarning() << "Unsupported QGeoMap item type: " << item->itemType();
                break;
            }
        }
        m_mapItemsToAdd.clear();

        for (const QMapbox::Feature &feature : m_sources) {
            const QString id = feature.id.toString();

            Q_ASSERT(map->sourceExists(id));

            QVariantMap source;
            source["type"] = QStringLiteral("geojson");
            source["data"] = QVariant::fromValue<QMapbox::Feature>(feature);
            map->updateSource(id, source);
        }
        m_sources.clear();

        auto paintPropertiesIterator = m_paintProperties.constBegin();
        while (paintPropertiesIterator != m_paintProperties.constEnd()) {
            map->setPaintProperty(paintPropertiesIterator.key(), paintPropertiesIterator.value().first, paintPropertiesIterator.value().second);
        }
        m_paintProperties.clear();

        auto layoutPropertiesIterator = m_layoutProperties.constBegin();
        while (layoutPropertiesIterator != m_layoutProperties.constEnd()) {
            map->setLayoutProperty(layoutPropertiesIterator.key(), layoutPropertiesIterator.value().first, layoutPropertiesIterator.value().second);
        }
        m_layoutProperties.clear();
    }

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
    // TODO https://bugreports.qt.io/browse/QTBUG-58869
    return QGeoMap::MapRectangle | QGeoMap::MapPolygon | QGeoMap::MapPolyline;
}

void QGeoMapMapboxGLPrivate::addMapItem(QDeclarativeGeoMapItemBase *item)
{
    Q_Q(QGeoMapMapboxGL);

    switch (item->itemType()) {
    case QGeoMap::NoItem:
    case QGeoMap::MapQuickItem:
    case QGeoMap::CustomMapItem:
    case QGeoMap::MapCircle:
        return;
    case QGeoMap::MapRectangle: {
        QDeclarativeRectangleMapItem *mapItem = qobject_cast<QDeclarativeRectangleMapItem *>(item);
        QObject::connect(mapItem, &QDeclarativeRectangleMapItem::bottomRightChanged, q, &QGeoMapMapboxGL::onMapRectangleGeometryChanged);
        QObject::connect(mapItem, &QDeclarativeRectangleMapItem::topLeftChanged, q, &QGeoMapMapboxGL::onMapRectangleGeometryChanged);
        QObject::connect(mapItem, &QDeclarativeRectangleMapItem::colorChanged, q, &QGeoMapMapboxGL::onMapRectangleColorChanged);
        QObject::connect(mapItem->border(), &QDeclarativeMapLineProperties::colorChanged, q, &QGeoMapMapboxGL::onMapRectangleBorderColorChanged);
        QObject::connect(mapItem->border(), &QDeclarativeMapLineProperties::widthChanged, q, &QGeoMapMapboxGL::onMapRectangleBorderWidthChanged);
    } break;
    case QGeoMap::MapPolygon: {
        QDeclarativePolygonMapItem *mapItem = qobject_cast<QDeclarativePolygonMapItem *>(item);
        QObject::connect(mapItem, &QDeclarativePolygonMapItem::pathChanged, q, &QGeoMapMapboxGL::onMapPolygonGeometryChanged);
        QObject::connect(mapItem, &QDeclarativePolygonMapItem::colorChanged, q, &QGeoMapMapboxGL::onMapPolygonColorChanged);
        QObject::connect(mapItem->border(), &QDeclarativeMapLineProperties::colorChanged, q, &QGeoMapMapboxGL::onMapPolygonBorderColorChanged);
        QObject::connect(mapItem->border(), &QDeclarativeMapLineProperties::widthChanged, q, &QGeoMapMapboxGL::onMapPolygonBorderWidthChanged);
    } break;
    case QGeoMap::MapPolyline: {
        QDeclarativePolylineMapItem *mapItem = qobject_cast<QDeclarativePolylineMapItem *>(item);
        QObject::connect(mapItem, &QDeclarativePolylineMapItem::pathChanged, q, &QGeoMapMapboxGL::onMapPolylineGeometryChanged);
        QObject::connect(mapItem->line(), &QDeclarativeMapLineProperties::colorChanged, q, &QGeoMapMapboxGL::onMapPolylineLineColorChanged);
        QObject::connect(mapItem->line(), &QDeclarativeMapLineProperties::widthChanged, q, &QGeoMapMapboxGL::onMapPolylineLineWidthChanged);
    } break;
    }

    QObject::connect(item, &QDeclarativeGeoMapItemBase::mapItemOpacityChanged, q, &QGeoMapMapboxGL::onMapItemOpacityChanged);

    m_mapItemsToAdd.append(item);
    m_managedMapItems.append(item);

    emit q->sgNodeChanged();
}

void QGeoMapMapboxGLPrivate::removeMapItem(QDeclarativeGeoMapItemBase *item)
{
    Q_Q(QGeoMapMapboxGL);

    switch (item->itemType()) {
    case QGeoMap::NoItem:
    case QGeoMap::MapQuickItem:
    case QGeoMap::CustomMapItem:
    case QGeoMap::MapCircle:
        return;
    case QGeoMap::MapRectangle:
        q->disconnect(static_cast<QDeclarativeRectangleMapItem *>(item)->border());
        break;
    case QGeoMap::MapPolygon:
        q->disconnect(static_cast<QDeclarativePolygonMapItem *>(item)->border());
        break;
    case QGeoMap::MapPolyline:
        q->disconnect(static_cast<QDeclarativePolylineMapItem *>(item)->line());
        break;
        break;
    }

    q->disconnect(item);

    m_mapItemsToRemove.append(item);
    m_managedMapItems.removeAll(item);

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

QString QGeoMapMapboxGL::copyrightsStyleSheet() const
{
    return QStringLiteral("* { vertical-align: middle; font-weight: normal }");
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

void QGeoMapMapboxGL::onMapChanged(QMapboxGL::MapChange change)
{
    Q_D(QGeoMapMapboxGL);

    if (change == QMapboxGL::MapChangeDidFinishLoadingStyle || change == QMapboxGL::MapChangeDidFailLoadingMap) {
        d->m_styleLoaded = true;
    } else if (change == QMapboxGL::MapChangeWillStartLoadingMap) {
        d->m_styleLoaded = false;
        d->m_mapItemsToAdd.clear();
        d->m_mapItemsToAdd.append(d->m_managedMapItems);
    }
}

// QDeclarativeGeoMapItemBase

void QGeoMapMapboxGL::onMapItemOpacityChanged()
{
    Q_D(QGeoMapMapboxGL);

    QDeclarativeGeoMapItemBase *mapItem = static_cast<QDeclarativeGeoMapItemBase *>(sender());
    const QString id = getId(mapItem);

    switch (mapItem->itemType()) {
    case QGeoMap::MapRectangle:
    case QGeoMap::MapPolygon:
        d->m_paintProperties.insertMulti(id, { QStringLiteral("fill-opacity"), mapItem->mapItemOpacity() });
        break;
    case QGeoMap::MapPolyline:
        d->m_paintProperties.insertMulti(id, { QStringLiteral("line-opacity"), mapItem->mapItemOpacity() });
        break;
    default:
        qWarning() << "Unsupported QGeoMap item type: " << mapItem->itemType();
        return;
    }

    emit sgNodeChanged();
}

// QDeclarativeRectangleMapItem

void QGeoMapMapboxGL::onMapRectangleGeometryChanged()
{
    Q_D(QGeoMapMapboxGL);

    d->m_sources.append(fromMapRectangle(qobject_cast<QDeclarativeRectangleMapItem *>(sender())));

    emit sgNodeChanged();
}

void QGeoMapMapboxGL::onMapRectangleColorChanged()
{
    Q_D(QGeoMapMapboxGL);

    QDeclarativeRectangleMapItem *mapItem = qobject_cast<QDeclarativeRectangleMapItem *>(sender());
    d->m_paintProperties.insertMulti(getId(mapItem), { QStringLiteral("fill-color"), mapItem->color() });

    emit sgNodeChanged();
}

void QGeoMapMapboxGL::onMapRectangleBorderColorChanged()
{
    Q_D(QGeoMapMapboxGL);

    QDeclarativeRectangleMapItem *mapItem = qobject_cast<QDeclarativeRectangleMapItem *>(sender()->parent());
    d->m_paintProperties.insertMulti(getId(mapItem), { QStringLiteral("fill-outline-color"), mapItem->border()->color() });

    emit sgNodeChanged();
}

void QGeoMapMapboxGL::onMapRectangleBorderWidthChanged()
{
    // TODO https://bugreports.qt.io/browse/QTBUG-58872
    qWarning() << "MapRectangle border color currently not supported in Mapbox GL native.";
}

// QDeclarativePolygonMapItem

void QGeoMapMapboxGL::onMapPolygonGeometryChanged()
{
    Q_D(QGeoMapMapboxGL);

    d->m_sources.append(fromMapPolygon(qobject_cast<QDeclarativePolygonMapItem *>(sender())));

    emit sgNodeChanged();
}

void QGeoMapMapboxGL::onMapPolygonColorChanged()
{
    Q_D(QGeoMapMapboxGL);

    QDeclarativePolygonMapItem *mapItem = qobject_cast<QDeclarativePolygonMapItem *>(sender());
    d->m_paintProperties.insertMulti(getId(mapItem), { QStringLiteral("fill-color"), mapItem->color() });

    emit sgNodeChanged();
}

void QGeoMapMapboxGL::onMapPolygonBorderColorChanged()
{
    Q_D(QGeoMapMapboxGL);

    QDeclarativePolygonMapItem *mapItem = qobject_cast<QDeclarativePolygonMapItem *>(sender()->parent());
    d->m_paintProperties.insertMulti(getId(mapItem), { QStringLiteral("fill-outline-color"), mapItem->border()->color() });

    emit sgNodeChanged();
}

void QGeoMapMapboxGL::onMapPolygonBorderWidthChanged()
{
    // TODO https://bugreports.qt.io/browse/QTBUG-58872
    qWarning() << "MapPolygon border color currently not supported in Mapbox GL native.";
}

// QDeclarativePolylineMapItem

void QGeoMapMapboxGL::onMapPolylineGeometryChanged()
{
    Q_D(QGeoMapMapboxGL);

    d->m_sources.append(fromMapPolyline(qobject_cast<QDeclarativePolylineMapItem *>(sender())));

    emit sgNodeChanged();
}

void QGeoMapMapboxGL::onMapPolylineLineColorChanged()
{
    Q_D(QGeoMapMapboxGL);

    QDeclarativePolylineMapItem *mapItem = qobject_cast<QDeclarativePolylineMapItem *>(sender()->parent());
    d->m_paintProperties.insertMulti(getId(mapItem), { QStringLiteral("line-color"), mapItem->line()->color() });

    emit sgNodeChanged();
}

void QGeoMapMapboxGL::onMapPolylineLineWidthChanged()
{
    Q_D(QGeoMapMapboxGL);

    QDeclarativePolylineMapItem *mapItem = qobject_cast<QDeclarativePolylineMapItem *>(sender()->parent());
    d->m_paintProperties.insertMulti(getId(mapItem), { QStringLiteral("line-width"), mapItem->line()->width() });

    emit sgNodeChanged();
}

void QGeoMapMapboxGL::copyrightsChanged(const QString &copyrightsHtml)
{
    Q_D(QGeoMapMapboxGL);

    QString copyrightsHtmlFinal = copyrightsHtml;

    if (d->m_developmentMode) {
        copyrightsHtmlFinal.prepend("<a href='https://www.mapbox.com/pricing'>"
            + tr("Development access token, do not use in production!") + "</a> - ");
    }

    if (d->m_activeMapType.name().startsWith("mapbox://")) {
        copyrightsHtmlFinal = "<table><tr><th><img src='qrc:/mapboxgl/logo.png'/></th><th>"
            + copyrightsHtmlFinal + "</th></tr></table>";
    }

    QGeoMap::copyrightsChanged(copyrightsHtmlFinal);
}
