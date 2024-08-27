// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qdeclarativegeoserviceprovider_p.h"
#include <QtQml/QQmlInfo>
#include <QtQml/QQmlEngine>
#include <QLocale>

QT_BEGIN_NAMESPACE

/*!
    \qmltype Plugin
    //! \nativetype QDeclarativeGeoServiceProvider
    \inqmlmodule QtLocation
    \ingroup qml-QtLocation5-common
    \since QtLocation 5.5

    \brief The Plugin type describes a Location based services plugin.

    The Plugin type is used to declaratively specify which available
    GeoServices plugin should be used for various tasks in the Location API.
    Plugins are used by \l Map, \l RouteModel, and \l GeocodeModel
    types, as well as a variety of others.

    Plugins recognized by the system have a \l name property, a simple string
    normally indicating the name of the service that the Plugin retrieves
    data from. They also have a variety of features, which can be test for using the
    \l {supportsRouting()}, \l {supportsGeocoding()}, \l {supportsMapping()},
    \l {supportsPlaces()} and \l {supportsNavigation()} methods.

    When a Plugin object is created, it is "detached" and not associated with
    any actual service plugin. Once it has received information via setting
    its \l name, \l preferred, or \l required properties, it will choose an
    appropriate service plugin to attach to. Plugin objects can only be
    attached once; to use multiple plugins, create multiple Plugin objects.

    \section2 Example Usage

    The following snippet shows a Plugin object being created with the
    \l required and \l preferred properties set. This Plugin will attach to the
    first found plugin that supports both mapping and geocoding, and will
    prefer plugins named "here" or "osm" to any others.

    \code
    Plugin {
        id: plugin
        preferred: ["here", "osm"]
        required: Plugin.AnyMappingFeatures | Plugin.AnyGeocodingFeatures
    }
    \endcode
*/

QDeclarativeGeoServiceProvider::QDeclarativeGeoServiceProvider(QObject *parent)
:   QObject(parent), required_(new QDeclarativeGeoServiceProviderRequirements)
{
    locales_.append(QLocale().name());
}

QDeclarativeGeoServiceProvider::~QDeclarativeGeoServiceProvider()
{
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

    if (complete_)
        tryAttach();

    emit nameChanged(name_);
}

/*!
    \internal
*/
bool QDeclarativeGeoServiceProvider::parametersReady() {
    for (const QDeclarativePluginParameter *p: std::as_const(parameters_)) {
        if (!p->isInitialized())
            return false;
    }
    return true;
}

/*!
    \internal
*/
void QDeclarativeGeoServiceProvider::tryAttach()
{
    if (!parametersReady())
        return;

    sharedProvider_.reset();

    if (name_.isEmpty())
        return;

    sharedProvider_.reset(new QGeoServiceProvider(name_, parameterMap()));
    sharedProvider_->setQmlEngine(qmlEngine(this));
    sharedProvider_->setLocale(QLocale(locales_.at(0)));
    sharedProvider_->setAllowExperimental(experimental_);

    emit attached();
}

QString QDeclarativeGeoServiceProvider::name() const
{
    return name_;
}


/*!
    \qmlproperty stringlist Plugin::availableServiceProviders

    This property holds a list of all available service plugins' names. This
    can be used to manually enumerate the available plugins if the
    control provided by \l name and \l required is not sufficient for your
    needs.
*/
QStringList QDeclarativeGeoServiceProvider::availableServiceProviders()
{
    return QGeoServiceProvider::availableServiceProviders();
}

/*!
    \internal
*/
void QDeclarativeGeoServiceProvider::componentComplete()
{
    complete_ = true;

    for (QDeclarativePluginParameter *p: std::as_const(parameters_)) {
        if (!p->isInitialized()) {
            connect(p, &QDeclarativePluginParameter::initialized,
                    this, &QDeclarativeGeoServiceProvider::tryAttach);
        }
    }

    if (!name_.isEmpty()) {
        tryAttach();
    } else if (!prefer_.isEmpty()
            || required_->mappingRequirements() != NoMappingFeatures
            || required_->routingRequirements() != NoRoutingFeatures
            || required_->geocodingRequirements() != NoGeocodingFeatures
            || required_->placesRequirements() != NoPlacesFeatures
            || required_->navigationRequirements() != NoNavigationFeatures) {

        QStringList providers = QGeoServiceProvider::availableServiceProviders();

        /* first check any preferred plugins */
        for (const QString &name : std::as_const(prefer_)) {
            if (providers.contains(name)) {
                // so we don't try it again later
                providers.removeAll(name);

                QGeoServiceProvider sp(name, parameterMap(), experimental_);
                if (required_->matches(&sp)) {
                    setName(name);
                    return;
                }
            }
        }

        /* then try the rest */
        for (const QString &name : std::as_const(providers)) {
            QGeoServiceProvider sp(name, parameterMap(), experimental_);
            if (required_->matches(&sp)) {
                setName(name);
                return;
            }
        }

        qmlWarning(this) << "Could not find a plugin with the required features to attach to";
    }
}

