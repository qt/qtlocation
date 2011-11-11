/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVE3DGRAPHICSGEOMAP_H
#define QDECLARATIVE3DGRAPHICSGEOMAP_H

#include <QPointer>
#include <QTouchEvent>
#include <QtDeclarative/qquickitem.h>
#include <QtCore/QMutex>

#include <QtCore/QCoreApplication>

#include "qsgtexture.h"
#include "qquickpainteditem.h"
#include "qdeclarativegeomapitemview_p.h"
#include "qdeclarativegeomapflickable_p.h"
#include "qdeclarativegeomappincharea_p.h"
#include "mapcontroller.h"

//#define QT_DECLARATIVE_LOCATION_TRACE 1

#ifdef QT_DECLARATIVE_LOCATION_TRACE
#define QLOC_TRACE0 qDebug() << __FILE__ << __FUNCTION__;
#define QLOC_TRACE1(msg1) qDebug() << __FILE__ << __FUNCTION__ << msg1;
#define QLOC_TRACE2(msg1, msg2) qDebug() << __FILE__ << __FUNCTION__ << msg1 << msg2;
#else
#define QLOC_TRACE0
#define QLOC_TRACE1(msg1)
#define QLOC_TRACE2(msg1, msg2)
#endif

#include "cameradata.h"
#include "map.h"

QT_BEGIN_NAMESPACE

class QGLPainter;
class QGLSceneNode;
class TileCache;
class Tile;
class TileSpec;
class MapSphere;

class QGeoCoordinate;
class QGeoMapObject;
class QGeoMapData;
class QGeoServiceProvider;
class QDeclarativeCoordinate;
class QDeclarativeGeoServiceProvider;
class QDeclarativeGeoMap;
class QDeclarativeGeoMapItem;
class QDeclarativeGeoMapItemBase;

