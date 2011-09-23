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

#include "qdeclarativepositionsource_p.h"
#include "qdeclarativeposition_p.h"

#include "qdeclarativegeoboundingarea_p.h"
#include "qdeclarativegeoboundingbox_p.h"
#include "qdeclarativegeoboundingcircle_p.h"
#include "qdeclarativegeoaddress_p.h"
#include "qdeclarativecoordinate_p.h"

#include "qdeclarativegeoserviceprovider_p.h"
#include "qdeclarative3dgraphicsgeomap_p.h"

#include "qdeclarativegeomapmousearea_p.h"
#include "qdeclarativegeomapmouseevent_p.h"

#include "qdeclarativegeoroute_p.h"
#include "qdeclarativegeoroutemodel_p.h"
#include "qdeclarativegeocodemodel_p.h"
#include "qdeclarativegeomaneuver_p.h"
#include "qdeclarativegeomapflickable_p.h"
#include "qdeclarativegeomappincharea_p.h"
#include "qdeclarativegeomapitem_p.h"
#include "qdeclarativegeomapobjectview_p.h"

//Place includes
#include "qdeclarativecategory_p.h"
#include "qdeclarativedescription_p.h"
#include "qdeclarativegeolocation_p.h"
#include "qdeclarativeplace_p.h"
#include "qdeclarativeplaceattribute_p.h"
#include "qdeclarativerating_p.h"
#include "qdeclarativesupplier_p.h"

#include "qdeclarativerecommendationmodel_p.h"
#include "qdeclarativesupportedcategoriesmodel_p.h"
#include "qdeclarativesearchresultmodel_p.h"
#include "qdeclarativetextpredictionmodel_p.h"

#include <QtDeclarative/qdeclarativeextensionplugin.h>
#include <QtDeclarative/qdeclarative.h>
#include <qdeclarativepropertymap.h>
#include <QDebug>

QT_BEGIN_NAMESPACE

class QLocationDeclarativeModule: public QDeclarativeExtensionPlugin
{
    Q_OBJECT

public:
    virtual void registerTypes(const char* uri) {
        if (QLatin1String(uri) == QLatin1String("QtMobility.location")) {
            qWarning("QtMobility.location is not supported. Use Qt.location instead.");
        } else if (QLatin1String(uri) == QLatin1String("Qt.location")) {
            // This version numbering is not correct. It is just something to get going
            // until the proper versioning scheme of QML plugins in Qt5 is agreed upon.
            qmlRegisterType<QDeclarativePosition>(uri, 5, 0, "Position");
            qmlRegisterType<QDeclarativePositionSource>(uri, 5, 0, "PositionSource");
            qmlRegisterType<QDeclarativeCoordinate>(uri, 5, 0, "Coordinate");
            qmlRegisterType<QDeclarativeGeoBoundingArea>();
            qmlRegisterType<QDeclarativeGeoBoundingBox>(uri, 5, 0, "BoundingBox");
            qmlRegisterType<QDeclarativeGeoBoundingCircle>(uri, 5, 0, "BoundingCircle");
            qmlRegisterType<QDeclarativeGeoAddress>(uri, 5, 0, "Address");

            qmlRegisterType<QDeclarativeGeoServiceProvider>(uri, 5, 0, "Plugin");
            qmlRegisterType<QDeclarativeGeoServiceProviderParameter>(uri, 5, 0, "PluginParameter");
            qmlRegisterType<QDeclarative3DGraphicsGeoMap>(uri, 5, 0, "Map");
            qmlRegisterType<QDeclarativeGeoMapItem>(uri, 5, 0, "MapItem");
            qmlRegisterType<QDeclarativeGeoMapObjectView>(uri, 5, 0, "MapObjectView");
            qmlRegisterType<QDeclarativeGeoMapMouseArea>(uri, 5, 0, "MapMouseArea");
            qmlRegisterType<QDeclarativeGeoMapMouseEvent>(uri, 5, 0, "MapMouseEvent");

            qmlRegisterType<QDeclarativeGeocodeModel>(uri, 5, 0, "GeocodeModel"); // geocoding and reverse geocoding
            qmlRegisterType<QDeclarativeGeoRouteModel>(uri, 5, 0, "RouteModel");
            qmlRegisterType<QDeclarativeGeoRouteQuery>(uri, 5, 0, "RouteQuery");
            qmlRegisterType<QDeclarativeGeoRoute>(uri, 5, 0, "Route"); // data type
            qmlRegisterType<QDeclarativeGeoRouteSegment>(uri, 5, 0, "RouteSegment");
            qmlRegisterType<QDeclarativeGeoManeuver>(uri, 5, 0, "RouteManeuver");
            qmlRegisterType<QDeclarativeGeoBoundingCircle>(uri, 5, 0, "BoundingCircle");
            qmlRegisterUncreatableType<QDeclarativeGeoMapFlickable>(uri, 5, 0, "MapFlickable", QDeclarativeGeoMapFlickable::tr("(Map)Flickable is not intended instantiable by developer."));
            qmlRegisterUncreatableType<QDeclarativeGeoMapPinchArea>(uri, 5, 0, "MapPinch", QDeclarativeGeoMapPinchArea::tr("(Map)PinchArea is not intended instantiable by developer."));
            qmlRegisterUncreatableType<QDeclarativeGeoMapPinchEvent>(uri, 5, 0, "", QDeclarativeGeoMapPinchEvent::tr("(Map)PinchEvent is not intended instantiable by developer."));

            qmlRegisterType<QDeclarativeCategory>(uri, 5, 0, "Category");
            qmlRegisterType<QDeclarativeDescription>(uri, 5, 0, "Description");
            qmlRegisterType<QDeclarativeGeoLocation>(uri, 5, 0, "Location");
            qmlRegisterType<QDeclarativePlaceImageModel>(uri, 5, 0, "PlaceImageModel");
            qmlRegisterType<QDeclarativePlace>(uri, 5, 0, "Place");
            qmlRegisterType<QDeclarativeRating>(uri, 5, 0, "Rating");
            qmlRegisterType<QDeclarativeReviewModel>(uri, 5, 0, "ReviewModel");
            qmlRegisterType<QDeclarativeSupplier>(uri, 5, 0, "Supplier");

            qmlRegisterType<QDeclarativeRecommendationModel>(uri, 5, 0, "RecommendationModel");
            qmlRegisterType<QDeclarativeSupportedCategoriesModel>(uri, 5, 0, "SupportedCategoriesModel");
            qmlRegisterType<QDeclarativeSearchResultModel>(uri, 5, 0, "SearchResultModel");
            qmlRegisterType<QDeclarativeTextPredictionModel>(uri, 5, 0, "TextPredictionModel");
            qmlRegisterType<QDeclarativePropertyMap>(uri, 5,0,"ExtendedAttributes");
            qmlRegisterType<QDeclarativePlaceAttribute>(uri, 5,0, "PlaceAttribute");
        } else {
            qDebug() << "Unsupported URI given to load location QML plugin: " << QLatin1String(uri);
        }
    }
};

#include "location.moc"

QT_END_NAMESPACE

Q_EXPORT_PLUGIN2(declarative_location, QT_PREPEND_NAMESPACE(QLocationDeclarativeModule));
