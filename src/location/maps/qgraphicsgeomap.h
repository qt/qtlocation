/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGRAPHICSGEOMAP_H
#define QGRAPHICSGEOMAP_H

#include "qmobilityglobal.h"

#include <QGraphicsWidget>

QTM_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoMappingManager;
class QGeoBoundingBox;
class QGeoMapObject;
class QGeoMapOverlay;
class QGraphicsGeoMapPrivate;

class Q_LOCATION_EXPORT QGraphicsGeoMap : public QGraphicsWidget
{
    Q_OBJECT
    Q_ENUMS(MapType)
    Q_ENUMS(ConnectivityMode)

    Q_PROPERTY(qreal minimumZoomLevel READ minimumZoomLevel CONSTANT)
    Q_PROPERTY(qreal maximumZoomLevel READ maximumZoomLevel CONSTANT)
    Q_PROPERTY(qreal zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(MapType mapType READ mapType WRITE setMapType NOTIFY mapTypeChanged)
    Q_PROPERTY(QGeoCoordinate center READ center WRITE setCenter NOTIFY centerChanged)
    Q_PROPERTY(ConnectivityMode connectivityMode READ connectivityMode WRITE setConnectivityMode NOTIFY connectivityModeChanged)
    Q_PROPERTY(qreal minimumTilt READ minimumTilt CONSTANT)
    Q_PROPERTY(qreal maximumTilt READ maximumTilt CONSTANT)
    Q_PROPERTY(qreal tilt READ tilt WRITE setTilt NOTIFY tiltChanged)
    Q_PROPERTY(bool supportsTilting READ supportsTilting CONSTANT)
    Q_PROPERTY(qreal bearing READ bearing WRITE setBearing NOTIFY bearingChanged)
    Q_PROPERTY(bool supportsBearing READ supportsBearing CONSTANT)

public:
    enum MapType {
        NoMap,
        StreetMap,
        SatelliteMapDay,
        SatelliteMapNight,
        TerrainMap
    };

    enum ConnectivityMode {
        NoConnectivity,
        OfflineMode,
        OnlineMode,
        HybridMode
    };

    QGraphicsGeoMap(QGeoMappingManager *manager, QGraphicsItem *parent = 0);
    virtual ~QGraphicsGeoMap();

    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *parent);

    qreal minimumZoomLevel() const;
    qreal maximumZoomLevel() const;

    void setZoomLevel(qreal zoomLevel);
    qreal zoomLevel() const;

    bool supportsBearing() const;
    void setBearing(qreal bearing);
    qreal bearing() const;

    bool supportsTilting() const;
    void setTilt(qreal tilt);
    qreal tilt() const;
    qreal minimumTilt() const;
    qreal maximumTilt() const;

    void setCenter(const QGeoCoordinate &center);
    QGeoCoordinate center() const;

    QList<MapType> supportedMapTypes() const;
    void setMapType(MapType mapType);
    MapType mapType() const;

    QList<ConnectivityMode> supportedConnectivityModes() const;
    void setConnectivityMode(ConnectivityMode connectivityMode);
    ConnectivityMode connectivityMode() const;

    bool supportsCustomMapObjects() const;

    QList<QGeoMapObject*> mapObjects() const;
    void addMapObject(QGeoMapObject *mapObject);
    void removeMapObject(QGeoMapObject *mapObject);
    void clearMapObjects();

    QList<QGeoMapOverlay*> mapOverlays() const;
    void addMapOverlay(QGeoMapOverlay *overlay);
    void removeMapOverlay(QGeoMapOverlay *overlay);
    void clearMapOverlays();

    QGeoBoundingBox viewport() const;
    void fitInViewport(const QGeoBoundingBox &bounds, bool preserveViewportCenter = false);

    QList<QGeoMapObject*> mapObjectsAtScreenPosition(const QPointF &screenPosition) const;
    QList<QGeoMapObject*> mapObjectsInScreenRect(const QRectF &screenRect) const;
    QList<QGeoMapObject*> mapObjectsInViewport() const;

    QPointF coordinateToScreenPosition(const QGeoCoordinate &coordinate) const;
    QGeoCoordinate screenPositionToCoordinate(QPointF screenPosition) const;

public Q_SLOTS:
    void pan(int dx, int dy);

protected:
    void resizeEvent(QGraphicsSceneResizeEvent *event);

Q_SIGNALS:
    void zoomLevelChanged(qreal zoomLevel);
    void centerChanged(const QGeoCoordinate &coordinate);
    void mapTypeChanged(QGraphicsGeoMap::MapType mapType);
    void connectivityModeChanged(QGraphicsGeoMap::ConnectivityMode connectivityMode);
    void bearingChanged(qreal bearing);
    void tiltChanged(qreal tilt);

private Q_SLOTS:
    void updateMapDisplay(const QRectF& target);

private:
    QGraphicsGeoMapPrivate *d_ptr;
    Q_DISABLE_COPY(QGraphicsGeoMap)
};

QTM_END_NAMESPACE

#endif
