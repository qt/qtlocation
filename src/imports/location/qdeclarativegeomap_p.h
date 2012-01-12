/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QtQuick/QQuickItem>
#include <QtCore/QMutex>

#include <QtCore/QCoreApplication>

#include <QtQuick/QSGTexture>
#include <QtQuick/QQuickPaintedItem>
#include <QtDeclarative/QDeclarativeParserStatus>
#include "qdeclarativegeomapitemview_p.h"
#include "qdeclarativegeomapflickable_p.h"
#include "qdeclarativegeomappincharea_p.h"
#include "qgeomapcontroller_p.h"
#include "qgeomappingmanager.h"

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

#include "qgeocameradata_p.h"
#include "qgeomap_p.h"
#include "qdeclarativegeomaptype_p.h"

QT_BEGIN_NAMESPACE

class QGLPainter;
class QGLSceneNode;
class QGeoTileCache;
class Tile;
class QGeoTileSpec;
class QGeoMapSphere;

class QGeoCoordinate;
class QGeoMapObject;
class QGeoMapData;
class QGeoServiceProvider;
class QDeclarativeCoordinate;
class QDeclarativeGeoServiceProvider;
class QDeclarativeGeoMap;
class QDeclarativeGeoMapItem;
class QDeclarativeGeoMapItemBase;
class QDeclarativeGeoMapType;

class QDeclarativeGeoMap : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeGeoMapPinchArea* pinch READ pinch CONSTANT)
    Q_PROPERTY(QDeclarativeGeoMapFlickable* flick READ flick CONSTANT)
    Q_PROPERTY(QDeclarativeGeoServiceProvider *plugin READ plugin WRITE setPlugin NOTIFY pluginChanged)
    Q_PROPERTY(qreal minimumZoomLevel READ minimumZoomLevel NOTIFY minimumZoomLevelChanged)
    Q_PROPERTY(qreal maximumZoomLevel READ maximumZoomLevel NOTIFY maximumZoomLevelChanged)
    Q_PROPERTY(qreal zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(QDeclarativeGeoMapType* activeMapType READ activeMapType WRITE setActiveMapType NOTIFY activeMapTypeChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeGeoMapType> supportedMapTypes READ supportedMapTypes NOTIFY supportedMapTypesChanged)
    Q_PROPERTY(QDeclarativeCoordinate* center READ center WRITE setCenter NOTIFY centerChanged)
    Q_PROPERTY(QList<QObject*> mapItems READ mapItems NOTIFY mapItemsChanged)
    // Tilt and bearing are not part of supported API
    Q_PROPERTY(qreal tilt READ tilt WRITE setTilt NOTIFY tiltChanged)
    Q_PROPERTY(qreal bearing READ bearing WRITE setBearing NOTIFY bearingChanged)
    Q_INTERFACES(QDeclarativeParserStatus)

public:

    QDeclarativeGeoMap(QQuickItem *parent = 0);
    ~QDeclarativeGeoMap();

    // From QDeclarativeParserStatus
    virtual void componentComplete();

    // from QQuickItem
    virtual QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
    virtual void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

    void itemChange(ItemChange change, const ItemChangeData & data);

    void setPlugin(QDeclarativeGeoServiceProvider *plugin);
    QDeclarativeGeoServiceProvider* plugin() const;

    void setActiveMapType(QDeclarativeGeoMapType* mapType);
    QDeclarativeGeoMapType* activeMapType()const;

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

    QDeclarativeListProperty<QDeclarativeGeoMapType> supportedMapTypes();

    Q_INVOKABLE void removeMapItem(QDeclarativeGeoMapItemBase *item);
    Q_INVOKABLE void addMapItem(QDeclarativeGeoMapItemBase *item);
    Q_INVOKABLE void clearMapItems();
    QList<QObject*> mapItems();

    Q_INVOKABLE QDeclarativeCoordinate* toCoordinate(QPointF screenPosition) const;
    Q_INVOKABLE QPointF toScreenPosition(QDeclarativeCoordinate* coordinate) const;

    // callback for map mouse areas
    bool mouseEvent(QMouseEvent* event);

    QDeclarativeGeoMapPinchArea* pinch();

public Q_SLOTS:
    void pan(int dx, int dy);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void touchEvent(QTouchEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *e);

Q_SIGNALS:
    void wheel(qreal delta);
    void pluginChanged(QDeclarativeGeoServiceProvider *plugin);
    void zoomLevelChanged(qreal zoomLevel);
    void bearingChanged(qreal bearing);
    void tiltChanged(qreal tilt);
    void centerChanged(const QDeclarativeCoordinate *coordinate);
    void activeMapTypeChanged();
    void supportedMapTypesChanged();
    void minimumZoomLevelChanged();
    void maximumZoomLevelChanged();
    void mapItemsChanged();

private Q_SLOTS:
    void updateMapDisplay(const QRectF& target);
    void centerLatitudeChanged(double latitude);
    void centerLongitudeChanged(double longitude);
    void centerAltitudeChanged(double altitude);
    void mappingManagerInitialized();
    void mapZoomLevelChanged(qreal zoom);
    void mapTiltChanged(qreal tilt);
    void mapBearingChanged(qreal bearing);
    void mapCenterChanged(AnimatableCoordinate center);
    void pluginReady();

private:
    void setupMapView(QDeclarativeGeoMapItemView *view);
    void updateAspectRatio();
    void populateMap();

    QDeclarativeGeoServiceProvider* plugin_;
    QGeoServiceProvider* serviceProvider_;
    QGeoMappingManager* mappingManager_;

    qreal zoomLevel_;
    qreal bearing_;
    qreal tilt_;
    QPointer<QDeclarativeCoordinate> center_;

    QDeclarativeGeoMapType* activeMapType_;
    QList<QDeclarativeGeoMapType*> supportedMapTypes_;
    bool componentCompleted_;
    bool mappingManagerInitialized_;
    QList<QDeclarativeGeoMapItemView*> mapViews_;

    QDeclarativeGeoMapFlickable* flickable_;
    QDeclarativeGeoMapPinchArea* pinchArea_;

    void paintGL(QGLPainter *painter);
    void earlyDraw(QGLPainter *painter);
    void restoreDefaults(QGLPainter *painter);

    QQuickCanvas* canvas_;
    int touchTimer_;

    QGeoTileCache *tileCache_;
    QGeoMap *map_;

    QList<QObject*> mapItems_;

    QMutex updateMutex_;
    friend class QDeclarativeGeoMapItem;
    friend class QDeclarativeGeoMapItemView;
    friend class QDeclarativeGeoMapPinchArea;
    friend class QDeclarativeGeoMapFlickable;
    Q_DISABLE_COPY(QDeclarativeGeoMap)
};


QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeGeoMap))

#endif
