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

#include "qgeomappingmanagerengine.h"
#include "qgeomappingmanagerengine_p.h"
#include "qgeotiledmapreply.h"
#include "tilespec.h"

#include <QThread>
#include <QNetworkProxy>

QT_BEGIN_NAMESPACE

/*!
    \class QGeoMappingManagerEngine
    \inmodule QtLocation
    \ingroup QtLocation-impl
    \since QtLocation 5.0

    \brief The QGeoMappingManagerEngine class provides an interface and convenience methods
    to implementors of QGeoServiceProvider plugins who want to provides support for displaying
    and interacting with maps.

    Subclasses of QGeoMappingManagerEngine need to provide an implementations
    of createMapData(). The QGeoMapData instances returned by createMapData()
    can be used to contain and manage information concerning what a particular
    QGraphicsGeoMap is viewing.

    Most of the other functions configure the reported capabilities of the engine.
    It is important that these functions are called before createMapData() or any of the
    capability reporting functions are used to prevent incorrect or
    inconsistent behaviour.
*/

/*!
    Constructs a new engine with the specified \a parent, using \a parameters
    to pass any implementation specific data to the engine.
*/
QGeoMappingManagerEngine::QGeoMappingManagerEngine(const QMap<QString, QVariant> &parameters, QObject *parent)
    : QObject(parent),
      d_ptr(new QGeoMappingManagerEnginePrivate())
{
    d_ptr->parameters = parameters;
    d_ptr->initialized = false;
}

/*!
  \internal
*/
QGeoMappingManagerEngine::QGeoMappingManagerEngine(QGeoMappingManagerEnginePrivate *dd, QObject *parent)
    : QObject(parent),
      d_ptr(dd) {}

/*!
    Destroys this engine.
*/
QGeoMappingManagerEngine::~QGeoMappingManagerEngine()
{
    Q_D(QGeoMappingManagerEngine);
    delete d;
}

QMap<QString, QVariant> QGeoMappingManagerEngine::parameters() const
{
    Q_D(const QGeoMappingManagerEngine);
    return d->parameters;
}

/*!
    Initializes the engine. Subclasses of QGeoMappingManagerEngine may
    implement this function to perform (potentially asynchronous) initialization.

    Static/already known data (such as min/max zoom levels) is better to
    initialize already earlier (e.g. in constructor).

    Once subclasses are done with initialization, they should call this baseclass
    implementation of init().
*/
void QGeoMappingManagerEngine::init()
{
    Q_D(QGeoMappingManagerEngine);
    if (d->stopped_)
        return;
    d->initialized = true;
    emit initialized();
}

void QGeoMappingManagerEngine::threadStarted()
{
    Q_D(QGeoMappingManagerEngine);

    if (d->stopped_)
        return;

    init();

    d->timer_ = new QTimer(this);

    d->timer_->setInterval(0);

    connect(d->timer_,
            SIGNAL(timeout()),
            this,
            SLOT(requestNextTile()));

    d->started_ = true;
    if (!d->queue_.isEmpty())
        d->timer_->start();
}

void QGeoMappingManagerEngine::threadFinished()
{
    Q_D(QGeoMappingManagerEngine);
    d->stopped_ = true;
    disconnect(d->timer_);
    d->timer_->stop();
    this->deleteLater();
}

void QGeoMappingManagerEngine::requestTiles(const QList<TileSpec> &tiles)
{
    Q_D(QGeoMappingManagerEngine);

    if (d->stopped_)
        return;

    if (!d->started_) {
        d->queue_ = tiles;
        return;
    }

    for (int i = 0; i < d->queue_.size(); ++i) {
        QGeoTiledMapReply* reply = d->invmap_.value(d->queue_.at(i), 0);
        if (reply) {
            reply->abort();
            d->map_.remove(reply);
            d->invmap_.remove(d->queue_.at(i));
            reply->deleteLater();
        }
    }

    d->queue_ = tiles;

    if (!d->queue_.empty())
        d->timer_->start();
}

void QGeoMappingManagerEngine::requestNextTile()
{
    Q_D(QGeoMappingManagerEngine);

    if (d->stopped_)
        return;

    TileSpec ts = d->queue_.takeFirst();

    QGeoTiledMapReply *reply = getTileImage(ts);

    if (reply->isFinished()) {
        handleReply(reply, ts);
    } else {
        connect(reply,
                SIGNAL(finished()),
                this,
                SLOT(finished()));

        d->map_.insert(reply, ts);
        d->invmap_.insert(ts, reply);
    }

    if (d->queue_.isEmpty())
        d->timer_->stop();
}

void QGeoMappingManagerEngine::finished()
{
    Q_D(QGeoMappingManagerEngine);

    QGeoTiledMapReply *reply = qobject_cast<QGeoTiledMapReply*>(sender());
    if (!reply)
        return;

    if (!d->map_.contains(reply)) {
        reply->deleteLater();
        return;
    }

    TileSpec spec = d->map_.value(reply);

    d->map_.remove(reply);
    d->invmap_.remove(spec);

    handleReply(reply, spec);
}

