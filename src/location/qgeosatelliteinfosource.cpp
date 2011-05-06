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
#include <qgeosatelliteinfosource.h>
#include "qgeopositioninfosourcefactory.h"
#include <QPluginLoader>
#include <QStringList>
#include <QCryptographicHash>
#include <QSettings>
#include "qmobilitypluginsearch.h"

#if defined(Q_OS_SYMBIAN)
#   include "qgeosatelliteinfosource_s60_p.h"
#elif defined(QT_SIMULATOR)
#   include "qgeosatelliteinfosource_simulator_p.h"
#elif defined(Q_OS_WINCE)
#   include "qgeosatelliteinfosource_wince_p.h"
#elif defined(Q_WS_MAEMO_6)
#   include "qgeosatelliteinfosource_maemo_p.h"
#elif defined(Q_WS_MAEMO_5)
#   include "qgeosatelliteinfosource_maemo5_p.h"
#endif

#if defined(Q_WS_MEEGO)
#include "qgeosatelliteinfosource_maemo_p.h"
#if defined(GYPSY_AVAILABLE)
#include "qgeosatelliteinfosource_gypsy_p.h"
#endif
#endif

QTM_BEGIN_NAMESPACE

class QGeoSatelliteInfoSourcePrivate
{
public:
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
    QStringList paths;
    paths << mobilityPlugins(QLatin1String("position"));

    QPluginLoader qpl;
    QString blockName;

    QSettings settings(QSettings::SystemScope, QLatin1String("Nokia"), QLatin1String("QtLocationPosAndSat"));
    QVariant value = settings.value("position.plugin.operator.whitelist");
    if (value.isValid()) {
        QStringList parts = value.toString().split(",");
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

    for (int i = 0; i < paths.count(); ++i) {
        if (paths.at(i) != blockName) {
            qpl.setFileName(paths.at(i));

            QGeoPositionInfoSourceFactory *f =
                    qobject_cast<QGeoPositionInfoSourceFactory*>(qpl.instance());
            if (f) {
                QString name = f->sourceName();

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
    \brief The QGeoSatelliteInfoSource class is an abstract base class for the distribution of satellite information updates.

    \inmodule QtLocation
    \since 1.0

    \ingroup location

    The static function QGeoSatelliteInfoSource::createDefaultSource() creates a default
    satellite data source that is appropriate for the platform, if one is
    available. Otherwise, available QGeoPositionInfoSourceFactory plugins will
    be checked for one that has a satellite data source available.

    Call startUpdates() and stopUpdates() to start and stop regular updates,
    or requestUpdate() to request a single update.
    When an update is available, satellitesInViewUpdated() and/or
    satellitesInUseUpdated() will be emitted.

    \warning On Windows CE it is not possible to detect if a device is GPS enabled.
    The default satellite source on a Windows CE device without GPS support will never provide any satellite data.

    \warning On Symbian it is currently only possible to instantiate and use the satellite sources in the main thread
    of the application.
*/

/*!
    Creates a source with the specified \a parent.
*/
QGeoSatelliteInfoSource::QGeoSatelliteInfoSource(QObject *parent)
        : QObject(parent)
{
}

/*!
    Creates and returns a source with the specified \a parent that reads
    from the system's default source of satellite update information, or the
    highest priority available plugin.

    Returns 0 if the system has no default source and no valid plugins
    could be found.

    Note: Symbian applications will need to have the Location capability
    otherwise this will return 0.
*/
QGeoSatelliteInfoSource *QGeoSatelliteInfoSource::createDefaultSource(QObject *parent)
{
    QSettings pluginSettings(QSettings::SystemScope, QLatin1String("Nokia"), QLatin1String("QtLocationPosAndSat"));
    QVariant value = pluginSettings.value("position.plugin.operator.whitelist");
    if (value.isValid()) {
        QStringList parts = value.toString().split(",");
        if (parts.size() == 4) {
            QGeoSatelliteInfoSource *source = createSource(parts.at(0), parent);
            if (source)
                return source;
        }
    }

#if defined(Q_OS_SYMBIAN)
    CQGeoSatelliteInfoSourceS60 *ret = NULL;
    TRAPD(error, QT_TRYCATCH_LEAVING(ret = CQGeoSatelliteInfoSourceS60::NewL(parent)));
    if (error == KErrNone)
        return ret;
#elif defined(Q_OS_WINCE)
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
    within the given \a timeout (in milliseconds), requestTimeout() is
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

QTM_END_NAMESPACE
