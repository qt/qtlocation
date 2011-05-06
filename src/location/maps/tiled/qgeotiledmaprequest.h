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

#ifndef QGEOTILEDMAPREQUEST_H
#define QGEOTILEDMAPREQUEST_H

#include "qgraphicsgeomap.h"

#include <QRect>
#include <QExplicitlySharedDataPointer>

QTM_BEGIN_NAMESPACE

class QGeoTiledMapData;
class QGeoTiledMapRequestPrivate;

class Q_LOCATION_EXPORT QGeoTiledMapRequest
{
public:
    QGeoTiledMapRequest();
    QGeoTiledMapRequest(QGraphicsGeoMap::ConnectivityMode connectivityMode,
                        QGraphicsGeoMap::MapType mapType,
                        qreal zoomLevel,
                        int row,
                        int column,
                        const QRect &tileRect);
    QGeoTiledMapRequest(const QGeoTiledMapRequest &other);
    ~QGeoTiledMapRequest();

    QGeoTiledMapRequest& operator= (const QGeoTiledMapRequest &other);

    bool operator== (const QGeoTiledMapRequest &other) const;

    //QGeoTiledMapData *mapData() const;

    QGraphicsGeoMap::ConnectivityMode connectivityMode() const;
    QGraphicsGeoMap::MapType mapType() const;
    int zoomLevel() const;

    int row() const;
    int column() const;
    QRect tileRect() const;

private:
    QExplicitlySharedDataPointer<QGeoTiledMapRequestPrivate> d_ptr;
};

Q_LOCATION_EXPORT uint qHash(const QGeoTiledMapRequest &key);

QTM_END_NAMESPACE

#endif
