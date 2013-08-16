/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativegeoserviceprovider_p.h"
#include "qdeclarativegeomap_p.h"

#include "qdeclarativegeomapmousearea_p.h"
#include "qdeclarativegeomapmouseevent_p.h"

#include "qdeclarativegeoroute_p.h"
#include "qdeclarativegeoroutemodel_p.h"
#include "qdeclarativegeocodemodel_p.h"
#include "qdeclarativegeomaneuver_p.h"
#include "qdeclarativegeomapquickitem_p.h"
#include "qdeclarativegeomapitemview_p.h"
#include "qdeclarativegeomaptype_p.h"
#include "qdeclarativerectanglemapitem_p.h"
#include "qdeclarativecirclemapitem_p.h"
#include "qdeclarativeroutemapitem_p.h"
#include "qdeclarativepolylinemapitem_p.h"
#include "qdeclarativepolygonmapitem_p.h"

//Place includes
#include "qdeclarativecategory_p.h"
#include "qdeclarativeplace_p.h"
#include "qdeclarativeplaceattribute_p.h"
#include "qdeclarativeplaceicon_p.h"
#include "qdeclarativeratings_p.h"
#include "qdeclarativesupplier_p.h"
#include "qdeclarativeplaceuser_p.h"
#include "qdeclarativecontactdetail_p.h"

#include "qdeclarativesupportedcategoriesmodel_p.h"
#include "qdeclarativesearchresultmodel_p.h"
#include "qdeclarativesearchsuggestionmodel_p.h"
#include "error_messages.h"

#include <QtQml/qqmlextensionplugin.h>
#include <QtQml/qqml.h>
#include <QtQml/private/qqmlvaluetype_p.h>
#include <QtQml/private/qqmlglobal_p.h>
#include <QtQml/private/qqmlmetatype_p.h>

#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE


class QLocationDeclarativeModule: public QQmlExtensionPlugin
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface/1.0"
                      FILE "plugin.json")