/*!
    \qmlmethod bool Plugin::supportsGeocoding(GeocodingFeatures features)

    This method returns a boolean indicating whether the specified set of \a features are supported
    by the geo service provider plugin. \c True is returned if all specified \a features are
    supported; otherwise \c false is returned.

    The \a features parameter can be any flag combination of:
    \table
        \header
            \li Feature
            \li Description
        \row
            \li Plugin.NoGeocodingFeatures
            \li No geocoding features are supported.
        \row
            \li Plugin.OnlineGeocodingFeature
            \li Online geocoding is supported.
        \row
            \li Plugin.OfflineGeocodingFeature
            \li Offline geocoding is supported.
        \row
            \li Plugin.ReverseGeocodingFeature
            \li Reverse geocoding is supported.
        \row
            \li Plugin.LocalizedGeocodingFeature
            \li Supports returning geocoding results with localized addresses.
        \row
            \li Plugin.AnyGeocodingFeatures
            \li Matches a geo service provider that provides any geocoding features.
    \endtable
*/
bool QDeclarativeGeoServiceProvider::supportsGeocoding(const GeocodingFeatures &feature) const
{
    QGeoServiceProvider *sp = sharedGeoServiceProvider();
    QGeoServiceProvider::GeocodingFeatures f =
            static_cast<QGeoServiceProvider::GeocodingFeature>(int(feature));
    if (f == QGeoServiceProvider::AnyGeocodingFeatures)
        return (sp && (sp->geocodingFeatures() != QGeoServiceProvider::NoGeocodingFeatures));
    else
        return (sp && (sp->geocodingFeatures() & f) == f);
}

/*!
    \qmlmethod bool Plugin::supportsMapping(MappingFeatures features)

    This method returns a boolean indicating whether the specified set of \a features are supported
    by the geo service provider plugin.  True is returned if all specified \a features are
    supported; otherwise false is returned.

    The \a features parameter can be any flag combination of:
    \table
        \header
            \li Feature
            \li Description
        \row
            \li Plugin.NoMappingFeatures
            \li No mapping features are supported.
        \row
            \li Plugin.OnlineMappingFeature
            \li Online mapping is supported.
        \row
            \li Plugin.OfflineMappingFeature
            \li Offline mapping is supported.
        \row
            \li Plugin.LocalizedMappingFeature
            \li Supports returning localized map data.
        \row
            \li Plugin.AnyMappingFeatures
            \li Matches a geo service provider that provides any mapping features.
    \endtable
*/
bool QDeclarativeGeoServiceProvider::supportsMapping(const MappingFeatures &feature) const
{
    QGeoServiceProvider *sp = sharedGeoServiceProvider();
    QGeoServiceProvider::MappingFeatures f =
            static_cast<QGeoServiceProvider::MappingFeature>(int(feature));
    if (f == QGeoServiceProvider::AnyMappingFeatures)
        return (sp && (sp->mappingFeatures() != QGeoServiceProvider::NoMappingFeatures));
    else
        return (sp && (sp->mappingFeatures() & f) == f);
}

