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

#include "qgeoserviceprovider.h"
#include "qgeoserviceprovider_p.h"
#include "qgeoserviceproviderfactory.h"

#include "qgeosearchmanager.h"
#include "qgeomappingmanager.h"
#include "qgeoroutingmanager.h"
#include "qgeosearchmanagerengine.h"
#include "qgeomappingmanagerengine.h"
#include "qgeoroutingmanagerengine.h"

#include <QList>
#include <QString>
#include <QVariant>

#include <QPluginLoader>
#include <QDebug>
#include <QStringList>
#include <QCoreApplication>
#include <QObject>
#include <QProcess>
#include <QEventLoop>

#include "qmobilitypluginsearch.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoServiceProvider
    \brief The QGeoServiceProvider class aggregates access to services which provide
    geographical information.

    \inmodule QtLocation
    \since 1.1

    \ingroup maps

    The Maps and Navigation API allows people to access various kinds of
    geographical information, including functionality to perform geocoding,
    routing and the display of maps.  The QGeoServiceProvider aggregates the
    access to a set of these services that are provided by a single vendor.

    It is possible to mix and match service providers for the various domains,
    so that a geocoding manager from one service provider can be used with
    a geographic routing manager from another service provider.

    This is not recommended unless the client is able to verify that the
    data provided by the different services are compatible, as differences
    in the underlying data sets could cause serious incongruences between
    the services.

    Subclasses of QGeoServiceProvider guarantee that the different services
    that they provide are interoperable.

    At this point only the Nokia Services plugin is pacakged with Qt Mobility,
    which is accessible using the provider name "nokia".
*/

/*!
\enum QGeoServiceProvider::Error

Describes an error related to the loading and setup of a service provider
plugin.

\value NoError
No error has occurred.

\value NotSupportedError
The plugin does not support this functionality.

\value UnknownParameterError
The plugin did not recognise one of the parameters it was given.

\value MissingRequiredParameterError
The plugin did not find one of the parameters it was expecting.
*/

/*!
    Returns a list of names of the available service providers, for use with
    the QGeoServiceProvider constructors.
*/
QStringList QGeoServiceProvider::availableServiceProviders()
{
    return QGeoServiceProviderPrivate::plugins().keys();
}

/*!
    Constructs a QGeoServiceProvider whose backend has the name \a
    providerName, using the provided \a parameters.

    If multiple plugins have the same \a providerName, the plugin with the
    highest reported providerVersion() will be used.

    If no plugin matching \a providerName was able to be loaded then error()
    and errorString() will provide details about why this is the case.
*/
QGeoServiceProvider::QGeoServiceProvider(const QString &providerName, const QMap<QString, QVariant> &parameters)
    : d_ptr(new QGeoServiceProviderPrivate())
{
    d_ptr->loadPlugin(providerName, parameters);
    d_ptr->parameterMap = parameters;
}

/*!
    Destroys the service provider object.
*/
QGeoServiceProvider::~QGeoServiceProvider()
{
    delete d_ptr;
}

/*!
    Returns the QGeoSearchManager made available by the service
    provider.

    This function will return 0 if the service provider does not provide
    any geocoding services.

    This function will attempt to construct a QGeoSearchManager instance
    when it is called for the first time.  If the attempt is successful the
    QGeoSearchManager will be cached, otherwise each call of this function
    will attempt to construct a QGeoSearchManager instance until the
    construction is successful.

    After this function has been called, error() and errorString() will
    report any errors which occurred during the construction of the
    QGeoSearchManager.
*/
QGeoSearchManager* QGeoServiceProvider::searchManager() const
{
    if (!d_ptr->factory || (d_ptr->searchError != QGeoServiceProvider::NoError))
        return 0;

    if (!d_ptr->searchManager) {
        QGeoSearchManagerEngine *engine = d_ptr->factory->createSearchManagerEngine(d_ptr->parameterMap,
                                          &(d_ptr->searchError),
                                          &(d_ptr->searchErrorString));
        if (engine) {
            engine->setManagerName(d_ptr->factory->providerName());
            engine->setManagerVersion(d_ptr->factory->providerVersion());
            d_ptr->searchManager = new QGeoSearchManager(engine);
        } else {
            d_ptr->searchError = QGeoServiceProvider::NotSupportedError;
            d_ptr->searchErrorString = "The service provider does not support searchManager().";
        }

        if (d_ptr->searchError != QGeoServiceProvider::NoError) {
            if (d_ptr->searchManager)
                delete d_ptr->searchManager;
            d_ptr->searchManager = 0;
            d_ptr->error = d_ptr->searchError;
            d_ptr->errorString = d_ptr->searchErrorString;
        }
    }

    return d_ptr->searchManager;
}

