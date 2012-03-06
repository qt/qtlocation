/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeomappingmanager.h"
#include "qgeomappingmanager_p.h"
#include "qgeomappingmanagerengine.h"
#include "qgeotiledmapreply.h"


#include "qgeomap_p.h"
#include "qgeomap_p_p.h"
#include "qgeocameracapabilities_p.h"
#include "qgeotilecache_p.h"
#include "qgeotilespec.h"

#include <QTimer>
#include <QNetworkProxy>
#include <QLocale>

QT_BEGIN_NAMESPACE

#define Q_SHARED_D(type) \
    QSharedPointer< type > d = d_ptr;

/*!
    \class QGeoMappingManager
    \inmodule QtLocation
    \ingroup QtLocation-maps
    \since QtLocation 5.0

    \brief The QGeoMappingManager class provides support for displaying
    and interacting with maps.
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

    qRegisterMetaType<QGeoTileSpec>();

    connect(d_ptr->engine,
            SIGNAL(tileFinished(QGeoTileSpec,QByteArray,QString)),
            this,
            SLOT(engineTileFinished(QGeoTileSpec,QByteArray,QString)),
            Qt::QueuedConnection);
    connect(d_ptr->engine,
            SIGNAL(tileError(QGeoTileSpec,QString)),
            this,
            SLOT(engineTileError(QGeoTileSpec,QString)),
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
    Q_SHARED_D(QGeoMappingManagerPrivate);
    if (!d) return QString();
    return d->engine->managerName();
}

/*!
    Returns the version of the engine which implements the behaviour of this
    mapping manager.

    The combination of managerName() and managerVersion() should be unique
    amongst the plugin implementations.
*/
int QGeoMappingManager::managerVersion() const
{
    Q_SHARED_D(QGeoMappingManagerPrivate);
    if (!d) return 0;
    return d_ptr->engine->managerVersion();
}

void QGeoMappingManager::registerMap(QGeoMap *map)
{
    Q_SHARED_D(QGeoMappingManagerPrivate);
    if (!d) return;

    QGeoTileCache *cache = map->tileCache();
    QSet<QGeoMap*> maps = d->caches.value(cache);
    maps.insert(map);
    d->caches.insert(cache, maps);
}

void QGeoMappingManager::deregisterMap(QGeoMap *map)
{
    Q_SHARED_D(QGeoMappingManagerPrivate);
    if (!d) return;

    QGeoTileCache *cache = map->tileCache();
    QSet<QGeoMap*> maps = d->caches.value(cache);
    maps.remove(map);
    if (maps.isEmpty()) {
        d->caches.remove(cache);
    } else {
        d->caches.insert(cache, maps);
    }

    // clear any tileHash / mapHash entries
    d->mapHash.remove(map);

    QHash<QGeoTileSpec, QSet<QGeoMap*> > newTileHash = d->tileHash;
    typedef QHash<QGeoTileSpec, QSet<QGeoMap*> >::const_iterator h_iter;
    h_iter hi = d->tileHash.constBegin();
    h_iter hend = d->tileHash.constEnd();
    for (; hi != hend; ++hi) {
        QSet<QGeoMap*> maps = hi.value();
        if (maps.contains(map)) {
            maps.remove(map);
            if (maps.isEmpty())
                newTileHash.remove(hi.key());
            else
                newTileHash.insert(hi.key(), maps);
        }
    }
    d->tileHash = newTileHash;
}

void QGeoMappingManager::updateTileRequests(QGeoMap *map,
                                            const QSet<QGeoTileSpec> &tilesAdded,
                                            const QSet<QGeoTileSpec> &tilesRemoved)
{
    Q_SHARED_D(QGeoMappingManagerPrivate);
    if (!d) return;

    typedef QSet<QGeoTileSpec>::const_iterator tile_iter;

    // add and remove tiles from tileset for this map

    QSet<QGeoTileSpec> oldTiles = d->mapHash.value(map);

    tile_iter rem = tilesRemoved.constBegin();
    tile_iter remEnd = tilesRemoved.constEnd();
    for (; rem != remEnd; ++rem) {
        oldTiles.remove(*rem);
    }

    tile_iter add = tilesAdded.constBegin();
    tile_iter addEnd = tilesAdded.constEnd();
    for (; add != addEnd; ++add) {
        oldTiles.insert(*add);
    }

    d->mapHash.insert(map, oldTiles);

    // add and remove map from mapset for the tiles

    QSet<QGeoTileSpec> reqTiles;
    QSet<QGeoTileSpec> cancelTiles;

    rem = tilesRemoved.constBegin();
    for (; rem != remEnd; ++rem) {
        QSet<QGeoMap*> mapSet = d->tileHash.value(*rem);
        mapSet.remove(map);
        if (mapSet.isEmpty()) {
            cancelTiles.insert(*rem);
            d->tileHash.remove(*rem);
        } else {
            d->tileHash.insert(*rem, mapSet);
        }
    }

    add = tilesAdded.constBegin();
    for (; add != addEnd; ++add) {
        QSet<QGeoMap*> mapSet = d->tileHash.value(*add);
        if (mapSet.isEmpty()) {
            reqTiles.insert(*add);
        }
        mapSet.insert(map);
        d->tileHash.insert(*add, mapSet);
    }

    cancelTiles -= reqTiles;

    QMetaObject::invokeMethod(d->engine, "updateTileRequests",
                              Qt::QueuedConnection,
                              Q_ARG(QSet<QGeoTileSpec>, reqTiles),
                              Q_ARG(QSet<QGeoTileSpec>, cancelTiles));
}