void QGeoMappingManagerEngine::handleReply(QGeoTiledMapReply *reply, const TileSpec &spec)
{
    Q_D(QGeoMappingManagerEngine);

    if (d->stopped_) {
        reply->deleteLater();
        return;
    }

    if (reply->error() == QGeoTiledMapReply::NoError) {
        QByteArray bytes = reply->mapImageData();
        emit tileFinished(spec, bytes);
    } else {
        emit tileError(spec, reply->errorString());
    }

    if (d->queue_.isEmpty())
        emit queueFinished();

    reply->deleteLater();
}

/*!
    Sets the name which this engine implementation uses to distinguish itself
    from the implementations provided by other plugins to \a managerName.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
*/
void QGeoMappingManagerEngine::setManagerName(const QString &managerName)
{
    d_ptr->managerName = managerName;
}

/*!
    Returns the name which this engine implementation uses to distinguish
    itself from the implementations provided by other plugins.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
*/
QString QGeoMappingManagerEngine::managerName() const
{
    return d_ptr->managerName;
}

/*!
    Sets the version of this engine implementation to \a managerVersion.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
*/
void QGeoMappingManagerEngine::setManagerVersion(int managerVersion)
{
    d_ptr->managerVersion = managerVersion;
}

/*!
    Returns the version of this engine implementation.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
*/
int QGeoMappingManagerEngine::managerVersion() const
{
    return d_ptr->managerVersion;
}

QList<MapType> QGeoMappingManagerEngine::supportedMapTypes() const
{
    Q_D(const QGeoMappingManagerEngine);
    return d->supportedMapTypes;
}

/*!
    Sets the list of map types supported by this engine to \a mapTypes.

    Subclasses of QGeoMappingManagerEngine should use this function to ensure
    that supportedMapTypes() provides accurate information.
*/
void QGeoMappingManagerEngine::setSupportedMapTypes(const QList<MapType> &supportedMapTypes)
{
    Q_D(QGeoMappingManagerEngine);
    d->supportedMapTypes = supportedMapTypes;
}

///*!
//    Returns a list of the connectivity modes supported by this engine.
//*/
//QList<QGraphicsGeoMap::ConnectivityMode> QGeoMappingManagerEngine::supportedConnectivityModes() const
//{
//    Q_D(const QGeoMappingManagerEngine);
//    return d->supportedConnectivityModes;
//}

/*!
    Returns the minimum zoom level supported by this engine.

    Larger values of the zoom level correspond to more detailed views of the
    map.
*/
qreal QGeoMappingManagerEngine::minimumZoomLevel() const
{
    Q_D(const QGeoMappingManagerEngine);
    return d->minimumZoomLevel;
}

/*!
    Returns the maximum zoom level supported by this engine.

    Larger values of the zoom level correspond to more detailed views of the
    map.
*/
qreal QGeoMappingManagerEngine::maximumZoomLevel() const

{
    Q_D(const QGeoMappingManagerEngine);
    return d->maximumZoomLevel;
}

///*!
//    Sets the list of connectivity modes supported by this engine to \a connectivityModes.

//    Subclasses of QGeoMappingManagerEngine should use this function to ensure
//    that supportedConnectivityModes() provides accurate information.

//    If createMapData does not specify a connectivity mode the first mode from
//    \a connectivityModes will be used, or QGraphicsGeoMap::NoConnectivity will
//    be used if \a connectivityModes is empty.
//*/
//void QGeoMappingManagerEngine::setSupportedConnectivityModes(const QList<QGraphicsGeoMap::ConnectivityMode> &connectivityModes)
//{
//    Q_D(QGeoMappingManagerEngine);
//    d->supportedConnectivityModes = connectivityModes;
//}

void QGeoMappingManagerEngine::setTileSize(const QSize &tileSize)
{
    Q_D(QGeoMappingManagerEngine);
    d->tileSize = tileSize;
}

QSize QGeoMappingManagerEngine::tileSize() const
{
    Q_D(const QGeoMappingManagerEngine);
    return d->tileSize;
}


/*!
    Return whether the engine has been initialized and is ready to be used.
*/

bool QGeoMappingManagerEngine::isInitialized() const
{
    Q_D(const QGeoMappingManagerEngine);
    return d->initialized;
}

/*!
    Sets the minimum zoom level supported by this engine to \a minimumZoom.

    Larger values of the zoom level correspond to more detailed views of the
    map.

    Subclasses of QGeoMappingManagerEngine should use this function to ensure
    minimumZoomLevel() provides accurate information.
*/
void QGeoMappingManagerEngine::setMinimumZoomLevel(qreal minimumZoom)
{
    Q_D(QGeoMappingManagerEngine);
    d->minimumZoomLevel = minimumZoom;
}

/*!
    \fn void QGeoMappingManagerEngine::initialized()

    This signal is emitted when the mapping manager has been initialized
    and is ready to be used.

    Subclasses of QGeoMappingManagerEngine should call the
    QGeoMappingManagerEngine init() when they have initialized themselves.
*/