/*!
    Returns the QGeoMappingManager made available by the service provider.

    This function will return 0 if the service provider does not provide
    any mapping services.

    This function will attempt to construct a QGeoMappingManager instance
    when it is called for the first time.  If the attempt is successful the
    QGeoMappingManager will be cached, otherwise each call of this function
    will attempt to construct a QGeoMappingManager instance until the
    construction is successful.

    After this function has been called, error() and errorString() will
    report any errors which occurred during the construction of the
    QGeoMappingManager.
*/
QGeoMappingManager* QGeoServiceProvider::mappingManager() const
{
    if (!d_ptr->factory || (d_ptr->mappingError != QGeoServiceProvider::NoError))
        return 0;

    if (!d_ptr->mappingManager) {
        QGeoMappingManagerEngine *engine = d_ptr->factory->createMappingManagerEngine(d_ptr->parameterMap,
                                           &(d_ptr->mappingError),
                                           &(d_ptr->mappingErrorString));

        if (engine) {
            engine->setManagerName(d_ptr->factory->providerName());
            engine->setManagerVersion(d_ptr->factory->providerVersion());
            d_ptr->mappingManager = new QGeoMappingManager(engine);
        } else {
            d_ptr->mappingError = QGeoServiceProvider::NotSupportedError;
            d_ptr->mappingErrorString = "The service provider does not support mappingManager().";
        }

        if (d_ptr->mappingError != QGeoServiceProvider::NoError) {
            if (d_ptr->mappingManager)
                delete d_ptr->mappingManager;
            d_ptr->mappingManager = 0;
            d_ptr->error = d_ptr->mappingError;
            d_ptr->errorString = d_ptr->mappingErrorString;
        }
    }

    return d_ptr->mappingManager;
}

/*!
    Returns the QGeoRoutingManager made available by the service provider.

    This function will return 0 if the service provider does not provide
    any geographic routing services.

    This function will attempt to construct a QGeoRoutingManager instance
    when it is called for the first time.  If the attempt is successful the
    QGeoRoutingManager will be cached, otherwise each call of this function
    will attempt to construct a QGeoRoutingManager instance until the
    construction is successful.

    After this function has been called, error() and errorString() will
    report any errors which occurred during the construction of the
    QGeoRoutingManager.
*/
QGeoRoutingManager* QGeoServiceProvider::routingManager() const
{
    if (!d_ptr->factory || (d_ptr->routingError != QGeoServiceProvider::NoError))
        return 0;

    if (!d_ptr->routingManager) {
        QGeoRoutingManagerEngine *engine = d_ptr->factory->createRoutingManagerEngine(d_ptr->parameterMap,
                                           &(d_ptr->routingError),
                                           &(d_ptr->routingErrorString));

        if (engine) {
            engine->setManagerName(d_ptr->factory->providerName());
            engine->setManagerVersion(d_ptr->factory->providerVersion());
            d_ptr->routingManager = new QGeoRoutingManager(engine);
        } else {
            d_ptr->routingError = QGeoServiceProvider::NotSupportedError;
            d_ptr->routingErrorString = "The service provider does not support routingManager().";
        }

        if (d_ptr->routingError != QGeoServiceProvider::NoError) {
            if (d_ptr->routingManager)
                delete d_ptr->routingManager;
            d_ptr->routingManager = 0;
            d_ptr->error = d_ptr->routingError;
            d_ptr->errorString = d_ptr->routingErrorString;
        }
    }

    return d_ptr->routingManager;
}