/*!
    \qmlmethod bool Plugin::supportsRouting(RoutingFeatures features)

    This method returns a boolean indicating whether the specified set of \a features are supported
    by the geo service provider plugin.  True is returned if all specified \a features are
    supported; otherwise false is returned.

    The \a features parameter can be any flag combination of:
    \table
        \header
            \li Feature
            \li Description
        \row
            \li Plugin.NoRoutingFeatures
            \li No routing features are supported.
        \row
            \li Plugin.OnlineRoutingFeature
            \li Online routing is supported.
        \row
            \li Plugin.OfflineRoutingFeature
            \li Offline routing is supported.
        \row
            \li Plugin.LocalizedRoutingFeature
            \li Supports returning routes with localized addresses and instructions.
        \row
            \li Plugin.RouteUpdatesFeature
            \li Updating an existing route based on the current position is supported.
        \row
            \li Plugin.AlternativeRoutesFeature
            \li Supports returning alternative routes.
        \row
            \li Plugin.ExcludeAreasRoutingFeature
            \li Supports specifying a areas which the returned route must not cross.
        \row
            \li Plugin.AnyRoutingFeatures
            \li Matches a geo service provider that provides any routing features.
    \endtable
*/
bool QDeclarativeGeoServiceProvider::supportsRouting(const RoutingFeatures &feature) const
{
    QGeoServiceProvider *sp = sharedGeoServiceProvider();
    QGeoServiceProvider::RoutingFeatures f =
            static_cast<QGeoServiceProvider::RoutingFeature>(int(feature));
    if (f == QGeoServiceProvider::AnyRoutingFeatures)
        return (sp && (sp->routingFeatures() != QGeoServiceProvider::NoRoutingFeatures));
    else
        return (sp && (sp->routingFeatures() & f) == f);
}

/*!
    \qmlmethod bool Plugin::supportsPlaces(PlacesFeatures features)

    This method returns a boolean indicating whether the specified set of \a features are supported
    by the geo service provider plugin.  True is returned if all specified \a features are
    supported; otherwise false is returned.

    The \a features parameter can be any flag combination of:
    \table
        \header
            \li Feature
            \li Description
        \row
            \li Plugin.NoPlacesFeatures
            \li No places features are supported.
        \row
            \li Plugin.OnlinePlacesFeature
            \li Online places is supported.
        \row
            \li Plugin.OfflinePlacesFeature
            \li Offline places is supported.
        \row
            \li Plugin.SavePlaceFeature
            \li Saving categories is supported.
        \row
            \li Plugin.RemovePlaceFeature
            \li Removing or deleting places is supported.
        \row
            \li Plugin.PlaceRecommendationsFeature
            \li Searching for recommended places similar to another place is supported.
        \row
            \li Plugin.SearchSuggestionsFeature
            \li Search suggestions is supported.
        \row
            \li Plugin.LocalizedPlacesFeature
            \li Supports returning localized place data.
        \row
            \li Plugin.NotificationsFeature
            \li Notifications of place and category changes is supported.
        \row
            \li Plugin.PlaceMatchingFeature
            \li Supports matching places from two different geo service providers.
        \row
            \li Plugin.AnyPlacesFeatures
            \li Matches a geo service provider that provides any places features.
    \endtable
*/
bool QDeclarativeGeoServiceProvider::supportsPlaces(const PlacesFeatures &feature) const
{
    QGeoServiceProvider *sp = sharedGeoServiceProvider();
    QGeoServiceProvider::PlacesFeatures f =
            static_cast<QGeoServiceProvider::PlacesFeature>(int(feature));
    if (f == QGeoServiceProvider::AnyPlacesFeatures)
        return (sp && (sp->placesFeatures() != QGeoServiceProvider::NoPlacesFeatures));
    else
        return (sp && (sp->placesFeatures() & f) == f);
}

/*!
    \qmlmethod bool Plugin::supportsNavigation(NavigationFeatures features)

    This method returns a boolean indicating whether the specified set of \a features are supported
    by the geo service provider plugin.  True is returned if all specified \a features are
    supported; otherwise false is returned.

    The \a features parameter can be any flag combination of:
    \table
        \header
            \li Feature
            \li Description
        \row
            \li Plugin.NoNavigationFeatures
            \li No navigation features are supported.
        \row
            \li Plugin.OnlineNavigationFeature
            \li Online navigation is supported.
        \row
            \li Plugin.OfflineNavigationFeature
            \li Offline navigation is supported.
        \row
            \li Plugin.AnyNavigationFeatures
            \li Matches a geo service provider that provides any navigation features.
    \endtable
*/
bool QDeclarativeGeoServiceProvider::supportsNavigation(const QDeclarativeGeoServiceProvider::NavigationFeature &feature) const
{
    QGeoServiceProvider *sp = sharedGeoServiceProvider();
    QGeoServiceProvider::NavigationFeatures f =
            static_cast<QGeoServiceProvider::NavigationFeature>(int(feature));
    if (f == QGeoServiceProvider::AnyNavigationFeatures)
        return (sp && (sp->navigationFeatures() != QGeoServiceProvider::NoNavigationFeatures));
    else
        return (sp && (sp->navigationFeatures() & f) == f);
}

