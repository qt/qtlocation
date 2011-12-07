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

#include "qgeomappingmanager.h"
#include "qgeomappingmanager_p.h"
#include "qgeomappingmanagerengine.h"
#include "qgeotiledmapreply.h"
#include "tilespec.h"

#include <QTimer>
#include <QNetworkProxy>
#include <QLocale>

QT_BEGIN_NAMESPACE

/*!
    \class QGeoMappingManager
    \inmodule QtLocation
    \ingroup QtLocation-maps
    \since QtLocation 5.0

    \brief The QGeoMappingManager class provides support for displaying
    and interacting with maps.

    A QGeoMappingManager instance can create QGeoMapData instances with
    createMapData(). The QGeoMapData instances can be used to contain and
    manage information concerning what a particular QGraphicsGeoMap is viewing.

    The functions in this class will typically not be used by clients of this
    API, as the most common uses will only need to obtain a QGeoMappingManager
    instance and associate it with a QGraphicsGeoMap instance:
    \code
        QGeoServiceProvider serviceProvider("nokia");
        QGeoMappingManager *manager = serviceProvider.mappingManager();
        QGraphicsGeoMap *geoMap = new QGraphicsGeoMap(manager);
    \endcode

    This could have been simplified by having the plugin return a
    QGraphicsGeoMap instance instead, but this approach allows users to
    subclass QGraphicsGeoMap in order to override the standard event handlers
    and implement custom map behaviours.
*/

/*!
    Constructs a new manager with the specified \a parent and with the
    implementation provided by \a engine.

    This constructor is used internally by QGeoServiceProviderFactory. Regular
    users should acquire instances of QGeoMappingManager with
    QGeoServiceProvider::mappingManager()
*/
QGeoMappingManager::QGeoMappingManager(QGeoMappingManagerEngine *engine, QObject *parent)
    : QObject(parent),
      d_ptr(new QGeoMappingManagerPrivate)
{
    d_ptr->engine = engine;
    if (!d_ptr->engine) {
        qFatal("The mapping manager engine that was set for this mapping manager was NULL.");
    }

    d_ptr->thread= new QThread;

    qRegisterMetaType<TileSpec>();

    connect(d_ptr->engine,
            SIGNAL(tileFinished(TileSpec,QByteArray)),
            this,
            SIGNAL(tileFinished(TileSpec,QByteArray)),
            Qt::QueuedConnection);
    connect(d_ptr->engine,
            SIGNAL(tileError(TileSpec,QString)),
            this,
            SIGNAL(tileError(TileSpec,QString)),
            Qt::QueuedConnection);
    connect(d_ptr->engine,
            SIGNAL(queueFinished()),
            this,
            SIGNAL(queueFinished()),
            Qt::QueuedConnection);

    connect(d_ptr->engine,
            SIGNAL(initialized()),
            this,
            SIGNAL(initialized()));

    d_ptr->engine->moveToThread(d_ptr->thread);

    connect(d_ptr->thread,
            SIGNAL(started()),
            d_ptr->engine,
            SLOT(threadStarted()));

    connect(d_ptr->thread,
            SIGNAL(finished()),
            d_ptr->engine,
            SLOT(threadFinished()));

    connect(d_ptr->engine,
            SIGNAL(destroyed()),
            d_ptr->thread,
            SLOT(deleteLater()));

    QTimer::singleShot(0, d_ptr->thread, SLOT(start()));
}

/*!
    Destroys this mapping manager.
*/
QGeoMappingManager::~QGeoMappingManager()
{
    delete d_ptr;
}

/*!
    \fn void QGeoMappingManager::initialized()

    This signal is emitted when the mapping manager has been initialized
    and is ready to be used.
*/

/*!
    Returns the name of the engine which implements the behaviour of this
    mapping manager.

    The combination of managerName() and managerVersion() should be unique
    amongst the plugin implementations.
*/
QString QGeoMappingManager::managerName() const
{
    return d_ptr->engine->managerName();
}

