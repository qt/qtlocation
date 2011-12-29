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

#include "qdeclarativegeoserviceprovider_p.h"
#include "qgeoserviceprovider.h"
#include "qgeocodingmanager.h"
#include "qgeomappingmanager.h"
#include "qgeoroutingmanager.h"
#include <QtCore/QStringList>

#include <QDebug>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass Plugin QDeclarativeGeoServiceProvider
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-common
    \since QtLocation 5.0

    \brief The Plugin element describes a Location based services plugin.

    A Plugin is necessary in order to make use of many other parts of the Qt
    Location functionality, such as a Map element.

    The simplest way, in many cases, to make use of Plugin is to employ the
    withMapping, withRouting and similar feature properties. When creating a
    Plugin, set the desired features in these properties, and the first
    available service plugin meeting the criteria set will be chosen.

    Alternatively, a Plugin can be instantiated from a function that iterates
    through the contents of availableServiceProviders in order to find
    the desired service plugin.
*/

QDeclarativeGeoServiceProvider::QDeclarativeGeoServiceProvider(QObject *parent)
:   QObject(parent), sharedProvider_(0), supportsGeocoding_(false),
    supportsReverseGeocoding_(false), supportsRouting_(false), supportsMapping_(false),
    supportsPlaces_(false), withGeocoding_(false), withReverseGeocoding_(false),
    withRouting_(false), withMapping_(false), withPlaces_(false), complete_(false)
{
    locales_.append(QLocale().name());
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
        update();
    emit nameChanged(name_);
}

