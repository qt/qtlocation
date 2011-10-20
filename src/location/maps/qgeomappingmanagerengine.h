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

#ifndef QGEOMAPPINGMANAGERENGINE_H
#define QGEOMAPPINGMANAGERENGINE_H

#include <QObject>
#include <QSize>
#include <QPair>
#include <QtLocation/qlocationglobal.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE



class QLocale;

class QGeoBoundingBox;
class QGeoCoordinate;
class QGeoMapData;
class QGeoMappingManagerPrivate;
class QGeoMapRequestOptions;

class QGeoMappingManagerEnginePrivate;

class QGeoTiledMapReply;
class TileSpec;

class Q_LOCATION_EXPORT_TEMP QGeoMappingManagerEngine : public QObject
{
    Q_OBJECT

public:
    QGeoMappingManagerEngine(const QMap<QString, QVariant> &parameters, QObject *parent = 0);
    virtual ~QGeoMappingManagerEngine();

    QMap<QString, QVariant> parameters() const;

    QString managerName() const;
    int managerVersion() const;

//    QList<QGraphicsGeoMap::MapType> supportedMapTypes() const;
//    QList<QGraphicsGeoMap::ConnectivityMode> supportedConnectivityModes() const;

    QSize tileSize() const;

    qreal minimumZoomLevel() const;
    qreal maximumZoomLevel() const;

    bool supportsBearing() const;

    bool supportsTilting() const;
    qreal minimumTilt() const;
    qreal maximumTilt() const;

    void setLocale(const QLocale &locale);
    QLocale locale() const;

    virtual void init();
    bool isInitialized() const;

public Q_SLOTS:
    void threadStarted();
    void requestTiles(const QList<TileSpec> &tiles);

private Q_SLOTS:
    void requestNextTile();
    void finished();

Q_SIGNALS:
    void tileFinished(const TileSpec &spec, const QByteArray &bytes);
    void tileError(const TileSpec &spec, const QString &errorString);
    void queueFinished();
    void initialized();

protected:
    QGeoMappingManagerEngine(QGeoMappingManagerEnginePrivate *dd, QObject *parent = 0);

//    void setSupportedMapTypes(const QList<QGraphicsGeoMap::MapType> &mapTypes);
//    void setSupportedConnectivityModes(const QList<QGraphicsGeoMap::ConnectivityMode> &connectivityModes);

    void setTileSize(const QSize &tileSize);

    void setMinimumZoomLevel(qreal minimumZoom);
    void setMaximumZoomLevel(qreal maximumZoom);

    void setMaximumTilt(qreal maximumTilt);
    void setMinimumTilt(qreal minimumTilt);

    void setSupportsBearing(bool supportsBearing);
    void setSupportsTilting(bool supportsTilting);

    QGeoMappingManagerEnginePrivate* d_ptr;

private:
    virtual QGeoTiledMapReply* getTileImage(const TileSpec &spec) = 0;

    void handleReply(QGeoTiledMapReply *reply, const TileSpec &spec);

    void setManagerName(const QString &managerName);
    void setManagerVersion(int managerVersion);

    Q_DECLARE_PRIVATE(QGeoMappingManagerEngine)
    Q_DISABLE_COPY(QGeoMappingManagerEngine)

    friend class QGeoServiceProvider;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
