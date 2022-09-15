/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtLocation/private/qdeclarativegeoserviceprovider_p.h>
#include <QtPositioningQuick/private/qdeclarativepluginparameter_p.h>
#include <QtLocation/private/qdeclarativegeomap_p.h>

#include <QtLocation/private/qdeclarativegeoroute_p.h>
#include <QtLocation/private/qdeclarativegeoroutemodel_p.h>
#include <QtLocation/private/qdeclarativegeocodemodel_p.h>
#include <QtLocation/private/qdeclarativegeomapitembase_p.h>
#include <QtLocation/private/qdeclarativegeomapquickitem_p.h>
#include <QtLocation/private/qdeclarativegeomapitemview_p.h>
#include <QtLocation/private/qdeclarativerectanglemapitem_p.h>
#include <QtLocation/private/qdeclarativecirclemapitem_p.h>
#include <QtLocation/private/qdeclarativeroutemapitem_p.h>
#include <QtLocation/private/qdeclarativepolylinemapitem_p.h>
#include <QtLocation/private/qdeclarativepolygonmapitem_p.h>
#include <QtLocation/private/qdeclarativegeomapparameter_p.h>
#include <QtLocation/private/qdeclarativegeomapcopyrightsnotice_p.h>
#include <QtLocation/private/qdeclarativegeomapitemgroup_p.h>
#include <QtLocation/private/qgeomapobject_p.h>

//Place includes
#include <QtLocation/private/qdeclarativecategory_p.h>
#include <QtLocation/private/qdeclarativeplace_p.h>
#include <QtLocation/private/qdeclarativecontactdetails_p.h>

#include <QtLocation/private/qdeclarativesupportedcategoriesmodel_p.h>
#include <QtLocation/private/qdeclarativesearchresultmodel_p.h>
#include <QtLocation/private/qdeclarativesearchsuggestionmodel_p.h>

#include <QtQml/qqmlextensionplugin.h>

#include <QtCore/QDebug>

#include "location.h"

QT_BEGIN_NAMESPACE

namespace {

bool convertToGadget(const QMetaObject &metaObject, const QVariantMap &map, void *gadget)
{
    for (auto &&[key, value] : map.asKeyValueRange()) {
        const int propIndex = metaObject.indexOfProperty(key.toUtf8());
        if (propIndex == -1) {
            qCritical("No property %s in %s", qPrintable(key), metaObject.className());
            return false;
        }
        const QMetaProperty prop = metaObject.property(propIndex);
        bool successfulWrite = false;
        if (value.metaType() != prop.metaType()) {
            QVariant coercedValue = value;
            if (!coercedValue.convert(prop.metaType())) {
                qCritical("Could not convert value from %s to %s for property %s::%s",
                          value.typeName(), prop.typeName(), metaObject.className(), qPrintable(key));
                return false;
            }
            successfulWrite = prop.writeOnGadget(gadget, coercedValue);
        } else {
            successfulWrite = prop.writeOnGadget(gadget, value);
        }
        if (!successfulWrite) {
            qCritical("Could not set property %s on %s", qPrintable(key), metaObject.className());
            return false;
        }
    }
    return true;
}

template<typename SourceType, typename GadgetType>
bool converterToGadget(const void *src, void *gadget)
{
    const QMetaObject &metaObject = GadgetType::staticMetaObject;
    QVariantMap variantMap;
    if constexpr (std::is_same_v<SourceType, QJSValue>) {
        const QJSValue &jsValue = *static_cast<const QJSValue *>(src);
        const QVariant &variant = jsValue.toVariant();
        if (variant.metaType() != QMetaType::fromType<QVariantMap>())
            return false;
        variantMap = variant.toMap();
    } else {
        static_assert(std::is_same_v<SourceType, QVariantMap>);
        variantMap = *static_cast<const QVariantMap *>(src);
    }
    return convertToGadget(metaObject, variantMap, gadget);
}

template<typename GadgetType>
bool registerConverterToGadget()
{
    if (!QMetaType::registerConverterFunction(converterToGadget<QJSValue, GadgetType>,
                    QMetaType::fromType<QJSValue>(), QMetaType::fromType<GadgetType>())) {
        qCritical("Failed to register conversion function from QJSValue to %s",
                  GadgetType::staticMetaObject.className());
        return false;
    }
    if (!QMetaType::registerConverterFunction(converterToGadget<QVariantMap, GadgetType>,
                    QMetaType::fromType<QVariantMap>(), QMetaType::fromType<GadgetType>())) {
        qCritical("Failed to register conversion function from QVariantMap to %s",
                  GadgetType::staticMetaObject.className());
        return false;
    }

    return true;
}

} // anonymous namespace

