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

#ifndef QGEOMAPPINGMANAGER_H
#define QGEOMAPPINGMANAGER_H

#include <QObject>
#include <QSize>
#include <QPair>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Location)

class QLocale;

class QGeoBoundingBox;
class QGeoCoordinate;
class QGeoMappingManagerPrivate;
class QGeoMapRequestOptions;

class QGeoMappingManagerEngine;

class QGeoTiledMapReply;
class TileSpec;

class Q_LOCATION_EXPORT QGeoMappingManager : public QObject
{
    Q_OBJECT

public:
    ~QGeoMappingManager();

    QString managerName() const;
    int managerVersion() const;

    void requestTiles(const QList<TileSpec> &tiles);

    /*
      Possibly replace the MapType enum with some kind of struct / class
        MapLayer {
            enum? type
            QString key;
            QString description;
        }
        where enum includes things like street, satellite (day), satellite (night), traffic
        but _really_ needs to include custom so people can go wild with layers
    */
//    QList<QGraphicsGeoMap::MapType> supportedMapTypes() const;
//    QList<QGraphicsGeoMap::ConnectivityMode> supportedConnectivityModes() const;

    qreal minimumZoomLevel() const;
    qreal maximumZoomLevel() const;

    bool supportsBearing() const;

    bool supportsTilting() const;
    qreal minimumTilt() const;
    qreal maximumTilt() const;

    void setLocale(const QLocale &locale);
    QLocale locale() const;

Q_SIGNALS:
    void tileFinished(const TileSpec &spec, const QByteArray &bytes);
    void tileError(const TileSpec &spec, const QString &errorString);
    void queueFinished();

private:
    QGeoMappingManager(QGeoMappingManagerEngine *engine, QObject *parent = 0);

    QGeoMappingManagerPrivate* d_ptr;
    Q_DISABLE_COPY(QGeoMappingManager)

    friend class QGeoServiceProvider;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
