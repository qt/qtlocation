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

#include "qgeotiledmaprequest.h"
#include "qgeotiledmaprequest_p.h"

#include "qgeotiledmapdata.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoTiledMapRequest
    \brief The QGeoTiledMapRequest class represents a request for a map tile
    from a tile-based mapping service.

    \inmodule QtLocation

    \ingroup maps-impl-tiled

    \since 1.2

    The tile is specified by a map type, a zoom level, a row and a column.

    At a zoom level of z the world is represented as a 2^z by 2^z grid
    of tiles, and the row and column are relative to the grid of tiles
    for the zoom level of the request.
*/

/*!
    Constructs a new tiled map request.
*/
QGeoTiledMapRequest::QGeoTiledMapRequest()
    : d_ptr(new QGeoTiledMapRequestPrivate()) {}

/*!
    Constructs a new tiled map request.

    At a zoom level of z the world is represented as a 2^z by 2^z grid
    of tiles.

    This tiled map request represents a request for the tile at row \a row and
    column \a column at zoom level \a zoomLevel and type
    \a mapType.

    The request will use connectivity mode \a connectivityMode when
    accessing the map data.

    The rectangle that the tile occupies on the map at the maximum zoom level
    is also given as \a tileRect.
*/
QGeoTiledMapRequest::QGeoTiledMapRequest(QGraphicsGeoMap::ConnectivityMode connectivityMode,
        QGraphicsGeoMap::MapType mapType,
        qreal zoomLevel,
        int row,
        int column,
        const QRect &tileRect)
    : d_ptr(new QGeoTiledMapRequestPrivate())
{
    //d_ptr->mapData = mapData;
    d_ptr->row = row;
    d_ptr->column = column;
    d_ptr->tileRect = tileRect;

    d_ptr->zoomLevel = zoomLevel;
    d_ptr->mapType = mapType;
    d_ptr->connectivityMode = connectivityMode;
}

/*!
    Constructs a tiled map request from the contents of \a other.
*/
QGeoTiledMapRequest::QGeoTiledMapRequest(const QGeoTiledMapRequest &other)
    : d_ptr(other.d_ptr) {}

/*!
    Destroys this tiled map request.
*/
QGeoTiledMapRequest::~QGeoTiledMapRequest() {}

/*!
    Assigns \a other to this tiled map request and then returns a reference to
    this tiled map request.
*/
QGeoTiledMapRequest& QGeoTiledMapRequest::operator= (const QGeoTiledMapRequest & other)
{
    d_ptr = other.d_ptr;

    return *this;
}

/*!
    Returns with this tiled map request is equal to \a other.
*/
bool QGeoTiledMapRequest::operator== (const QGeoTiledMapRequest &other) const
{
    return (d_ptr->row == other.d_ptr->row) &&
           (d_ptr->column == other.d_ptr->column) &&
           (d_ptr->zoomLevel == other.d_ptr->zoomLevel) &&
           (d_ptr->mapType == other.d_ptr->mapType) &&
           (d_ptr->connectivityMode == other.d_ptr->connectivityMode);
}

/*
    Returns the QGeoMapData instance associated with this request.
*/
//QGeoTiledMapData* QGeoTiledMapRequest::mapData() const
//{
//    return d_ptr->mapData;
//}

/*!
    Returns the connectivity mode of the tile request.
*/
QGraphicsGeoMap::ConnectivityMode QGeoTiledMapRequest::connectivityMode() const
{
    return d_ptr->connectivityMode;
}

/*!
    Returns the map type of the requested tile.
*/
QGraphicsGeoMap::MapType QGeoTiledMapRequest::mapType() const
{
    return d_ptr->mapType;
}

/*!
    Returns the zoom level of the requested tile.

    The lower and upper bounds of the zoom level are set by
    the QGeoMappingManager that created this request.
*/
int QGeoTiledMapRequest::zoomLevel() const
{
    return d_ptr->zoomLevel;
}

/*!
    Returns the row of the requested tile.

    At a zoom level of z the world is represented as a 2^z by 2^z grid
    of tiles, and so the row will be between 0 and 2^z - 1.
*/
int QGeoTiledMapRequest::row() const
{
    return d_ptr->row;
}

/*!
    Returns the column of the requested tile.

    At a zoom level of z the world is represented as a 2^z by 2^z grid
    of tiles, and so the column will be between 0 and 2^z - 1.
*/
int QGeoTiledMapRequest::column() const
{
    return d_ptr->column;
}

/*!
    Returns the rectangle that the tile covers on the map at the maximum zoon
    level.

    At a zoom level of z the world is represented as a 2^z by 2^z grid of
    tiles. If m is the maximum zoom level and the tiles are t by t pixel
    squares, then the entire world could be viewed as a 2^m * t by 2^m * t
    pixel image.

    The rectangle returned is specified relative to the pixel coordinates of
    the map at the maximum zoom level.
*/
QRect QGeoTiledMapRequest::tileRect() const
{
    return d_ptr->tileRect;
}

/*!
    Returns a hash of the tiled map request \a key.
*/
uint qHash(const QGeoTiledMapRequest &key)
{
    uint result = QT_PREPEND_NAMESPACE(qHash)(key.row() * 13);
    result += QT_PREPEND_NAMESPACE(qHash)(key.column() * 17);
    result += QT_PREPEND_NAMESPACE(qHash)(key.zoomLevel() * 19);
    result += QT_PREPEND_NAMESPACE(qHash)(static_cast<int>(key.mapType()));
    return result;
}

/*******************************************************************************
*******************************************************************************/

QGeoTiledMapRequestPrivate::QGeoTiledMapRequestPrivate()
    : QSharedData(),
    connectivityMode(QGraphicsGeoMap::NoConnectivity),
    mapType(QGraphicsGeoMap::NoMap),
    zoomLevel(0),
    row(0),
    column(0),
    tileRect(QRect()) {}
//mapData(0) {}

QGeoTiledMapRequestPrivate::QGeoTiledMapRequestPrivate(const QGeoTiledMapRequestPrivate &other)
    : QSharedData(other),
//        mapData(other.mapData),
      connectivityMode(other.connectivityMode),
      mapType(other.mapType),
      zoomLevel(other.zoomLevel),
      row(other.row),
      column(other.column),
      tileRect(other.tileRect) {}

QGeoTiledMapRequestPrivate::~QGeoTiledMapRequestPrivate() {}

QGeoTiledMapRequestPrivate& QGeoTiledMapRequestPrivate::operator= (const QGeoTiledMapRequestPrivate & other)
{
//    mapData = other.mapData;
    connectivityMode = other.connectivityMode;
    mapType = other.mapType;
    zoomLevel = other.zoomLevel;
    row = other.row;
    column = other.column;
    tileRect = other.tileRect;

    return *this;
}

QTM_END_NAMESPACE