/*!
    Sets the maximum zoom level supported by this engine to \a maximumZoom.

    Larger values of the zoom level correspond to more detailed views of the
    map.

    Subclasses of QGeoMappingManagerEngine should use this function to ensure
    maximumZoomLevel() provides accurate information.
*/
void QGeoMappingManagerEngine::setMaximumZoomLevel(qreal maximumZoom)
{
    Q_D(QGeoMappingManagerEngine);
    d->maximumZoomLevel = maximumZoom;
}

/*!
    Return whether bearing is supported by this engine.
*/
bool QGeoMappingManagerEngine::supportsBearing() const
{
    Q_D(const QGeoMappingManagerEngine);
    return d->supportsBearing;
}

/*!
    Return whether tilting is supported by this engine.
*/
bool QGeoMappingManagerEngine::supportsTilting() const
{
    Q_D(const QGeoMappingManagerEngine);
    return d->supportsTilting;
}

/*!
    Returns the minimum tilt supported by this engine.

    Value in degrees where 0 is equivalent to 90 degrees between view and earth's
    surface i.e. looking straight down to earth.
*/
qreal QGeoMappingManagerEngine::minimumTilt() const
{
    Q_D(const QGeoMappingManagerEngine);
    return d->minimumTilt;
}

/*!
    Returns the maximum tilt supported by this engine.

    Value in degrees where 0 is equivalent to 90 degrees between view and earth's
    surface i.e. looking straight down to earth.
*/
qreal QGeoMappingManagerEngine::maximumTilt() const
{
    Q_D(const QGeoMappingManagerEngine);
    return d->maximumTilt;
}

/*!
    Sets the minimum tilt supported by this engine to \a minimumTilt.

    Value in degrees where 0 is equivalent to 90 degrees between view and earth's
    surface i.e. looking straight down to earth.

    Subclasses of QGeoMappingManagerEngine should use this function to ensure
    minimumTilt() provides accurate information. If no minimum value is set
    by the subclass the value of 0 is used.
*/
void QGeoMappingManagerEngine::setMinimumTilt(qreal minimumTilt)
{
    Q_D(QGeoMappingManagerEngine);
    d->minimumTilt = minimumTilt;
}

/*!
    Sets the maximum tilt supported by this engine to \a maximumTilt.

    Value in degrees where 0 is equivalent to 90 degrees between view and earth's
    surface i.e. looking straight down to earth.

    Subclasses of QGeoMappingManagerEngine should use this function to ensure
    maximumTilt() provides accurate information. If no maximum value is set
    by the subclass the value of 0 is used.
*/
void QGeoMappingManagerEngine::setMaximumTilt(qreal maximumTilt)
{
    Q_D(QGeoMappingManagerEngine);
    d->maximumTilt = maximumTilt;
}

/*!
    Sets whether bearing is supported by this engine to \a supportsBearing.

    Subclasses of QGeoMappingManagerEngine should use this function to ensure
    supportsBearing() provides accurate information. If no value is set
    by the subclass then bearing support is disabled and supportsBearing set
    to false.
*/
void QGeoMappingManagerEngine::setSupportsBearing(bool supportsBearing)
{
    Q_D(QGeoMappingManagerEngine);
    d->supportsBearing = supportsBearing;
}

/*!
    Sets whether tilting is supported by this engine to \a supportsTilting.

    Subclasses of QGeoMappingManagerEngine should use this function to ensure
    supportsTilting() provides accurate information. If no value is set
    by the subclass then tilting support is disabled and supportsTilting set
    to false.
*/
void QGeoMappingManagerEngine::setSupportsTilting(bool supportsTilting)
{
    Q_D(QGeoMappingManagerEngine);
    d->supportsTilting = supportsTilting;
}

/*!
    Sets the locale to be used by the this manager to \a locale.

    If this mapping manager supports returning map labels
    in different languages, they will be returned in the language of \a locale.

    The locale used defaults to the system locale if this is not set.
*/
void QGeoMappingManagerEngine::setLocale(const QLocale &locale)
{
    d_ptr->locale = locale;
}

/*!
    Returns the locale used to hint to this mapping manager about what
    language to use for map labels.
*/
QLocale QGeoMappingManagerEngine::locale() const
{
    return d_ptr->locale;
}

/*******************************************************************************
*******************************************************************************/

QGeoMappingManagerEnginePrivate::QGeoMappingManagerEnginePrivate()
    : managerVersion(-1),
    minimumZoomLevel(-2.0),
    maximumZoomLevel(-2.0), // todo fixme, this needs to come from plugin
    supportsBearing(false),
    supportsTilting(false),
    minimumTilt(0.0),
    maximumTilt(0.0),
    started_(false),
    stopped_(false) {}

QGeoMappingManagerEnginePrivate::~QGeoMappingManagerEnginePrivate() {}

#include "moc_qgeomappingmanagerengine.cpp"

QT_END_NAMESPACE