public:
    virtual void registerTypes(const char *uri)
    {
        if (QLatin1String(uri) == QLatin1String("QtMobility.location")) {
            qWarning("QtMobility.location is not supported. Use QtLocation instead.");
        } else if (QLatin1String(uri) == QLatin1String("Qt.location")) {
            qWarning("Qt.location is not supported. Use QtLocation instead.");
        } else if (QLatin1String(uri) == QLatin1String("QtLocation")) {

            // @uri QtLocation 5.0

            qmlRegisterType<QDeclarativeGeoServiceProvider>(uri, 5, 0, "Plugin");
            qmlRegisterType<QDeclarativeGeoServiceProviderParameter>(uri, 5, 0, "PluginParameter");
            qmlRegisterUncreatableType<QDeclarativeGeoServiceProviderRequirements>(uri, 5, 0, "PluginRequirements", QCoreApplication::translate(CONTEXT_NAME, NOT_INSTANTIABLE_BY_DEVELOPER).arg("PluginRequirements"));
            qmlRegisterType<QDeclarativeGeoMap>(uri, 5, 0, "Map");

            qmlRegisterUncreatableType<QDeclarativeGeoMapItemBase>(uri, 5, 0, "GeoMapItemBase", QCoreApplication::translate(CONTEXT_NAME, NOT_INSTANTIABLE_BY_DEVELOPER).arg("GeoMapItemBase"));
            qmlRegisterType<QDeclarativeGeoMapQuickItem>(uri, 5, 0, "MapQuickItem");
            qmlRegisterType<QDeclarativeGeoMapItemView>(uri, 5, 0, "MapItemView");
            qmlRegisterType<QDeclarativeGeoMapMouseArea>(uri, 5, 0, "MapMouseArea");
            qmlRegisterType<QDeclarativeGeoMapMouseEvent>(uri, 5, 0, "MapMouseEvent");

            qmlRegisterType<QDeclarativeGeocodeModel>(uri, 5, 0, "GeocodeModel"); // geocoding and reverse geocoding
            qmlRegisterType<QDeclarativeGeoRouteModel>(uri, 5, 0, "RouteModel");
            qmlRegisterType<QDeclarativeGeoRouteQuery>(uri, 5, 0, "RouteQuery");
            qmlRegisterType<QDeclarativeGeoRoute>(uri, 5, 0, "Route"); // data type
            qmlRegisterType<QDeclarativeGeoRouteSegment>(uri, 5, 0, "RouteSegment");
            qmlRegisterType<QDeclarativeGeoManeuver>(uri, 5, 0, "RouteManeuver");
            qmlRegisterUncreatableType<QDeclarativeGeoMapPinchEvent>(uri, 5, 0, "MapPinchEvent", QCoreApplication::translate(CONTEXT_NAME, NOT_INSTANTIABLE_BY_DEVELOPER).arg("(Map)PinchEvent"));
            qmlRegisterUncreatableType<QDeclarativeGeoMapGestureArea>(uri, 5, 0, "MapGestureArea", QCoreApplication::translate(CONTEXT_NAME, NOT_INSTANTIABLE_BY_DEVELOPER).arg("(Map)GestureArea"));
            qmlRegisterUncreatableType<QDeclarativeGeoMapType>(uri, 5, 0, "MapType", QCoreApplication::translate(CONTEXT_NAME, NOT_INSTANTIABLE_BY_DEVELOPER).arg("MapType"));
            qmlRegisterType<QDeclarativeCategory>(uri, 5, 0, "Category");
            qmlRegisterType<QDeclarativePlaceEditorialModel>(uri, 5, 0, "EditorialModel");
            qmlRegisterType<QDeclarativePlaceImageModel>(uri, 5, 0, "ImageModel");
            qmlRegisterType<QDeclarativePlace>(uri, 5, 0, "Place");
            qmlRegisterType<QDeclarativePlaceIcon>(uri, 5, 0, "Icon");
            qmlRegisterType<QDeclarativeRatings>(uri, 5, 0, "Ratings");
            qmlRegisterType<QDeclarativeReviewModel>(uri, 5, 0, "ReviewModel");
            qmlRegisterType<QDeclarativeSupplier>(uri, 5, 0, "Supplier");
            qmlRegisterType<QDeclarativePlaceUser>(uri, 5, 0, "User");
            qmlRegisterType<QDeclarativeRectangleMapItem>(uri, 5, 0, "MapRectangle");
            qmlRegisterType<QDeclarativeCircleMapItem>(uri, 5, 0, "MapCircle");
            qmlRegisterType<QDeclarativeMapLineProperties>();
            qmlRegisterType<QDeclarativePolylineMapItem>(uri, 5, 0, "MapPolyline");
            qmlRegisterType<QDeclarativePolygonMapItem>(uri, 5, 0, "MapPolygon");
            qmlRegisterType<QDeclarativeRouteMapItem>(uri, 5, 0, "MapRoute");

            qmlRegisterType<QDeclarativeSupportedCategoriesModel>(uri, 5, 0, "CategoryModel");
            qmlRegisterType<QDeclarativeSearchResultModel>(uri, 5, 0, "PlaceSearchModel");
            qmlRegisterType<QDeclarativeSearchSuggestionModel>(uri, 5, 0, "PlaceSearchSuggestionModel");
            qmlRegisterType<QDeclarativePlaceAttribute>(uri, 5,0, "PlaceAttribute");
            qmlRegisterUncreatableType<QQmlPropertyMap>(uri, 5, 0, "ExtendedAttributes", "ExtendedAttributes instances cannot be instantiated.  "
                                                                   "Only Place types have ExtendedAttributes and they cannot be re-assigned "
                                                                   "(but can be modified).");
            qmlRegisterType<QDeclarativeContactDetail>(uri, 5, 0, "ContactDetail");
            qmlRegisterUncreatableType<QDeclarativeContactDetails>(uri, 5, 0, "ContactDetails", "ContactDetails instances cannot be instantiated.  "
                                                                                                "Only Place types have ContactDetails and they cannot "
                                                                                                "be re-assigned (but can be modified).");
            qRegisterMetaType<QPlaceCategory>("QPlaceCategory");
            qRegisterMetaType<QPlace>("QPlace");
            qRegisterMetaType<QPlaceIcon>("QPlaceIcon");
            qRegisterMetaType<QPlaceRatings>("QPlaceRatings");
            qRegisterMetaType<QPlaceSupplier>("QPlaceSupplier");
            qRegisterMetaType<QPlaceUser>("QPlaceUser");
            qRegisterMetaType<QPlaceAttribute>("QPlaceAttribute");
            qRegisterMetaType<QPlaceContactDetail>("QPlaceContactDetail");
        } else {
            qDebug() << "Unsupported URI given to load location QML plugin: " << QLatin1String(uri);
        }
    }
};

#include "location.moc"

QT_END_NAMESPACE
