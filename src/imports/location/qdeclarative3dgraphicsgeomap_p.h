/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QDECLARATIVE3DGRAPHICSGEOMAP_H
#define QDECLARATIVE3DGRAPHICSGEOMAP_H

//#include <qgraphicsgeomap.h>
#include <QPointer>
#include <QTouchEvent>
#include <QtDeclarative/qsgitem.h>
#include <QtQuick1/QDeclarativeItem>
//#include <QGeoMapData>
#include "qdeclarativegeomapobject_p.h"
#include <QtCore/QCoreApplication>

// Check $$pwd/location.pro how to enable these
#ifdef QSGMOUSEAREA_AVAILABLE
#include "qsgmousearea_p.h"
#endif
#include "qsgtexture.h"
#include "qdeclarativegeomapflickable_p.h"
#include "qdeclarativegeomappincharea_p.h"

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
class QDeclarative3DGraphicsGeoMap;
class QDeclarativeGeoMapItem;
#ifdef QSGMOUSEAREA_AVAILABLE
class QSGMouseEvent;
#endif

class QDeclarative3DGraphicsGeoMap : public QSGItem
{
    Q_OBJECT

    Q_ENUMS(MapType)
    Q_ENUMS(ConnectivityMode)
    // Temporary tweak for testing
    Q_PROPERTY(QDeclarativeGeoMapPinchArea* pinch READ pinch CONSTANT)
    Q_PROPERTY(QDeclarativeGeoMapFlickable* flick READ flick CONSTANT);
    Q_PROPERTY(QDeclarativeGeoServiceProvider *plugin READ plugin WRITE setPlugin NOTIFY pluginChanged)
    Q_PROPERTY(QSizeF size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(qreal minimumZoomLevel READ minimumZoomLevel CONSTANT)
    Q_PROPERTY(qreal maximumZoomLevel READ maximumZoomLevel CONSTANT)
    Q_PROPERTY(qreal zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
//    Q_PROPERTY(MapType mapType READ mapType WRITE setMapType NOTIFY mapTypeChanged)
    Q_PROPERTY(QDeclarativeCoordinate* center READ center WRITE setCenter NOTIFY declarativeCenterChanged)
//    Q_PROPERTY(ConnectivityMode connectivityMode READ connectivityMode WRITE setConnectivityMode NOTIFY connectivityModeChanged)
    Q_INTERFACES(QDeclarativeParserStatus)

public:
//    enum MapType {
//        NoMap = QGraphicsGeoMap::NoMap,
//        StreetMap = QGraphicsGeoMap::StreetMap,
//        SatelliteMapDay = QGraphicsGeoMap::SatelliteMapDay,
//        SatelliteMapNight = QGraphicsGeoMap::SatelliteMapNight,
//        TerrainMap = QGraphicsGeoMap::TerrainMap
//    };

//    enum ConnectivityMode {
//        NoConnectivity = QGraphicsGeoMap::NoConnectivity,
//        OfflineMode = QGraphicsGeoMap::OfflineMode,
//        OnlineMode = QGraphicsGeoMap::OnlineMode,
//        HybridMode = QGraphicsGeoMap::HybridMode
//    };

public:

    QDeclarative3DGraphicsGeoMap(QSGItem *parent = 0);
    ~QDeclarative3DGraphicsGeoMap();

    // From QDeclarativeParserStatus
    virtual void componentComplete();

    // from QSGItem
    virtual QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
    void itemChange(ItemChange change, const ItemChangeData & data);

    void setPlugin(QDeclarativeGeoServiceProvider *plugin);
    QDeclarativeGeoServiceProvider* plugin() const;

    qreal minimumZoomLevel() const;
    qreal maximumZoomLevel() const;

    void setSize(const QSizeF &size);
    QSizeF size() const;

    QDeclarativeGeoMapFlickable* flick();

    void setMouseEnabled(bool enabled);
    bool mouseEnabled() const;
    void setMouseHoverEnabled(bool enabled);
    bool mouseHoverEnabled() const;
    bool containsMouse() const;

    void setZoomLevel(qreal zoomLevel);
    qreal zoomLevel() const;

    void setCenter(QDeclarativeCoordinate *center);
    QDeclarativeCoordinate* center();

//    void setMapType(MapType mapType);
//    MapType mapType() const;

//    void setConnectivityMode(ConnectivityMode connectivityMode);
//    ConnectivityMode connectivityMode() const;

    QDeclarativeListProperty<QDeclarativeGeoMapObject> objects();

    //Q_INVOKABLE void addMapObject(QDeclarativeGeoMapObject* object);
    //Q_INVOKABLE void removeMapObject(QDeclarativeGeoMapObject* object);

    Q_INVOKABLE void removeMapItem(QDeclarativeGeoMapItem *item);
    Q_INVOKABLE void addMapItem(QDeclarativeGeoMapItem *item);
    Q_INVOKABLE void clearMapItems();

    Q_INVOKABLE QDeclarativeCoordinate* toCoordinate(QPointF screenPosition) const;
    Q_INVOKABLE QPointF toScreenPosition(QDeclarativeCoordinate* coordinate) const;

    // This function is strictly for testing purposes and may be removed at
    // any time without any notice (hence also the obscure naming to avoid
    // accidental usage):
    Q_INVOKABLE int testGetDeclarativeMapObjectCount();
    //void setActiveMouseArea(QDeclarativeGeoMapMouseArea *area);
    //QDeclarativeGeoMapMouseArea* activeMouseArea() const;

    QDeclarativeGeoMapPinchArea* pinch() {return pinchArea_;}

    // Internal
    Map* map() {return map_;}

public Q_SLOTS:
    void pan(int dx, int dy);

protected:
    void touchEvent(QTouchEvent *event);
    void wheelEvent(QWheelEvent *event);
    void geometryChanged(const QRectF &newGeometry,
                         const QRectF &oldGeometry);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void keyPressEvent(QKeyEvent *e);
    void closeEvent(QCloseEvent *e);
    //void enterEvent(QEvent *e);
    //void leaveEvent(QEvent *e);
    void showEvent(QShowEvent *e);
    void resizeEvent(QResizeEvent *e);

Q_SIGNALS:
    // wheel
    void wheel(qreal delta);

    void pluginChanged(QDeclarativeGeoServiceProvider *plugin);
    void sizeChanged(const QSizeF &size);
    void zoomLevelChanged(qreal zoomLevel);
    void declarativeCenterChanged(const QDeclarativeCoordinate *coordinate);
//    void mapTypeChanged(QDeclarative3DGraphicsGeoMap::MapType mapType);
//    void connectivityModeChanged(QDeclarative3DGraphicsGeoMap::ConnectivityMode connectivityMode);

private Q_SLOTS:
    void updateMapDisplay(const QRectF& target);
    void internalCenterChanged(const QGeoCoordinate &coordinate);
//    void internalMapTypeChanged(QGraphicsGeoMap::MapType mapType);
//    void internalConnectivityModeChanged(QGraphicsGeoMap::ConnectivityMode connectivityMode);
    void centerLatitudeChanged(double latitude);
    void centerLongitudeChanged(double longitude);
    void centerAltitudeChanged(double altitude);
    void sceneGraphInitialized();
    void beforeRendering();
    void mouseChanged();
    void mapItemTextureChanged();
    void cameraZoomLevelChanged(double zoomLevel);

private:
    void setupMapView(QDeclarativeGeoMapObjectView *view);
    void updateAspectRatio();
    void populateMap();

    QDeclarativeGeoMapObject* createItem(int modelIndex);

    QDeclarativeGeoServiceProvider* plugin_;
    QGeoServiceProvider* serviceProvider_;
    QGeoMappingManager* mappingManager_;
//    QGeoMapData* mapData_;

    qreal zoomLevel_;
    QPointer<QDeclarativeCoordinate> center_;

    QGeoCoordinate* initialCoordinate;
//    QDeclarative3DGraphicsGeoMap::MapType mapType_;
//    QDeclarative3DGraphicsGeoMap::ConnectivityMode connectivityMode_;
    QSizeF size_;

    QList<QDeclarativeGeoMapObject*> mapObjects_;
    QHash<QGeoMapObject*, QDeclarativeGeoMapObject*> objectMap_;

    bool componentCompleted_;
    QList<QDeclarativeGeoMapObjectView*> mapViews_;

#ifdef QSGMOUSEAREA_AVAILABLE
    QSGMouseArea* mouseArea_;
#endif
    QDeclarativeGeoMapFlickable* flickable_;
    QDeclarativeGeoMapPinchArea* pinchArea_;

    void paintGL(QGLPainter *painter);
    void earlyDraw(QGLPainter *painter);
    void restoreDefaults(QGLPainter *painter);

    QSGCanvas* canvas_;
    int touchTimer_;

    TileCache *tileCache_;
    void setCameraData(const CameraData &camera);
    CameraData cameraData() const;
    Map *map_;
    QList<QDeclarativeGeoMapItem*> mapItems_;
    QList<QDeclarativeGeoMapItem*> mapItemsPending_;

    friend class QDeclarativeGeoMapObjectView;
    friend class QDeclarativeGeoMapPinchArea;
    friend class QDeclarativeGeoMapFlickable;
    Q_DISABLE_COPY(QDeclarative3DGraphicsGeoMap);
};


QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarative3DGraphicsGeoMap));

#endif
