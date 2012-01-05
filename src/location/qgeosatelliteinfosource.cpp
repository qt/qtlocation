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
#include <qgeosatelliteinfosource.h>
#include "qgeopositioninfosourcefactory.h"
#include <QPluginLoader>
#include <QStringList>
#include <QCryptographicHash>
#include <QSettings>
#include <QtCore/private/qfactoryloader_p.h>
#include <QFile>

#if defined(QT_SIMULATOR)
#   include "qgeosatelliteinfosource_simulator_p.h"
#elif defined(Q_OS_WINCE)
#   include "qgeosatelliteinfosource_wince_p.h"
#elif defined(Q_WS_MAEMO_6)
#   include "qgeosatelliteinfosource_maemo_p.h"
#elif defined(Q_WS_MAEMO_5)
#   include "qgeosatelliteinfosource_maemo5_p.h"
#elif defined (NPE_BACKEND)
#   include "qgeosatelliteinfosource_npe_backend_p.h"
#endif

#if defined(Q_WS_MEEGO)
#include "qgeosatelliteinfosource_maemo_p.h"
#if defined(GYPSY_AVAILABLE)
#include "qgeosatelliteinfosource_gypsy_p.h"
#endif
#endif

QT_BEGIN_NAMESPACE

#ifndef QT_NO_LIBRARY
Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader,
        (QT_POSITION_SOURCE_INTERFACE, QLatin1String("/position")))
#endif

class QGeoSatelliteInfoSourcePrivate
{
public:
    int interval;
    static QList<QGeoPositionInfoSourceFactory*> pluginsSorted();
    static QHash<QString, QGeoPositionInfoSourceFactory*> plugins(bool reload = false);
    static void loadDynamicPlugins(QHash<QString, QGeoPositionInfoSourceFactory*> &plugins);
    static void loadStaticPlugins(QHash<QString, QGeoPositionInfoSourceFactory*> &plugins);
};

QHash<QString, QGeoPositionInfoSourceFactory*> QGeoSatelliteInfoSourcePrivate::plugins(bool reload)
{
    static QHash<QString, QGeoPositionInfoSourceFactory*> plugins;
    static bool alreadyDiscovered = false;

    if (reload == true)
        alreadyDiscovered = false;

    if (!alreadyDiscovered) {
        loadStaticPlugins(plugins);
        loadDynamicPlugins(plugins);
        alreadyDiscovered = true;
    }
    return plugins;
}

static bool pluginComparator(const QGeoPositionInfoSourceFactory *p1, const QGeoPositionInfoSourceFactory *p2)
{
    return (p1->sourcePriority() > p2->sourcePriority());
}

QList<QGeoPositionInfoSourceFactory*> QGeoSatelliteInfoSourcePrivate::pluginsSorted()
{
    QList<QGeoPositionInfoSourceFactory*> list = plugins().values();
    qStableSort(list.begin(), list.end(), pluginComparator);
    return list;
}

