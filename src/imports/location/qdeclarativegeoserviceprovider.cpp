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

    The Plugin element is used to declaratively specify which available
    GeoServices plugin should be used for various tasks in the Location API.
    Plugins are used by \l Map, \l RouteModel, and \l GeocodeModel
    elements, as well as a variety of others.

    Plugins recognised by the system have a \l name property, a simple string
    normally indicating the name of the service that the Plugin retrieves
    data from. They also have a variety of features, which can be enumerated
    using the \l supported property.

    When a Plugin element is created, it is "detached" and not associated with
    any actual service plugin. Once it has received information via setting
    its \l name or \l required properties, it will choose an appropriate
    service plugin to attach to. Plugin elements can only be attached once;
    to use multiple plugins, create multiple Plugin elements.

    \section2 Example Usage

    The following snippet shows a Plugin element being created with the
    \l required property set. This Plugin will attach to the first plugin
    found that supports both Mapping and Geocoding.

    \code
    Plugin {
        id: plugin
        required: Plugin.MappingFeature | Plugin.GeocodingFeature
    }
    \endcode
*/

QDeclarativeGeoServiceProvider::QDeclarativeGeoServiceProvider(QObject *parent)
:   QObject(parent),
    sharedProvider_(0),
    supported_(NoFeatures),
    required_(NoFeatures),
    complete_(false),
    placesFeatures_(NoPlaceFeatures)
{
    locales_.append(QLocale().name());
}

QDeclarativeGeoServiceProvider::~QDeclarativeGeoServiceProvider()
{
    delete sharedProvider_;
}

/*!
    \qmlproperty string Plugin::name

    This property holds the name of the plugin. Setting this property
    will cause the Plugin to only attach to a plugin with exactly this
    name. The value of \l required will be ignored.
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

void QDeclarativeGeoServiceProvider::update(bool doEmit)
{
    supported_ = NoFeatures;
    placesFeatures_ = NoPlaceFeatures;

    QGeoServiceProvider *serviceProvider = sharedGeoServiceProvider();
    if (!serviceProvider  || serviceProvider->error() != QGeoServiceProvider::NoError) {
        if (doEmit) {
            emit supportedFeaturesChanged(supported_);
            emit supportedPlacesFeaturesChanged(placesFeatures_);
        }
        return;
    }

    if (locales_.isEmpty())
        locales_.append(QLocale().name());

    Q_ASSERT(!locales_.isEmpty());
    QGeocodingManager* geocodingManager = serviceProvider->geocodingManager();
    if (geocodingManager && serviceProvider->error() == QGeoServiceProvider::NoError) {
        if (geocodingManager->supportsGeocoding())
            supported_ |= GeocodingFeature;
        if (geocodingManager->supportsReverseGeocoding())
            supported_ |= ReverseGeocodingFeature;
        geocodingManager->setLocale(QLocale(locales_.at(0)));
    }

    QGeoRoutingManager* routingManager = serviceProvider->routingManager();
    if (routingManager  && serviceProvider->error() == QGeoServiceProvider::NoError) {
        supported_ |= RoutingFeature;
        routingManager->setLocale(QLocale(locales_.at(0)));
    }


    QGeoMappingManager* mappingManager = serviceProvider->mappingManager();
    if (mappingManager  && serviceProvider->error() == QGeoServiceProvider::NoError) {
        supported_ |= MappingFeature;
        mappingManager->setLocale(QLocale(locales_.at(0)));
    }

    QPlaceManager *placeManager = serviceProvider->placeManager();
    if (placeManager && serviceProvider->error() == QGeoServiceProvider::NoError) {
        placesFeatures_ = static_cast<QDeclarativeGeoServiceProvider::PlacesFeatures> ((int)placeManager->supportedFeatures());
        supported_ |= AnyPlacesFeature;

        QList<QLocale> localePreferences;
        foreach (const QString &locale, locales_)
            localePreferences.append(locale);

        placeManager->setLocales(localePreferences);
    }

    if (doEmit) {
        emit supportedFeaturesChanged(supported_);
        emit supportedPlacesFeaturesChanged(placesFeatures_);
    }
}

/*!
    \qmlproperty list<string> Plugin::availableServiceProviders

    This property holds a list of all available service plugins' names. This
    can be used to manually enumerate the available plugins if the
    control provided by \l name and \l required is not sufficient for your
    needs.
*/
QStringList QDeclarativeGeoServiceProvider::availableServiceProviders()
{
    return QGeoServiceProvider::availableServiceProviders();
}

void QDeclarativeGeoServiceProvider::componentComplete()
{
    complete_ = true;
    if (!name_.isEmpty()) {
        update();
        return;
    }
    if (required_ != NoFeatures) {
        QStringList providers = QGeoServiceProvider::availableServiceProviders();
        foreach (QString name, providers) {
            if (sharedProvider_)
                delete sharedProvider_;
            sharedProvider_ = 0;
            name_ = name;
            // do an update with no emits
            update(false);

            if ((supported_ & required_) == required_) {
                // run it again to send the notifications
                emit nameChanged(name_);
                emit supportedFeaturesChanged(supported_);
                emit supportedPlacesFeaturesChanged(placesFeatures_);
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
    \qmlproperty enumeration Plugin::supported

    This property enumerates all supported features of the currently attached
    plugin. Its value will be equal to \c{Plugin.NoFeatures} until the Plugin
    is attached.

    See \l required for a list of possible enumeration values.
*/
QDeclarativeGeoServiceProvider::PluginFeatures QDeclarativeGeoServiceProvider::supportedFeatures() const
{
    return supported_;
}

/*!
    \qmlproperty enumeration Plugin::required

    This property contains the set of features that will be required by the
    Plugin element when choosing which service plugin to attach to. If the
    \l name property is set, this has no effect.

    The following values, or any bitwise combination (AND, OR etc) of these
    may be set:

    \list
    \o Plugin.NoFeatures
    \o Plugin.GeocodingFeature
    \o Plugin.ReverseGeocodingFeature
    \o Plugin.RoutingFeature
    \o Plugin.MappingFeature
    \o Plugin.AnyPlacesFeature
    \endlist
*/
QDeclarativeGeoServiceProvider::PluginFeatures QDeclarativeGeoServiceProvider::requiredFeatures() const
{
    return required_;
}

void QDeclarativeGeoServiceProvider::setRequiredFeatures(const PluginFeatures &features)
{
    required_ = features;
    emit requiredFeaturesChanged(required_);
}

bool QDeclarativeGeoServiceProvider::ready() const
{
    return complete_;
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
QDeclarativeGeoServiceProvider::PlacesFeatures QDeclarativeGeoServiceProvider::supportedPlacesFeatures() const
{
    if ((supported_ & AnyPlacesFeature) == 0)
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