class QtLocationDeclarativeModule: public QQmlExtensionPlugin
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    QtLocationDeclarativeModule(QObject *parent = nullptr) : QQmlExtensionPlugin(parent) { }
    void registerTypes(const char *uri) override
    {
        if (QLatin1String(uri) == QLatin1String("QtLocation")) {

            // @uri QtLocation
            int major = 5;
            int minor = 0;

            // Register the 5.0 types
            // 5.0 is silent and not advertised

            qmlRegisterType<QDeclarativeGeoServiceProvider          >(uri, major, minor, "Plugin");
            qmlRegisterType<QDeclarativePluginParameter >(uri, major, minor, "PluginParameter");
            qmlRegisterUncreatableType<QDeclarativeGeoServiceProviderRequirements>(uri, major, minor, "PluginRequirements",
                                        QStringLiteral("PluginRequirements is not intended instantiable by developer."));
            qmlRegisterType<QDeclarativeGeoMap                      >(uri, major, minor, "Map");

            qmlRegisterUncreatableType<QDeclarativeGeoMapItemBase   >(uri, major, minor, "GeoMapItemBase",
                                        QStringLiteral("GeoMapItemBase is not intended instantiable by developer."));
            qmlRegisterType<QDeclarativeGeoMapQuickItem             >(uri, major, minor, "MapQuickItem");
            qmlRegisterType<QDeclarativeGeoMapItemView              >(uri, major, minor, "MapItemView");

            qmlRegisterType<QDeclarativeGeocodeModel                >(uri, major, minor, "GeocodeModel"); // geocoding and reverse geocoding
            qmlRegisterType<QDeclarativeGeoRouteModel               >(uri, major, minor, "RouteModel");
            qmlRegisterType<QDeclarativeGeoRouteQuery               >(uri, major, minor, "RouteQuery");
            qmlRegisterType<QDeclarativeGeoRoute                    >(uri, major, minor, "Route"); // data type
            qmlRegisterUncreatableType<QGeoMapPinchEvent >(uri, major, minor, "MapPinchEvent",
                                        QStringLiteral("(Map)PinchEvent is not intended instantiable by developer."));
            qmlRegisterUncreatableType<QQuickGeoMapGestureArea>(uri, major, minor, "MapGestureArea",
                                        QStringLiteral("(Map)GestureArea is not intended instantiable by developer."));
            qmlRegisterType<QDeclarativeCategory                    >(uri, major, minor, "Category");
            qmlRegisterType<QDeclarativePlaceEditorialModel         >(uri, major, minor, "EditorialModel");
            qmlRegisterType<QDeclarativePlaceImageModel             >(uri, major, minor, "ImageModel");
            qmlRegisterType<QDeclarativePlace                       >(uri, major, minor, "Place");
            qmlRegisterType<QDeclarativePlaceReviewModel            >(uri, major, minor, "ReviewModel");
            qmlRegisterType<QDeclarativeRectangleMapItem            >(uri, major, minor, "MapRectangle");
            qmlRegisterType<QDeclarativeCircleMapItem               >(uri, major, minor, "MapCircle");
            qmlRegisterAnonymousType<QDeclarativeMapLineProperties  >(uri, major);
            qmlRegisterType<QDeclarativePolylineMapItem             >(uri, major, minor, "MapPolyline");
            qmlRegisterType<QDeclarativePolygonMapItem              >(uri, major, minor, "MapPolygon");
            qmlRegisterType<QDeclarativeRouteMapItem                >(uri, major, minor, "MapRoute");

            qmlRegisterType<QDeclarativeSupportedCategoriesModel    >(uri, major, minor, "CategoryModel");
            qmlRegisterType<QDeclarativeSearchResultModel           >(uri, major, minor, "PlaceSearchModel");
            qmlRegisterType<QDeclarativeSearchSuggestionModel       >(uri, major, minor, "PlaceSearchSuggestionModel");
            qmlRegisterUncreatableType<QQmlPropertyMap              >(uri, major, minor, "ExtendedAttributes", "ExtendedAttributes instances cannot be instantiated.  "
                                                                   "Only Place types have ExtendedAttributes and they cannot be re-assigned "
                                                                   "(but can be modified).");
            qmlRegisterUncreatableType<QDeclarativeContactDetails   >(uri, major, minor, "ContactDetails", "ContactDetails instances cannot be instantiated.  "
                                                                                                "Only Place types have ContactDetails and they cannot "
                                                                                                "be re-assigned (but can be modified).");

            // Introduction of 5.3 version; existing 5.0 exports automatically become available under 5.3 as well
            // 5.3 is committed QML API despite missing release of QtLocation 5.3

            minor = 6;
            //TODO: this is broken QTBUG-50990
            qmlRegisterUncreatableType<QQuickGeoMapGestureArea, 1>(uri, major, minor, "MapGestureArea",
                                        QStringLiteral("(Map)GestureArea is not intended instantiable by developer."));

            // Register the 5.8 types

            // Register the 5.9 types
            minor = 9;
            qmlRegisterType<QDeclarativeGeoMapParameter>(uri, major, minor, "MapParameter");
            qmlRegisterType<QDeclarativeGeoMapCopyrightNotice>(uri, major, minor, "MapCopyrightNotice");
            qmlRegisterType<QDeclarativeGeoMapItemGroup>(uri, major, minor, "MapItemGroup");

            // Register the 5.11 types
            minor = 11;
            qmlRegisterAnonymousType<QGeoMapObject>(uri, major);
            qmlRegisterType<QDeclarativeGeoMap, 11>(uri, major, minor, "Map");
            qmlRegisterUncreatableType<QDeclarativeGeoMapItemBase, 11>(uri, major, minor, "GeoMapItemBase",
                                        QStringLiteral("GeoMapItemBase is not intended instantiable by developer."));
            qmlRegisterType<QDeclarativeGeoMapParameter>(uri, major, minor, "DynamicParameter");
            qmlRegisterType<QDeclarativeGeoRoute, 11>(uri, major, minor, "Route");
            qmlRegisterType<QDeclarativeGeoRouteQuery, 11>(uri, major, minor, "RouteQuery");
            qmlRegisterType<QDeclarativeGeoServiceProvider, 11>(uri, major, minor, "Plugin");
            qmlRegisterType<QDeclarativeGeoWaypoint>(uri, major, minor, "Waypoint");

            // Register the 5.12 types
            minor = 12;
            qmlRegisterType<QDeclarativeGeoMapItemView, 12>(uri, major, minor, "MapItemView");
            qmlRegisterType<QDeclarativeSearchResultModel, 12>(uri, major, minor, "PlaceSearchModel");
            qmlRegisterType<QDeclarativeGeoMap, 12>(uri, major, minor, "Map");
            qmlRegisterType<QDeclarativeGeoRoute, 12>(uri, major, minor, "Route");

            minor = 13;
            qmlRegisterType<QDeclarativeGeoMap, 13>(uri, major, minor, "Map");
            qmlRegisterType<QDeclarativeGeoRoute, 13>(uri, major, minor, "Route");
            qmlRegisterType<QDeclarativeGeoRouteQuery, 13>(uri, major, minor, "RouteQuery");

            minor = 14;
            qmlRegisterType<QDeclarativeGeoMap, 14>(uri, major, minor, "Map");
            qmlRegisterUncreatableType<QDeclarativeGeoMapItemBase, 14>(uri, major, minor, "GeoMapItemBase",
                                        QStringLiteral("GeoMapItemBase is not intended instantiable by developer."));

            minor = 15;
            qmlRegisterType<QDeclarativePolylineMapItem,  15>(uri, major, minor, "MapPolyline");
            qmlRegisterType<QDeclarativePolygonMapItem,   15>(uri, major, minor, "MapPolygon");
            qmlRegisterType<QDeclarativeRectangleMapItem, 15>(uri, major, minor, "MapRectangle");
            qmlRegisterType<QDeclarativeCircleMapItem,    15>(uri, major, minor, "MapCircle");
            qmlRegisterUncreatableType<QDeclarativeGeoMapItemBase, 15>(uri, major, minor, "GeoMapItemBase",
                                        QStringLiteral("GeoMapItemBase is not intended instantiable by developer."));

            // The minor version used to be the current Qt 5 minor. For compatibility it is the last
            // Qt 5 release.
            qmlRegisterModule(uri, 5, 15);

            //registrations below are version independent
            qRegisterMetaType<QPlaceCategory>();
            qRegisterMetaType<QPlace>();

            registerConverterToGadget<QPlaceRatings>();
            registerConverterToGadget<QPlaceIcon>();
            registerConverterToGadget<QPlaceSupplier>();
        } else {
            qDebug() << "Unsupported URI given to load location QML plugin: " << QLatin1String(uri);
        }
    }
};

QT_END_NAMESPACE

#include "location.moc"
#include "moc_location.cpp"