void QGeoSatelliteInfoSourcePrivate::loadDynamicPlugins(QHash<QString, QGeoPositionInfoSourceFactory *> &plugins)
{
    QPluginLoader qpl;
    QString blockName;

    QSettings settings(QSettings::SystemScope, QLatin1String("Nokia"), QLatin1String("QtLocationPosAndSat"));
    QVariant value = settings.value(QLatin1String("position.plugin.operator.whitelist"));
    if (value.isValid()) {
        QStringList parts = value.toString().split(QLatin1String(","));
        if (parts.size() == 4) {
            QFile file(parts.at(1));
            file.open(QIODevice::ReadOnly);

            QCryptographicHash hash(QCryptographicHash::Sha1);
            while (!file.atEnd()) {
                QByteArray data = file.read(4096);
                hash.addData(data);
            }
            file.close();

            QByteArray hexHash = hash.result().toHex();

            bool loadIt = true;
            if (QString::number(file.size()) != parts.at(3)) {
                qCritical("Position info plugin: bad plugin size for %s",
                          qPrintable(parts.at(1)));
                qWarning("Will fall back to platform default");
                loadIt = false;
            }

            if (hexHash != parts.at(2).toLatin1()) {
                qCritical("Position info plugin: bad plugin hash for %s",
                          qPrintable(parts.at(1)));
                qWarning("Will fall back to platform default");
                loadIt = false;
            }

            if (loadIt) {
                qpl.setFileName(parts.at(1));
                QGeoPositionInfoSourceFactory *f =
                        qobject_cast<QGeoPositionInfoSourceFactory*>(qpl.instance());

                if (f) {
                    QString name = f->sourceName();
                    if (name == parts.at(0)) {
                        plugins.insert(name, f);
                    } else {
                        qCritical("Position info plugin: bad plugin name for %s",
                                  qPrintable(parts.at(1)));
                        qWarning("Will fall back to platform default");
                    }
                }
            }

            // still set blockName to ensure the plugin doesn't load
            blockName = parts.at(1);
        } else {
            qWarning("Position plugin whitelist: invalid format -- should be key,filename,hash,size");
        }
    }

    QFactoryLoader* l = loader();
    QString key;
    for (int i = 0; i < l->keys().count(); ++i) {
        key = l->keys().at(i);
        QGeoPositionInfoSourceFactory *f =
                qobject_cast<QGeoPositionInfoSourceFactory*>(l->instance(key));
        if (f) {
            const QString name = f->sourceName();
            if (name == blockName) {
                delete f;
            } else {
#if !defined QT_NO_DEBUG
                const bool showDebug = qgetenv("QT_DEBUG_PLUGINS").toInt() > 0;
                if (showDebug)
                    qDebug("Dynamic: found a service provider plugin with name %s", qPrintable(name));
#endif
                plugins.insertMulti(name, f);
            }
        }
    }
}

void QGeoSatelliteInfoSourcePrivate::loadStaticPlugins(QHash<QString, QGeoPositionInfoSourceFactory *> &plugins)
{
#if !defined QT_NO_DEBUG
    const bool showDebug = qgetenv("QT_DEBUG_PLUGINS").toInt() > 0;
#endif

    QObjectList staticPlugins = QPluginLoader::staticInstances();
    for (int i = 0; i < staticPlugins.count(); ++i) {
        QGeoPositionInfoSourceFactory *f =
                qobject_cast<QGeoPositionInfoSourceFactory*>(staticPlugins.at(i));

        if (f) {

            QString name = f->sourceName();

#if !defined QT_NO_DEBUG
            if (showDebug)
                qDebug("Static: found a service provider plugin with name %s", qPrintable(name));
#endif
            if (!name.isEmpty()) {
                plugins.insertMulti(name, f);
            }
        }

    }
}

/*!
    \class QGeoSatelliteInfoSource
    \inmodule QtLocation
    \ingroup QtLocation-positioning
    \since QtLocation 5.0

    \brief The QGeoSatelliteInfoSource class is an abstract base class for the distribution of satellite information updates.

    The static function QGeoSatelliteInfoSource::createDefaultSource() creates a default
    satellite data source that is appropriate for the platform, if one is
    available. Otherwise, available QGeoPositionInfoSourceFactory plugins will
    be checked for one that has a satellite data source available.

    Call startUpdates() and stopUpdates() to start and stop regular updates,
    or requestUpdate() to request a single update.
    When an update is available, satellitesInViewUpdated() and/or
    satellitesInUseUpdated() will be emitted.

    If regular satellite updates are required, setUpdateInterval() can be used
    to specify how often these updates should be emitted. If no interval is
    specified, updates are simply provided whenever they are available.
    For example:

    \code
        // Emit updates every 10 seconds if available
        QGeoSatelliteInfoSource *source = QGeoSatelliteInfoSource::createDefaultSource(0);
        if (source)
            source->setUpdateInterval(10000);
    \endcode

    To remove an update interval that was previously set, call
    setUpdateInterval() with a value of 0.

    Note that the satellite source may have a minimum value requirement for
    update intervals, as returned by minimumUpdateInterval().

    \warning On Windows CE it is not possible to detect if a device is GPS enabled.
    The default satellite source on a Windows CE device without GPS support will never provide any satellite data.
*/