/*!
    Returns the version of the engine which implements the behaviour of this
    mapping manager.

    The combination of managerName() and managerVersion() should be unique
    amongst the plugin implementations.
*/
int QGeoMappingManager::managerVersion() const
{
    return d_ptr->engine->managerVersion();
}

void QGeoMappingManager::requestTiles(const QList<TileSpec> &tiles)
{
    QMetaObject::invokeMethod(d_ptr->engine, "requestTiles",
                              Qt::QueuedConnection,
                              Q_ARG(QList<TileSpec>, tiles));
}

QList<MapType> QGeoMappingManager::supportedMapTypes() const
{
    return d_ptr->engine->supportedMapTypes();
}

//QGeoTiledMapReply* QGeoMappingManager::getTileImage(const TileSpec &spec)
//{
//    qWarning() << d_ptr->engine->minimumZoomLevel() << d_ptr->engine->maximumZoomLevel();
//    QGeoTiledMapReply* reply = d_ptr->engine->getTileImage(spec);

//    connect(reply, SIGNAL(finished()), this, SLOT(tileFinished()));

//    return reply;
//}

//void QGeoMappingManager::tileFinished()
//{
//    QGeoTiledMapReply *reply = qobject_cast<QGeoTiledMapReply*>(sender());
//    if (!reply)
//        return;

//    emit finished(reply);
//}

///*!
//    Returns a list of the connectivity modes supported by this manager.
//*/
//QList<QGraphicsGeoMap::ConnectivityMode> QGeoMappingManager::supportedConnectivityModes() const
//{
//    return d_ptr->engine->supportedConnectivityModes();
//}

/*!
    Returns the minimum zoom level supported by this manager.

    Larger values of the zoom level correspond to more detailed views of the
    map.
*/
qreal QGeoMappingManager::minimumZoomLevel() const
{
    return d_ptr->engine->minimumZoomLevel();
}

/*!
    Returns the maximum zoom level supported by this manager.

    Larger values of the zoom level correspond to more detailed views of the
    map.
*/
qreal QGeoMappingManager::maximumZoomLevel() const
{
    return d_ptr->engine->maximumZoomLevel();
}

/*!
    Return whether bearing is supported by this manager.
*/
bool QGeoMappingManager::supportsBearing() const
{
    return d_ptr->engine->supportsBearing();
}

/*!
    Return whether the manager has been initialized
    (will be done automatically but may take some time).

*/
bool QGeoMappingManager::isInitialized() const
{
    return d_ptr->engine->isInitialized();
}


/*!
    Return whether tilting is supported by this manager.
*/
bool QGeoMappingManager::supportsTilting() const
{
    return d_ptr->engine->supportsTilting();
}

/*!
    Returns minimum tilt supported by this manager.

    Value in degrees where 0 is equivalent to 90 degrees between view and earth's
    surface i.e. looking straight down to earth.
*/
qreal QGeoMappingManager::minimumTilt() const
{
    return d_ptr->engine->minimumTilt();
}

/*!
    Returns maximum tilt supported by this manager.

    Value in degrees where 0 is equivalent to 90 degrees between view and earth's
    surface i.e. looking straight down to earth.
*/
qreal QGeoMappingManager::maximumTilt() const
{
    return d_ptr->engine->maximumTilt();
}

/*!
    Sets the locale to be used by the this manager to \a locale.

    If this mapping manager supports returning map labels
    in different languages, they will be returned in the language of \a locale.

    The locale used defaults to the system locale if this is not set.
*/
void QGeoMappingManager::setLocale(const QLocale &locale)
{
    d_ptr->engine->setLocale(locale);
}

/*!
    Returns the locale used to hint to this mapping manager about what
    language to use for map labels.
*/
QLocale QGeoMappingManager::locale() const
{
    return d_ptr->engine->locale();
}

/*******************************************************************************
*******************************************************************************/

QGeoMappingManagerPrivate::QGeoMappingManagerPrivate()
    : engine(0) {}

QGeoMappingManagerPrivate::~QGeoMappingManagerPrivate()
{
    thread->quit();
}

#include "moc_qgeomappingmanager.cpp"

QT_END_NAMESPACE
