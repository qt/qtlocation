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
#ifndef MAP_H
#define MAP_H

#include <QObject>

#include "cameradata.h"
#include "maptype.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QGeoCoordinate;

class QGeoMappingManager;

class TileCache;
class MapPrivate;
class MapItem;
class MapController;

class QGLCamera;
class QGLPainter;

class QPointF;

class Q_LOCATION_EXPORT Map : public QObject
{
    Q_OBJECT

    Q_PROPERTY(CameraData camera READ cameraData WRITE setCameraData NOTIFY cameraDataChanged)
    Q_PROPERTY(MapType activeMapType READ activeMapType WRITE setActiveMapType NOTIFY activeMapTypeChanged)

public:
    Map(TileCache *cache, QObject *parent = 0);
    virtual ~Map();

    TileCache* tileCache();

    void setMappingManager(QGeoMappingManager *manager);

    MapController* mapController();

    QGLCamera* glCamera() const;
    void paintGL(QGLPainter *painter);

    void resize(int width, int height);
    int width() const;
    int height() const;

    void setCameraData(const CameraData &cameraData);
    CameraData cameraData() const;

    QGeoCoordinate screenPositionToCoordinate(const QPointF &pos, bool clipToViewport = true) const;
    QPointF coordinateToScreenPosition(const QGeoCoordinate &coordinate, bool clipToViewport = true) const;

    void setActiveMapType(const MapType mapType);
    const MapType activeMapType() const;

public Q_SLOTS:
    void update();

Q_SIGNALS:
    void updateRequired();
    void cameraDataChanged(const CameraData &cameraData);
    void activeMapTypeChanged();

private:
    MapPrivate *d_ptr;
    Q_DECLARE_PRIVATE(Map)

    friend class QGeoMappingManager;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // MAP_H