/*!
    Creates a satellite source with the specified \a parent.
*/
QGeoSatelliteInfoSource::QGeoSatelliteInfoSource(QObject *parent)
        : QObject(parent),
        d(new QGeoSatelliteInfoSourcePrivate)
{
    d->interval = 0;
}

/*!
    Destroys the satellite source.
*/
QGeoSatelliteInfoSource::~QGeoSatelliteInfoSource()
{
    delete d;
}

/*!
    \property QGeoSatelliteInfoSource::updateInterval
    \brief This property holds the requested interval in milliseconds between each update.

    If the update interval is not set (or is set to 0) the
    source will provide updates as often as necessary.

    If the update interval is set, the source will provide updates at an
    interval as close to the requested interval as possible. If the requested
    interval is less than the minimumUpdateInterval(),
    the minimum interval is used instead.

    Changes to the update interval will happen as soon as is practical, however the
    time the change takes may vary between implementations.  Whether or not the elapsed
    time from the previous interval is counted as part of the new interval is also
    implementation dependent.

    The default value for this property is 0.

    Note: Subclass implementations must call the base implementation of
    setUpdateInterval() so that updateInterval() returns the correct value.
*/
void QGeoSatelliteInfoSource::setUpdateInterval(int msec)
{
    d->interval = msec;
}

int QGeoSatelliteInfoSource::updateInterval() const
{
    return d->interval;
}



/*!
    Creates and returns a source with the specified \a parent that reads
    from the system's default source of satellite update information, or the
    highest priority available plugin.

    Returns 0 if the system has no default source and no valid plugins
    could be found.
*/
QGeoSatelliteInfoSource *QGeoSatelliteInfoSource::createDefaultSource(QObject *parent)
{
    QSettings pluginSettings(QSettings::SystemScope, QLatin1String("Nokia"), QLatin1String("QtLocationPosAndSat"));
    QVariant value = pluginSettings.value(QLatin1String("position.plugin.operator.whitelist"));
    if (value.isValid()) {
        QStringList parts = value.toString().split(QLatin1String(","));
        if (parts.size() == 4) {
            QGeoSatelliteInfoSource *source = createSource(parts.at(0), parent);
            if (source)
                return source;
        }
    }

#if defined(Q_OS_WINCE)
    return new QGeoSatelliteInfoSourceWinCE(parent);
#elif (defined(Q_WS_MAEMO_6)) || (defined(Q_WS_MAEMO_5))
    QGeoSatelliteInfoSourceMaemo *source = new QGeoSatelliteInfoSourceMaemo(parent);
    int status = source->init();

    if (status != -1)
        return source;
    else
        delete source;
#elif defined(QT_SIMULATOR)
    return new QGeoSatelliteInfoSourceSimulator(parent);
#elif defined(Q_WS_MEEGO)
    // Use Maemo6 backend if available, otherwise use Gypsy backend
    QSettings maemo6Settings(QSettings::UserScope, QLatin1String("Nokia"), QLatin1String("QtLocationPosAndSatMaemo6"));
    if (!maemo6Settings.value("maemo6satelliteavailable").isValid()) {
        QGeoSatelliteInfoSourceMaemo *maemoSource = new QGeoSatelliteInfoSourceMaemo(parent);
        int status = maemoSource->init();
        if (status == -1) {
            delete maemoSource;
            maemoSource = 0;
            maemo6Settings.setValue("maemo6satelliteavailable", false);
        } else {
            return maemoSource;
        }
    }
#ifdef GYPSY_AVAILABLE
    QGeoSatelliteInfoSourceGypsy* gypsySource = new QGeoSatelliteInfoSourceGypsy(parent);
    int status = gypsySource->init();
    if (status >= 0)
        return gypsySource;
    delete gypsySource;
#endif // GYPSY_AVAILABLE
#elif defined(NPE_BACKEND)
    QGeoSatelliteInfoSourceNpeBackend* npeBackendSource = new QGeoSatelliteInfoSourceNpeBackend(parent);
    if (npeBackendSource->init())
        return npeBackendSource;
    else
        delete npeBackendSource;
#endif
    foreach (QGeoPositionInfoSourceFactory *f, QGeoSatelliteInfoSourcePrivate::pluginsSorted()) {
        QGeoSatelliteInfoSource *src = f->satelliteInfoSource(parent);
        if (src)
            return src;
    }

    return 0;
}