/*!
    \qmlproperty enumeration Plugin::required

    This property contains the set of features that will be required by the
    Plugin object when choosing which service plugin to attach to. If the
    \l name property is set, this has no effect.

    Any of the following values or a bitwise combination of multiple values
    may be set:

    \list
    \li Plugin.NoFeatures
    \li Plugin.GeocodingFeature
    \li Plugin.ReverseGeocodingFeature
    \li Plugin.RoutingFeature
    \li Plugin.MappingFeature
    \li Plugin.AnyPlacesFeature
    \endlist
*/
QDeclarativeGeoServiceProviderRequirements *QDeclarativeGeoServiceProvider::requirements() const
{
    return required_.get();
}

void QDeclarativeGeoServiceProvider::setRequirements(QDeclarativeGeoServiceProviderRequirements *req)
{
    if (!name().isEmpty() || !req)
        return;

    if (required_ && *required_ == *req)
        return;

    required_.reset(req);
    QQmlEngine::setObjectOwnership(req, QQmlEngine::CppOwnership); // To prevent the engine from making this object disappear
}

/*!
    \qmlproperty stringlist Plugin::preferred

    This property contains an ordered list of preferred plugin names, which
    will be checked for the required features set in \l{Plugin::required}{required}
    before any other available plugins are checked.
*/
QStringList QDeclarativeGeoServiceProvider::preferred() const
{
    return prefer_;
}

void QDeclarativeGeoServiceProvider::setPreferred(const QStringList &val)
{
    prefer_ = val;
    emit preferredChanged(prefer_);
}

/*!
    \qmlproperty bool Plugin::isAttached

    This property indicates if the Plugin item is attached to a geoservice provider plugin.
*/
bool QDeclarativeGeoServiceProvider::isAttached() const
{
    return (sharedProvider_ != 0);
}

/*!
    \qmlproperty bool Plugin::allowExperimental

    This property indicates if experimental plugins can be used.
*/
bool QDeclarativeGeoServiceProvider::allowExperimental() const
{
    return experimental_;
}

void QDeclarativeGeoServiceProvider::setAllowExperimental(bool allow)
{
    if (experimental_ == allow)
        return;

    experimental_ = allow;
    if (sharedProvider_)
        sharedProvider_->setAllowExperimental(allow);

    emit allowExperimentalChanged(allow);
}

/*!
    \internal
*/
QGeoServiceProvider *QDeclarativeGeoServiceProvider::sharedGeoServiceProvider() const
{
    return sharedProvider_.get();
}

/*!
    \qmlproperty stringlist Plugin::locales

    This property contains an ordered list of preferred plugin locales.  If the first locale cannot be accommodated, then
    the backend falls back to using the second, and so on.  By default the locales property contains the system locale.

    The locales are specified as strings which have the format
    "language[_script][_country]" or "C", where:

    \list
    \li language is a lowercase, two-letter, ISO 639 language code,
    \li script is a titlecase, four-letter, ISO 15924 script code,
    \li country is an uppercase, two- or three-letter, ISO 3166 country code (also "419" as defined by United Nations),
    \li the "C" locale is identical in behavior to English/UnitedStates as per QLocale
    \endlist

    If the first specified locale cannot be accommodated, the \l {Plugin} falls back to the next and so forth.
    Some \l {Plugin} backends may not support a set of locales which are rigidly defined.  An arbitrary
    example is that some \l {Place}'s in France could have French and English localizations, while
    certain areas in America may only have the English localization available.  In the above scenario,
    the set of supported locales is context dependent on the search location.

    If the \l {Plugin} cannot accommodate any of the preferred locales, the manager falls
    back to using a supported language that is backend specific.

    For \l {Plugin}'s that do not support locales, the locales list is always empty.

    The following code demonstrates how to set a single and multiple locales:
    \snippet declarative/plugin.qml Plugin locale
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

    if (sharedProvider_)
        sharedProvider_->setLocale(QLocale(locales_.at(0)));

    emit localesChanged();
}

/*!
    \qmlproperty list<PluginParameter> Plugin::parameters
    \qmldefault

    This property holds the list of plugin parameters.
*/
QQmlListProperty<QDeclarativePluginParameter> QDeclarativeGeoServiceProvider::parameters()
{
    return QQmlListProperty<QDeclarativePluginParameter>(this,
            0,
            parameter_append,
            parameter_count,
            parameter_at,
            parameter_clear);
}