class QDeclarativeGeoMap : public QQuickPaintedItem
{
    Q_OBJECT

//    Q_ENUMS(MapType)
    Q_PROPERTY(QDeclarativeGeoMapPinchArea* pinch READ pinch CONSTANT)
    Q_PROPERTY(QDeclarativeGeoMapFlickable* flick READ flick CONSTANT);
    Q_PROPERTY(QDeclarativeGeoServiceProvider *plugin READ plugin WRITE setPlugin NOTIFY pluginChanged)
    Q_PROPERTY(qreal minimumZoomLevel READ minimumZoomLevel NOTIFY minimumZoomLevelChanged)
    Q_PROPERTY(qreal maximumZoomLevel READ maximumZoomLevel NOTIFY maximumZoomLevelChanged)
    Q_PROPERTY(qreal zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
//    Q_PROPERTY(MapType mapType READ mapType WRITE setMapType NOTIFY mapTypeChanged)
    Q_PROPERTY(QDeclarativeCoordinate* center READ center WRITE setCenter NOTIFY centerChanged)
    // Tilt and bearing are not part of supported API
    Q_PROPERTY(qreal tilt READ tilt WRITE setTilt NOTIFY tiltChanged)
    Q_PROPERTY(qreal bearing READ bearing WRITE setBearing NOTIFY bearingChanged)
    Q_INTERFACES(QDeclarativeParserStatus)

//public:
//    enum MapType {
//        NoMap = QGraphicsGeoMap::NoMap,
//        StreetMap = QGraphicsGeoMap::StreetMap,
//        SatelliteMapDay = QGraphicsGeoMap::SatelliteMapDay,
//        SatelliteMapNight = QGraphicsGeoMap::SatelliteMapNight,
//        TerrainMap = QGraphicsGeoMap::TerrainMap
//    };

public:

    QDeclarativeGeoMap(QQuickItem *parent = 0);
    ~QDeclarativeGeoMap();

    // From QDeclarativeParserStatus
    virtual void componentComplete();

    // from QQuickPaintedItem
    virtual void paint(QPainter *painter);

    // from QQuickItem
    virtual QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
    void itemChange(ItemChange change, const ItemChangeData & data);

    void setPlugin(QDeclarativeGeoServiceProvider *plugin);
    QDeclarativeGeoServiceProvider* plugin() const;

    qreal minimumZoomLevel() const;
    qreal maximumZoomLevel() const;

    QDeclarativeGeoMapFlickable* flick();

    void setZoomLevel(qreal zoomLevel);
    qreal zoomLevel() const;

    void setBearing(qreal bearing);
    qreal bearing() const;

    void setTilt(qreal tilt);
    qreal tilt() const;

    void setCenter(QDeclarativeCoordinate *center);
    QDeclarativeCoordinate* center();

//    void setMapType(MapType mapType);
//    MapType mapType() const;

    QDeclarativeListProperty<QDeclarativeGeoMapItem> items();

    Q_INVOKABLE void removeMapItem(QDeclarativeGeoMapItem *item);
    Q_INVOKABLE void addMapItem(QDeclarativeGeoMapItem *item);
    Q_INVOKABLE void clearMapItems();

    Q_INVOKABLE void removeMapScreenItem(QDeclarativeGeoMapItemBase *item);
    Q_INVOKABLE void addMapScreenItem(QDeclarativeGeoMapItemBase *item);
    Q_INVOKABLE void clearMapScreenItems();

    Q_INVOKABLE QDeclarativeCoordinate* toCoordinate(QPointF screenPosition) const;
    Q_INVOKABLE QPointF toScreenPosition(QDeclarativeCoordinate* coordinate) const;

    // This function is strictly for testing purposes and may be removed at
    // any time without any notice (hence also the obscure naming to avoid
    // accidental usage):
    Q_INVOKABLE int testGetDeclarativeMapItemCount();
    void setActiveMouseArea(QDeclarativeGeoMapMouseArea *area);
    //QDeclarativeGeoMapMouseArea* activeMouseArea() const;

    QDeclarativeGeoMapPinchArea* pinch() {return pinchArea_;}

public Q_SLOTS:
    void pan(int dx, int dy);

protected:
    void touchEvent(QTouchEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *e);
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

Q_SIGNALS:
    void wheel(qreal delta);
    void pluginChanged(QDeclarativeGeoServiceProvider *plugin);
    void zoomLevelChanged(qreal zoomLevel);
    void bearingChanged(qreal bearing);
    void tiltChanged(qreal tilt);
    void centerChanged(const QDeclarativeCoordinate *coordinate);
    void minimumZoomLevelChanged();
    void maximumZoomLevelChanged();
//    void mapTypeChanged(QDeclarativeGeoMap::MapType mapType);

private Q_SLOTS:
    void updateMapDisplay(const QRectF& target);
//    void internalMapTypeChanged(QGraphicsGeoMap::MapType mapType);
    void centerLatitudeChanged(double latitude);
    void centerLongitudeChanged(double longitude);
    void centerAltitudeChanged(double altitude);
    void mapItemDestroyed(QObject* item);
    void mapScreenItemDestroyed(QObject* item);
    void mappingManagerInitialized();
    void mapZoomLevelChanged(qreal zoom);
    void mapTiltChanged(qreal tilt);
    void mapBearingChanged(qreal bearing);
    void mapCenterChanged(AnimatableCoordinate center);

private:
    void setupMapView(QDeclarativeGeoMapItemView *view);
    void updateAspectRatio();
    void populateMap();

    QList<QDeclarativeGeoMapMouseArea*> mouseAreasAt(QPoint pos);
    bool deliverMouseEvent(QMouseEvent* event);
    bool deliverInitialMousePressEvent(QDeclarativeGeoMapMouseArea* ma, QMouseEvent* event);

    QDeclarativeGeoServiceProvider* plugin_;
    QGeoServiceProvider* serviceProvider_;
    QGeoMappingManager* mappingManager_;

    qreal zoomLevel_;
    qreal bearing_;
    qreal tilt_;
    QPointer<QDeclarativeCoordinate> center_;

//    QDeclarativeGeoMap::MapType mapType_;
    bool componentCompleted_;
    bool mappingManagerInitialized_;
    QList<QDeclarativeGeoMapItemView*> mapViews_;

    QDeclarativeGeoMapFlickable* flickable_;
    QDeclarativeGeoMapPinchArea* pinchArea_;
    QList<QDeclarativeGeoMapMouseArea*> mouseAreas_;

    QDeclarativeGeoMapMouseArea* mouseGrabberItem_;
    QPointF lastMousePosition_;

    void paintGL(QGLPainter *painter);
    void earlyDraw(QGLPainter *painter);
    void restoreDefaults(QGLPainter *painter);

    QQuickCanvas* canvas_;
    int touchTimer_;

    TileCache *tileCache_;
    Map *map_;
    QList<QDeclarativeGeoMapItem*> mapItems_;
    QList<QDeclarativeGeoMapItem*> mapItemsPending_;

    QList<QDeclarativeGeoMapItemBase*> mapScreenItems_;

    QMutex updateMutex_;
    friend class QDeclarativeGeoMapItem;
    friend class QDeclarativeGeoMapItemView;
    friend class QDeclarativeGeoMapPinchArea;
    friend class QDeclarativeGeoMapFlickable;
    Q_DISABLE_COPY(QDeclarativeGeoMap);
};


QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeGeoMap));

#endif