/*!
    Creates and returns a source with the given \a parent,
    by loading the plugin named \a sourceName.

    Returns 0 if the plugin cannot be found.
*/
QGeoSatelliteInfoSource *QGeoSatelliteInfoSource::createSource(const QString &sourceName, QObject *parent)
{
    QGeoPositionInfoSourceFactory *f = QGeoSatelliteInfoSourcePrivate::plugins().value(sourceName);
    if (f) {
        QGeoSatelliteInfoSource *src = f->satelliteInfoSource(parent);
        if (src)
            return src;
    }
    return 0;
}

/*!
    Returns a list of available source plugins. Note that this does not
    include the default system backend, if one is available.
*/
QStringList QGeoSatelliteInfoSource::availableSources()
{
    return QGeoSatelliteInfoSourcePrivate::plugins().keys();
}

/*!
    \fn void QGeoSatelliteInfoSource::satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &satellites);

    If startUpdates() or requestUpdate() is called, this signal is emitted
    when an update is available on the satellites that are
    currently in view.

    The \a satellites parameter holds the satellites currently in view.
*/

/*!
    \fn void QGeoSatelliteInfoSource::satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &satellites);

    If startUpdates() or requestUpdate() is called, this signal is emitted
    when an update is available on the number of satellites that are
    currently in use.

    These are the satellites that are used to get a "fix" - that
    is, those used to determine the current position.

    The \a satellites parameter holds the satellites currently in use.
*/

/*!
    \property QGeoSatelliteInfoSource::minimumUpdateInterval
    \brief This property holds the minimum time (in milliseconds) required to retrieve a satellite update.

    This is the minimum value accepted by setUpdateInterval() and
    requestUpdate().
*/


/*!
    \fn virtual void QGeoSatelliteInfoSource::startUpdates() = 0;

    Starts emitting updates at regular intervals. The updates will be
    provided whenever new satellite information becomes available.

    \sa satellitesInViewUpdated(), satellitesInUseUpdated()
*/

/*!
    \fn virtual void QGeoSatelliteInfoSource::stopUpdates() = 0;

    Stops emitting updates at regular intervals.
*/

/*!
    \fn virtual void QGeoSatelliteInfoSource::requestUpdate(int timeout = 0);

    Attempts to get the current satellite information and emit
    satellitesInViewUpdated() and satellitesInUseUpdated() with this
    information. If the current position cannot be found
    within the given \a timeout (in milliseconds) or if \a timeout is less than the value returned by
    minimumUpdateInterval(), requestTimeout() is
    emitted.

    If the timeout is zero, the timeout defaults to a reasonable timeout
    period as appropriate for the source.

    This does nothing if another update request is in progress. However
    it can be called even if startUpdates() has already been called and
    regular updates are in progress.
*/

/*!
    \fn void QGeoSatelliteInfoSource::requestTimeout();

    Emitted if requestUpdate() was called and the current satellite
    information could not be retrieved within the specified timeout.
*/

#include "moc_qgeosatelliteinfosource.cpp"

QT_END_NAMESPACE