void QDeclarativeGeoServiceProvider::update()
{
    QGeoServiceProvider *serviceProvider = sharedGeoServiceProvider();
    if (!serviceProvider  || serviceProvider->error() != QGeoServiceProvider::NoError) {
        setSupportsGeocoding(false);
        setSupportsReverseGeocoding(false);
        setSupportsRouting(false);
        setSupportsMapping(false);
        setSupportsPlaces(false);
        setSupportedPlacesFeatures(QDeclarativeGeoServiceProvider::NoPlaceFeatures);
        return;
    }

    if (locales_.isEmpty())
        locales_.append(QLocale().name());

    Q_ASSERT(!locales_.isEmpty());
    QGeocodingManager* geocodingManager = serviceProvider->geocodingManager();
    if (!geocodingManager || serviceProvider->error() != QGeoServiceProvider::NoError) {
        setSupportsGeocoding(false);
        setSupportsReverseGeocoding(false);
    } else {
        setSupportsGeocoding(geocodingManager->supportsGeocoding());
        setSupportsReverseGeocoding(geocodingManager->supportsReverseGeocoding());
        geocodingManager->setLocale(QLocale(locales_.at(0)));
    }


    QGeoRoutingManager* routingManager = serviceProvider->routingManager();
    if (!routingManager  || serviceProvider->error() != QGeoServiceProvider::NoError) {
        setSupportsRouting(false);
    } else {
        setSupportsRouting(true);
        routingManager->setLocale(QLocale(locales_.at(0)));
    }


    QGeoMappingManager* mappingManager = serviceProvider->mappingManager();
    if (!mappingManager  || serviceProvider->error() != QGeoServiceProvider::NoError) {
        setSupportsMapping(false);
    } else {
        setSupportsMapping(true);
        mappingManager->setLocale(QLocale(locales_.at(0)));
    }

    QPlaceManager *placeManager = serviceProvider->placeManager();
    if (!placeManager || serviceProvider->error() != QGeoServiceProvider::NoError) {
        setSupportedPlacesFeatures(QDeclarativeGeoServiceProvider::NoPlaceFeatures);
        setSupportsPlaces(false);
    } else {
        setSupportedPlacesFeatures(static_cast<QDeclarativeGeoServiceProvider::PlacesFeatures> ((int)placeManager->supportedFeatures()));
        setSupportsPlaces(true);

        QList<QLocale> localePreferences;
        foreach (const QString &locale, locales_)
            localePreferences.append(locale);

        placeManager->setLocales(localePreferences);
    }
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

void QDeclarativeGeoServiceProvider::setSupportedPlacesFeatures(PlacesFeatures placesFeatures)
{
    if (placesFeatures_ == placesFeatures)
        return;
    placesFeatures_ = placesFeatures;
    emit supportedPlacesFeaturesChanged();
}

void QDeclarativeGeoServiceProvider::componentComplete()
{
    complete_ = true;
    if (!name_.isEmpty()) {
        update();
        return;
    }
    if (withGeocoding_ || withReverseGeocoding_ || withRouting_ ||
            withMapping_ || withPlaces_) {
        QStringList providers = QGeoServiceProvider::availableServiceProviders();
        foreach (QString name, providers) {
            // hack: avoid notifying anyone else when we're just iterating
            blockSignals(true);
            setName(name);
            blockSignals(false);

            bool ok = true;
            if (withGeocoding_ && !supportsGeocoding_)
                ok = false;
            if (withReverseGeocoding_ && !supportsReverseGeocoding_)
                ok = false;
            if (withRouting_ && !supportsRouting_)
                ok = false;
            if (withMapping_ && !supportsMapping_)
                ok = false;
            if (withPlaces_ && !supportsPlaces_)
                ok = false;

            if (ok) {
                // run it again to send the notifications
                name_ = "";
                setName(name);
                break;
            }
        }
    }
}

QString QDeclarativeGeoServiceProvider::name() const
{
    return name_;
}

/*!
    \qmlproperty bool Plugin::withGeocoding
    \qmlproperty bool Plugin::withReverseGeocoding
    \qmlproperty bool Plugin::withRouting
    \qmlproperty bool Plugin::withMapping
    \qmlproperty bool Plugin::withPlaces

    These properties indicate the desired properties of a plugin. If at least
    one of these properties is set at the creation of the Plugin element, it
    will automatically work out which plugin to use, as the first available
    plugin that fulfills the set criteria.

    Note that these properties will only have any effect if set at creation
    time.
*/
bool QDeclarativeGeoServiceProvider::withGeocoding() const
{
    return withGeocoding_;
}
bool QDeclarativeGeoServiceProvider::withReverseGeocoding() const
{
    return withReverseGeocoding_;
}
bool QDeclarativeGeoServiceProvider::withRouting() const
{
    return withRouting_;
}
bool QDeclarativeGeoServiceProvider::withMapping() const
{
    return withMapping_;
}
bool QDeclarativeGeoServiceProvider::withPlaces() const
{
    return withPlaces_;
}

void QDeclarativeGeoServiceProvider::setWithGeocoding(bool value)
{
    withGeocoding_ = value;
}
void QDeclarativeGeoServiceProvider::setWithReverseGeocoding(bool value)
{
    withReverseGeocoding_ = value;
}
void QDeclarativeGeoServiceProvider::setWithRouting(bool value)
{
    withRouting_ = value;
}
void QDeclarativeGeoServiceProvider::setWithMapping(bool value)
{
    withMapping_ = value;
}
void QDeclarativeGeoServiceProvider::setWithPlaces(bool value)
{
    withPlaces_ = value;
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

/*!
    \qmlproperty enumeration Plugin::supportedPlacesFeatures

    This property holds a set of flags indicating what Places related features are provided by the
    plugin. It can be a binary concatenation of the following values:

    \table
        \row
            \o Plugin.NoFeatures
            \o No features specified/supported (value: 0x0).
        \row
            \o Plugin.SavePlaceFeature
            \o The plugin can be used to save places (value: 0x1).
        \row
            \o Plugin.RemovePlaceFeature
            \o The plugin can be used to remove places (value: 0x2).
        \row
            \o Plugin.SaveCategoryFeature
            \o The plugin can be used to save categories (value: 0x4).
        \row
            \o Plugin.RemoveCategoryFeature
            \o The plugin can be used to remove categories (value: 0x8).
        \row
            \o Plugin.RecommendationsFeature
            \o The plugin can provide recommendations (value: 0x10).
        \row
            \o Plugin.SearchSuggestionsFeature\
            \o The plugin can be used to provide search term suggestions (value: 0x20).
        \row
            \o Plugin.CorrectionsFeature
            \o The plugin can provide search term corrections (value: 0x40).
        \row
            \o Plugin.LocaleFeature
            \o The plugin can provide place data information localized according to a set of preferred locales (0x80).
        \row
            \o Plugin.NotificationsFeature
            \o The plugin has notfication mechanisms for when places/categories are added/modified/removed (0x100).
    \endtable
*/
QDeclarativeGeoServiceProvider::PlacesFeatures QDeclarativeGeoServiceProvider::supportedPlacesFeatures()
{
    if (!supportsPlaces_)
        return QDeclarativeGeoServiceProvider::NoPlaceFeatures;
    return placesFeatures_;
}

QGeoServiceProvider *QDeclarativeGeoServiceProvider::sharedGeoServiceProvider()
{
    if (!sharedProvider_)
        sharedProvider_ = new QGeoServiceProvider(name(), parameterMap());

    return sharedProvider_;
}

/*!
    \qmlproperty stringlist Plugin::locales

    This property holds a set of locale preferences.  If the first locale cannot be accommodated, then
    the backend falls back to using the second, and so on.  By default the locales property contains the system locale.

    The locales are specified as strings which have the format
    "language[_script][_country]" or "C", where:

    \list
    \i language is a lowercase, two-letter, ISO 639 language code,
    \i script is a titlecase, four-letter, ISO 15924 script code,
    \i country is an uppercase, two- or three-letter, ISO 3166 country code (also "419" as defined by United Nations),
    \endlist


    The following code demonstrates how to set a single and multiple locales:
    \snippet snippets/declarative/plugin.qml Plugin locale
*/
QStringList QDeclarativeGeoServiceProvider::locales() const
{
    return locales_;
}

void QDeclarativeGeoServiceProvider::setLocales(const QStringList &locales)
{
    if (locales_ == locales)
        return;

    locales_ = locales;

    if (locales_.isEmpty())
        locales_.append(QLocale().name());

    if (complete_)
        update();

    emit localesChanged();
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
    \qmlclass PluginParameter QDeclarativeGeoServiceProviderParameter
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-common
    \since QtLocation 5.0

    \brief The PluginParameter element describes the parameter to a \l Plugin.
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