/*!
    Returns an error code describing the error which occurred during the
    last operation that was performed by this class.
*/
QGeoServiceProvider::Error QGeoServiceProvider::error() const
{
    return d_ptr->error;
}

/*!
    Returns a string describing the error which occurred during the
    last operation that was performed by this class.
*/
QString QGeoServiceProvider::errorString() const
{
    return d_ptr->errorString;
}

/*******************************************************************************
*******************************************************************************/

QGeoServiceProviderPrivate::QGeoServiceProviderPrivate()
    : factory(0),
      searchManager(0),
      routingManager(0),
      mappingManager(0),
      searchError(QGeoServiceProvider::NoError),
      routingError(QGeoServiceProvider::NoError),
      mappingError(QGeoServiceProvider::NoError),
      error(QGeoServiceProvider::NoError) {}

QGeoServiceProviderPrivate::~QGeoServiceProviderPrivate()
{
    if (searchManager)
        delete searchManager;

    if (routingManager)
        delete routingManager;

    if (mappingManager)
        delete mappingManager;
}

void QGeoServiceProviderPrivate::loadPlugin(const QString &providerName, const QMap<QString, QVariant> &parameters)
{
    Q_UNUSED(parameters)

    if (!QGeoServiceProviderPrivate::plugins().keys().contains(providerName)) {
        error = QGeoServiceProvider::NotSupportedError;
        errorString = QString("The geoservices provider %1 is not supported.").arg(providerName);
        factory = 0;
        return;
    }

    factory = 0;

    error = QGeoServiceProvider::NoError;
    errorString = "";

    QList<QGeoServiceProviderFactory*> candidates = QGeoServiceProviderPrivate::plugins().values(providerName);

    int versionFound = -1;

    for (int i = 0; i < candidates.size(); ++i) {
        QGeoServiceProviderFactory* f = candidates[i];
        if (f && (f->providerVersion() > versionFound)) {
            versionFound = f->providerVersion();
            factory = f;
        }
    }
}

QHash<QString, QGeoServiceProviderFactory*> QGeoServiceProviderPrivate::plugins(bool reload)
{
    static QHash<QString, QGeoServiceProviderFactory*> plugins;
    static bool alreadyDiscovered = false;

    if (reload  == true)
        alreadyDiscovered = false;

    if (!alreadyDiscovered) {
        loadStaticPlugins(&plugins);
        loadDynamicPlugins(&plugins);
        alreadyDiscovered = true;
    }
    return plugins;
}

void QGeoServiceProviderPrivate::loadDynamicPlugins(QHash<QString, QGeoServiceProviderFactory*> *plugins)
{
    QStringList paths;
    paths << mobilityPlugins(QLatin1String("geoservices"));

    QPluginLoader qpl;
    for (int i = 0; i < paths.count(); ++i) {
        qpl.setFileName(paths.at(i));

        QGeoServiceProviderFactory *f = qobject_cast<QGeoServiceProviderFactory*>(qpl.instance());
        if (f) {
            QString name = f->providerName();

#if !defined QT_NO_DEBUG
            const bool showDebug = qgetenv("QT_DEBUG_PLUGINS").toInt() > 0;
            if (showDebug)
                qDebug() << "Dynamic: found a service provider plugin with name" << name;
#endif
            plugins->insertMulti(name, f);
        }
    }
}

void QGeoServiceProviderPrivate::loadStaticPlugins(QHash<QString, QGeoServiceProviderFactory*> *plugins)
{
#if !defined QT_NO_DEBUG
    const bool showDebug = qgetenv("QT_DEBUG_PLUGINS").toInt() > 0;
#endif

    QObjectList staticPlugins = QPluginLoader::staticInstances();
    for (int i = 0; i < staticPlugins.count(); ++i) {
        QGeoServiceProviderFactory *f = qobject_cast<QGeoServiceProviderFactory*>(staticPlugins.at(i));
        if (f) {
            QString name = f->providerName();

#if !defined QT_NO_DEBUG
            if (showDebug)
                qDebug() << "Static: found a service provider plugin with name" << name;
#endif
            if (!name.isEmpty()) {
                plugins->insertMulti(name, f);
            }
        }

    }
}

QTM_END_NAMESPACE

