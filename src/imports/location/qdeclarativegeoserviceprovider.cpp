/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#include "qdeclarativegeoserviceprovider_p.h"
#include "qgeoserviceprovider.h"
#include "qgeosearchmanager.h"
#include "qgeomappingmanager.h"
#include "qgeoroutingmanager.h"

#include <QDebug>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass Plugin

    \brief The Plugin element describes a Location based services plugin.
    \inherits QObject

    \ingroup qml-location-maps

    An example of setting the Plugin in the Map element:
    \snippet examples/declarative-location/landmarkmap/landmarkmap.qml Map Plugin

    The Plugin element is part of the \bold{QtMobility.location 1.2} module.
*/

QDeclarativeGeoServiceProvider::QDeclarativeGeoServiceProvider(QObject *parent)
:   QObject(parent), sharedProvider_(0), supportsGeocoding_(false),
    supportsReverseGeocoding_(false), supportsRouting_(false), supportsMapping_(false),
    supportsPlaces_(false), complete_(false)
{
}

QDeclarativeGeoServiceProvider::~QDeclarativeGeoServiceProvider()
{
    delete sharedProvider_;
}

/*!
    \qmlproperty string Plugin::name

    This property holds the name of the plugin.
*/
void QDeclarativeGeoServiceProvider::setName(const QString &name)
{
    if (name_ == name)
        return;

    name_ = name;
    delete sharedProvider_;
    sharedProvider_ = 0;
    if (complete_)
        updateSupportStatus();
    emit nameChanged(name_);
}

void QDeclarativeGeoServiceProvider::updateSupportStatus()
{
    QGeoServiceProvider *serviceProvider = sharedGeoServiceProvider();
    if (!serviceProvider  || serviceProvider->error() != QGeoServiceProvider::NoError) {
        setSupportsGeocoding(false);
        setSupportsReverseGeocoding(false);
        setSupportsRouting(false);
        setSupportsMapping(false);
        return;
    }

    QGeoSearchManager* searchManager = serviceProvider->searchManager();
    if (!searchManager || serviceProvider->error() != QGeoServiceProvider::NoError) {
        setSupportsGeocoding(false);
        setSupportsReverseGeocoding(false);
    } else {
        setSupportsGeocoding(searchManager->supportsGeocoding());
        setSupportsReverseGeocoding(searchManager->supportsReverseGeocoding());
    }

    QGeoRoutingManager* routingManager = serviceProvider->routingManager();
    if (!routingManager  || serviceProvider->error() != QGeoServiceProvider::NoError)
        setSupportsRouting(false);
    else
        setSupportsRouting(true);

    QGeoMappingManager* mappingManager = serviceProvider->mappingManager();
    if (!mappingManager  || serviceProvider->error() != QGeoServiceProvider::NoError)
        setSupportsMapping(false);
    else
        setSupportsMapping(true);

    QPlaceManager *placeManager = serviceProvider->placeManager();
    if (!placeManager || serviceProvider->error() != QGeoServiceProvider::NoError)
        setSupportsPlaces(false);
    else
        setSupportsPlaces(true);
}

QStringList QDeclarativeGeoServiceProvider::availableServiceProviders()
{
    return QGeoServiceProvider::availableServiceProviders();
}

void QDeclarativeGeoServiceProvider::setSupportsGeocoding(bool supports)
{
    if (supports == supportsGeocoding_)
        return;
    supportsGeocoding_ = supports;
    emit supportsGeocodingChanged();
}

void QDeclarativeGeoServiceProvider::setSupportsReverseGeocoding(bool supports)
{
    if (supports == supportsReverseGeocoding_)
        return;
    supportsReverseGeocoding_ = supports;
    emit supportsReverseGeocodingChanged();
}

void QDeclarativeGeoServiceProvider::setSupportsRouting(bool supports)
{
    if (supports == supportsRouting_)
        return;
    supportsRouting_ = supports;
    emit supportsRoutingChanged();
}

void QDeclarativeGeoServiceProvider::setSupportsMapping(bool supports)
{
    if (supports == supportsMapping_)
        return;
    supportsMapping_ = supports;
    emit supportsMappingChanged();
}

void QDeclarativeGeoServiceProvider::setSupportsPlaces(bool supports)
{
    if (supports == supportsPlaces_)
        return;
    supportsPlaces_ = supports;
    emit supportsPlacesChanged();
}

void QDeclarativeGeoServiceProvider::componentComplete()
{
    complete_ = true;
    if (!name_.isEmpty())
        updateSupportStatus();
}

QString QDeclarativeGeoServiceProvider::name() const
{
    return name_;
}



/*!
    \qmlproperty bool Plugin::supportsGeocoding

    This property holds whether plugin supports geocoding.
*/
bool QDeclarativeGeoServiceProvider::supportsGeocoding() const
{
    return supportsGeocoding_;
}

