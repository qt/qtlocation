/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MAPSWIDGET_H
#define MAPSWIDGET_H

#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QWidget>
#include <QtWidgets/QGraphicsRectItem>
#include <QtWidgets/QGraphicsSimpleTextItem>

#include "qgraphicsgeomap.h"
#include "qgeomappingmanager.h"
#include "qgeocoordinate.h"

class MapsWidget;
class MarkerManager;
class Marker;
class StatusBarItem;
class ZoomButtonItem;

// The graphics item that actually contains the map
class GeoMap : public QGraphicsGeoMap
{
    Q_OBJECT

    Q_PROPERTY(double centerLatitude READ centerLatitude WRITE setCenterLatitude)
    Q_PROPERTY(double centerLongitude READ centerLongitude WRITE setCenterLongitude)

public:
    explicit GeoMap(QGeoMappingManager *manager, MapsWidget *mapsWidget);
    ~GeoMap();

    double centerLatitude() const;
    void setCenterLatitude(double lat);
    double centerLongitude() const;
    void setCenterLongitude(double lon);

private:
    MapsWidget *mapsWidget;

    bool panActive;
    bool markerPressed;
    QGeoMapObject *pressed;

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void wheelEvent(QGraphicsSceneWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);

signals:
    void clicked(Marker *marker);
    void panned();
};

class FixedGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    FixedGraphicsView(QWidget *parent);

protected:
    void scrollContentsBy(int dx, int dy);
};

// A widget to hold the view and scene for a GeoMap, as well
// as control widgets
class MapsWidgetPrivate;
class MapsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MapsWidget(QWidget *parent = 0);
    ~MapsWidget();

    void setMarkerManager(MarkerManager *markerManager);
    MarkerManager *markerManager() const;

    QGraphicsGeoMap *map() const;
    StatusBarItem *statusBar() const;

    void animatedPanTo(QGeoCoordinate center);
    void setMyLocation(QGeoCoordinate location, bool center=true);

public slots:
    void initialize(QGeoMappingManager *manager);

signals:
    void markerClicked(Marker *marker);
    void mapPanned();

private:
    MapsWidgetPrivate *d;

    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);
};

// An animated status bar item that appears at the bottom
// of the map
class StatusBarItemPrivate;
class StatusBarItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
    Q_PROPERTY(int offset READ offset WRITE setOffset)

public:
    explicit StatusBarItem();
    ~StatusBarItem();

    int offset() const;
    void setRect(qreal x, qreal y, qreal w, qreal h);

public slots:
    void setText(QString text);

    void showText(QString text, quint32 timeout=3000);
    void show();
    void hide();

    void setOffset(int offset);

private:
    StatusBarItemPrivate *d;
};

// Zoom in / zoom out buttons, touch-friendly, appearing on the
// side of the map
class ZoomButtonItemPrivate;
class ZoomButtonItem : public QGraphicsRectItem
{
public:
    explicit ZoomButtonItem(GeoMap *map);

    void setRect(qreal x, qreal y, qreal w, qreal h);

private:
    ZoomButtonItemPrivate *d;

    bool isTopHalf(const QPointF &point);
    bool isBottomHalf(const QPointF &point);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
};

#endif // MAPSWIDGET_H