/*!
    \internal
*/
void QDeclarativeGeoServiceProvider::parameter_append(QQmlListProperty<QDeclarativePluginParameter> *prop, QDeclarativePluginParameter *parameter)
{
    QDeclarativeGeoServiceProvider *p = static_cast<QDeclarativeGeoServiceProvider *>(prop->object);
    p->parameters_.append(parameter);
    if (p->sharedProvider_)
        p->sharedProvider_->setParameters(p->parameterMap());
}

/*!
    \internal
*/
qsizetype QDeclarativeGeoServiceProvider::parameter_count(QQmlListProperty<QDeclarativePluginParameter> *prop)
{
    return static_cast<QDeclarativeGeoServiceProvider *>(prop->object)->parameters_.count();
}

/*!
    \internal
*/
QDeclarativePluginParameter *QDeclarativeGeoServiceProvider::parameter_at(QQmlListProperty<QDeclarativePluginParameter> *prop, qsizetype index)
{
    return static_cast<QDeclarativeGeoServiceProvider *>(prop->object)->parameters_[index];
}

/*!
    \internal
*/
void QDeclarativeGeoServiceProvider::parameter_clear(QQmlListProperty<QDeclarativePluginParameter> *prop)
{
    QDeclarativeGeoServiceProvider *p = static_cast<QDeclarativeGeoServiceProvider *>(prop->object);
    p->parameters_.clear();
    if (p->sharedProvider_)
        p->sharedProvider_->setParameters(p->parameterMap());
}

/*!
    \internal
*/
QVariantMap QDeclarativeGeoServiceProvider::parameterMap() const
{
    QVariantMap map;

    for (const auto *parameter : parameters_)
        map.insert(parameter->name(), parameter->value());

    return map;
}

/*******************************************************************************
*******************************************************************************/

QDeclarativeGeoServiceProviderRequirements::QDeclarativeGeoServiceProviderRequirements(QObject *parent)
    : QObject(parent)
{
}

QDeclarativeGeoServiceProviderRequirements::~QDeclarativeGeoServiceProviderRequirements()
{
}

/*!
    \internal
*/
QDeclarativeGeoServiceProvider::MappingFeatures QDeclarativeGeoServiceProviderRequirements::mappingRequirements() const
{
    return mapping_;
}

/*!
    \internal
*/
void QDeclarativeGeoServiceProviderRequirements::setMappingRequirements(const QDeclarativeGeoServiceProvider::MappingFeatures &features)
{
    if (mapping_ == features)
        return;

    mapping_ = features;
    emit mappingRequirementsChanged(mapping_);
    emit requirementsChanged();
}

/*!
    \internal
*/
QDeclarativeGeoServiceProvider::RoutingFeatures QDeclarativeGeoServiceProviderRequirements::routingRequirements() const
{
    return routing_;
}

/*!
    \internal
*/
void QDeclarativeGeoServiceProviderRequirements::setRoutingRequirements(const QDeclarativeGeoServiceProvider::RoutingFeatures &features)
{
    if (routing_ == features)
        return;

    routing_ = features;
    emit routingRequirementsChanged(routing_);
    emit requirementsChanged();
}

/*!
    \internal
*/
QDeclarativeGeoServiceProvider::GeocodingFeatures QDeclarativeGeoServiceProviderRequirements::geocodingRequirements() const
{
    return geocoding_;
}

/*!
    \internal
*/
void QDeclarativeGeoServiceProviderRequirements::setGeocodingRequirements(const QDeclarativeGeoServiceProvider::GeocodingFeatures &features)
{
    if (geocoding_ == features)
        return;

    geocoding_ = features;
    emit geocodingRequirementsChanged(geocoding_);
    emit requirementsChanged();
}

/*!
    \internal

    */
QDeclarativeGeoServiceProvider::PlacesFeatures QDeclarativeGeoServiceProviderRequirements::placesRequirements() const
{
    return places_;
}

/*!
    \internal
*/
void QDeclarativeGeoServiceProviderRequirements::setPlacesRequirements(const QDeclarativeGeoServiceProvider::PlacesFeatures &features)
{
    if (places_ == features)
        return;

    places_ = features;
    emit placesRequirementsChanged(places_);
    emit requirementsChanged();
}

/*!
    \internal
*/
QDeclarativeGeoServiceProvider::NavigationFeatures QDeclarativeGeoServiceProviderRequirements::navigationRequirements() const
{
    return navigation_;
}