void QGeoMappingManager::engineTileFinished(const QGeoTileSpec &spec, const QByteArray &bytes, const QString &format)
{
    Q_SHARED_D(QGeoMappingManagerPrivate);
    if (!d) return;

    QSet<QGeoTileCache*> caches;

    QSet<QGeoMap*> maps = d->tileHash.value(spec);

    typedef QSet<QGeoMap*>::const_iterator map_iter;

    map_iter map = maps.constBegin();
    map_iter mapEnd = maps.constEnd();
    for (; map != mapEnd; ++map) {
        caches.insert((*map)->tileCache());

        QSet<QGeoTileSpec> tileSet = d->mapHash.value(*map);
        tileSet.remove(spec);
        if (tileSet.isEmpty())
            d->mapHash.remove(*map);
        else
            d->mapHash.insert(*map, tileSet);
    }

    d->tileHash.remove(spec);

    typedef QSet<QGeoTileCache*>::const_iterator cache_iter;

    cache_iter cache = caches.constBegin();
    cache_iter cacheEnd = caches.constEnd();
    for (; cache != cacheEnd; ++cache) {
        (*cache)->insert(spec, bytes, format, d->engine->cacheHint());
    }

    map = maps.constBegin();
    for (; map != mapEnd; ++map) {
        (*map)->d_ptr->tileFetched(spec);
    }
}

void QGeoMappingManager::engineTileError(const QGeoTileSpec &spec, const QString &errorString)
{
    emit tileError(spec, errorString);
}

QList<QGeoMapType> QGeoMappingManager::supportedMapTypes() const
{
    Q_SHARED_D(QGeoMappingManagerPrivate);
    if (!d) return QList<QGeoMapType>();
    return d->engine->supportedMapTypes();
}

/*!
    Returns the length of the edge of the tiles returned by this manager.

    The tiles are assumed to be square.
*/
int QGeoMappingManager::tileSize() const
{
    Q_SHARED_D(QGeoMappingManagerPrivate);
    if (!d) return -1;
    return d->engine->tileSize();
}

/*!
    Return whether the manager has been initialized
    (will be done automatically but may take some time).

*/
bool QGeoMappingManager::isInitialized() const
{
    Q_SHARED_D(QGeoMappingManagerPrivate);
    if (!d) return false;
    return d->engine->isInitialized();
}

/*!
    Returns the camera capabilities supported by this manager.
*/
QGeoCameraCapabilities QGeoMappingManager::cameraCapabilities() const
{
    Q_SHARED_D(QGeoMappingManagerPrivate);
    if (!d) return QGeoCameraCapabilities();
    return d->engine->cameraCapabilities();
}

/*!
    Sets the locale to be used by the this manager to \a locale.

    If this mapping manager supports returning map labels
    in different languages, they will be returned in the language of \a locale.

    The locale used defaults to the system locale if this is not set.
*/
void QGeoMappingManager::setLocale(const QLocale &locale)
{
    Q_SHARED_D(QGeoMappingManagerPrivate);
    if (!d) return;
    d->engine->setLocale(locale);
}

/*!
    Returns the locale used to hint to this mapping manager about what
    language to use for map labels.
*/
QLocale QGeoMappingManager::locale() const
{
    Q_SHARED_D(QGeoMappingManagerPrivate);
    if (!d) return QLocale();
    return d_ptr->engine->locale();
}

QGeoMappingManager::CacheAreas QGeoMappingManager::cacheHint() const
{
    Q_SHARED_D(QGeoMappingManagerPrivate);
    if (!d) return QGeoMappingManager::CacheAreas();
    return d_ptr->engine->cacheHint();
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