/*!
    \qmlproperty bool Plugin::supportsReverseGeocoding

    This property holds whether plugin supports reverse geocoding.
*/
bool QDeclarativeGeoServiceProvider::supportsReverseGeocoding() const
{
    return supportsReverseGeocoding_;
}

/*!
    \qmlproperty bool Plugin::supportsRouting

    This property holds whether plugin supports routing.
*/
bool QDeclarativeGeoServiceProvider::supportsRouting() const
{
    return supportsRouting_;
}

/*!
    \qmlproperty bool Plugin::supportsMapping

    This property holds whether plugin supports mapping.
*/
bool QDeclarativeGeoServiceProvider::supportsMapping() const
{
    return supportsMapping_;
}

/*!
    \qmlproperty bool Plugin::supportsPlaces

    This property holds whether plugin supports places.
*/
bool QDeclarativeGeoServiceProvider::supportsPlaces() const
{
    return supportsPlaces_;
}

QGeoServiceProvider *QDeclarativeGeoServiceProvider::sharedGeoServiceProvider()
{
    if (!sharedProvider_)
        sharedProvider_ = new QGeoServiceProvider(name(), parameterMap());

    return sharedProvider_;
}

/*!
    \qmlproperty list<PluginParameter> Plugin::parameters
    \default

    This property holds the list of plugin parameters.
*/
QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> QDeclarativeGeoServiceProvider::parameters()
{
    return QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter>(this,
            0,
            parameter_append,
            parameter_count,
            parameter_at,
            parameter_clear);
}

void QDeclarativeGeoServiceProvider::parameter_append(QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> *prop, QDeclarativeGeoServiceProviderParameter *parameter)
{
    static_cast<QDeclarativeGeoServiceProvider*>(prop->object)->parameters_.append(parameter);
    delete static_cast<QDeclarativeGeoServiceProvider*>(prop->object)->sharedProvider_;
    static_cast<QDeclarativeGeoServiceProvider*>(prop->object)->sharedProvider_ = 0;
}

int QDeclarativeGeoServiceProvider::parameter_count(QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> *prop)
{
    return static_cast<QDeclarativeGeoServiceProvider*>(prop->object)->parameters_.count();
}

QDeclarativeGeoServiceProviderParameter* QDeclarativeGeoServiceProvider::parameter_at(QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> *prop, int index)
{
    return static_cast<QDeclarativeGeoServiceProvider*>(prop->object)->parameters_[index];
}

void QDeclarativeGeoServiceProvider::parameter_clear(QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> *prop)
{
    static_cast<QDeclarativeGeoServiceProvider*>(prop->object)->parameters_.clear();
    delete static_cast<QDeclarativeGeoServiceProvider*>(prop->object)->sharedProvider_;
    static_cast<QDeclarativeGeoServiceProvider*>(prop->object)->sharedProvider_ = 0;
}

QMap<QString, QVariant> QDeclarativeGeoServiceProvider::parameterMap() const
{
    QMap<QString, QVariant> map;

    for(int i = 0; i < parameters_.size(); ++i) {
        QDeclarativeGeoServiceProviderParameter *parameter = parameters_.at(i);
        map.insert(parameter->name(), parameter->value());
    }

    return map;
}
/*******************************************************************************
*******************************************************************************/

/*!
    \qmlclass PluginParameter

    \brief The PluginParameter element describes the parameter to a \l Plugin.
    \inherits QObject

    \ingroup qml-location-maps

    The PluginParameter element is part of the \bold{QtMobility.location 1.2} module.
*/

QDeclarativeGeoServiceProviderParameter::QDeclarativeGeoServiceProviderParameter(QObject *parent)
    : QObject(parent) {}

QDeclarativeGeoServiceProviderParameter::~QDeclarativeGeoServiceProviderParameter() {}

/*!
    \qmlproperty string PluginParameter::name

    This property holds the name of the plugin parameter.
*/
void QDeclarativeGeoServiceProviderParameter::setName(const QString &name)
{
    if (name_ == name)
        return;

    name_ = name;

    emit nameChanged(name_);
}

QString QDeclarativeGeoServiceProviderParameter::name() const
{
    return name_;
}

/*!
    \qmlproperty QVariant PluginParameter::value

    This property holds the value of the plugin parameter.
*/
void QDeclarativeGeoServiceProviderParameter::setValue(const QVariant &value)
{
    if (value_ == value)
        return;

    value_ = value;

    emit valueChanged(value_);
}

QVariant QDeclarativeGeoServiceProviderParameter::value() const
{
    return value_;
}

/*******************************************************************************
*******************************************************************************/

#include "moc_qdeclarativegeoserviceprovider_p.cpp"

QT_END_NAMESPACE