/*!
    \internal
*/
void QDeclarativeGeoServiceProviderRequirements::setNavigationRequirements(const QDeclarativeGeoServiceProvider::NavigationFeatures &features)
{
    if (navigation_ == features)
        return;

    navigation_ = features;
    emit navigationRequirementsChanged(navigation_);
    emit requirementsChanged();
}

/*!
    \internal
*/
bool QDeclarativeGeoServiceProviderRequirements::matches(const QGeoServiceProvider *provider) const
{
    QGeoServiceProvider::MappingFeatures mapping =
            static_cast<QGeoServiceProvider::MappingFeatures>(int(mapping_));

    // extra curlies here to avoid "dangling" else, which could belong to either if
    // same goes for all the rest of these blocks
    if (mapping == QGeoServiceProvider::AnyMappingFeatures) {
        if (provider->mappingFeatures() == QGeoServiceProvider::NoMappingFeatures)
            return false;
    } else {
        if ((provider->mappingFeatures() & mapping) != mapping)
            return false;
    }

    QGeoServiceProvider::RoutingFeatures routing =
            static_cast<QGeoServiceProvider::RoutingFeatures>(int(routing_));

    if (routing == QGeoServiceProvider::AnyRoutingFeatures) {
        if (provider->routingFeatures() == QGeoServiceProvider::NoRoutingFeatures)
            return false;
    } else {
        if ((provider->routingFeatures() & routing) != routing)
            return false;
    }

    QGeoServiceProvider::GeocodingFeatures geocoding =
            static_cast<QGeoServiceProvider::GeocodingFeatures>(int(geocoding_));

    if (geocoding == QGeoServiceProvider::AnyGeocodingFeatures) {
        if (provider->geocodingFeatures() == QGeoServiceProvider::NoGeocodingFeatures)
            return false;
    } else {
        if ((provider->geocodingFeatures() & geocoding) != geocoding)
            return false;
    }

    QGeoServiceProvider::PlacesFeatures places =
            static_cast<QGeoServiceProvider::PlacesFeatures>(int(places_));

    if (places == QGeoServiceProvider::AnyPlacesFeatures) {
        if (provider->placesFeatures() == QGeoServiceProvider::NoPlacesFeatures)
            return false;
    } else {
        if ((provider->placesFeatures() & places) != places)
            return false;
    }

    QGeoServiceProvider::NavigationFeatures navigation =
            static_cast<QGeoServiceProvider::NavigationFeatures>(int(navigation_));

    if (navigation == QGeoServiceProvider::AnyNavigationFeatures) {
        if (provider->navigationFeatures() == QGeoServiceProvider::NoNavigationFeatures)
            return false;
    } else {
        if ((provider->navigationFeatures() & navigation) != navigation)
            return false;
    }

    return true;
}

bool QDeclarativeGeoServiceProviderRequirements::operator == (const QDeclarativeGeoServiceProviderRequirements &rhs) const
{
    return (mapping_ == rhs.mapping_ && routing_ == rhs.routing_
            && geocoding_ == rhs.geocoding_ && places_ == rhs.places_
            && navigation_ == rhs.navigation_);
}

/*******************************************************************************
*******************************************************************************/

/*!
    \qmltype PluginParameter
    //! \nativetype QDeclarativePluginParameter
    \inqmlmodule QtLocation
    \ingroup qml-QtLocation5-common
    \since QtLocation 5.5

    \brief The PluginParameter type describes a parameter for a plugin.

    The PluginParameter object is used to provide a parameter of some kind
    to a plugin. Typically these parameters contain details like an application
    token for access to a service, or a proxy server to use for network access,
    or the serial port to which a serial GPS receiver is connected.

    To set such a parameter, declare a PluginParameter inside an \l Plugin object
    and give it \l{name} and \l{value} properties. A list of valid parameter names
    is available from the
    \l {Qt Location#Plugin References and Parameters}{plugin reference pages}.
*/

/*!
    \qmlproperty string PluginParameter::name

    This property holds the name of the plugin parameter as a single formatted string.
    This property is a write-once property.
*/

/*!
    \qmlproperty QVariant PluginParameter::value

    This property holds the value of the plugin parameter which support different types of values (variant).
    This property is a write-once property.
*/

/*******************************************************************************
 *  Implementation now in positioningquick
*******************************************************************************/

QT_END_NAMESPACE

