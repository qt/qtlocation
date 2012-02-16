/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef JSONCONVERTER_H
#define JSONCONVERTER_H

#include <QtCore/QVariant>

#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE

class QPlaceManagerEngineJsonDb;
class QPlace;
class QPlaceSearchRequest;
class QPlaceCategory;

namespace JsonConverter
{
    QVariantMap convertToJsonMap(const QPlace &place);
    QVariantMap convertToJsonMap(const QPlaceCategory &category, const QStringList &ancestors = QStringList());
    QString convertToQueryString(const QPlaceSearchRequest &query);
    QList<QVariantMap> convertToListOfMaps(const QVariant &data);

    QVariantMap addToJsonMap(const QVariant &var, const QPlaceCategory &category);
    QVariantMap addToJsonMap(const QVariant &var, const QPlace &place);

    QList<QPlace> convertJsonResponseToPlaces(const QVariant &response, const QPlaceManagerEngineJsonDb *engine);
    QList<QPlaceCategory> convertJsonResponseToCategories(const QVariant &response, const QPlaceManagerEngineJsonDb *engine);
    QPlace convertJsonMapToPlace(const QVariantMap &map, const QPlaceManagerEngineJsonDb *engine);
    QPlaceCategory convertJsonMapToCategory(const QVariantMap &map, const QPlaceManagerEngineJsonDb *engine);

    static QLatin1String Uuid("_uuid");
    static QLatin1String Type("_type");
    static QLatin1String Query("query");
    static QLatin1String Bindings("bindings");

    static QLatin1String Length("length");
    static QLatin1String Data("data");

    static QLatin1String Name("displayName");
    static QLatin1String PlaceType("com.nokia.mt.location.Place");
    static QLatin1String CategoryUuids("categoryUuids"); //only categories that are directly assigned to a place
    static QLatin1String AllCategoryUuids("allCategoryUuids"); //all categories that a place belongs to,
                                                             //it includes all ancestors of the assigned categories

    static QLatin1String CategoryType("com.nokia.mt.location.PlaceCategory");
    static QLatin1String Ancestors("ancestors");  //includes all ancestor category ids and also the current category id
                                                  //as the last element.   The first category is a top level category id.
    static QLatin1String CategoryParentId("parentId");

    static QLatin1String NotificationType("notification");
    static QLatin1String Actions("actions");

    //coord
    static QLatin1String Coordinate("geo");
    static QLatin1String Latitude("latitude");
    static QLatin1String Longitude("longitude");

    //address
    static QLatin1String Address("address");
    static QLatin1String Street("street");
    static QLatin1String District("district");
    static QLatin1String City("locality");
    static QLatin1String County("county");
    static QLatin1String State("state");
    static QLatin1String Country("country");
    static QLatin1String PostalCode("postalCode");

    static QLatin1String Location("location");

    //contact details
    static QLatin1String Phones("phones");
    static QLatin1String Emails("emails");
    static QLatin1String Websites("urls");

    static QLatin1String PhoneSubType("subType");
    static QLatin1String SubTypeFax("Fax");
    static QLatin1String SubTypeLandline("Landline");

    static QLatin1String Label("label");
    static QLatin1String Value("value");
    static QLatin1String Url("url");

    static QLatin1String ExtendedAttributes("extendedAttributes");
    static QLatin1String Text("text");

    static QLatin1String Height("height");
    static QLatin1String Width("width");

    static QLatin1String Thumbnails("thumbnails");
    static QLatin1String Small("small");
    static QLatin1String Medium("medium");
    static QLatin1String Large("large");
    static QLatin1String Fullscreen("fullscreen");
}

QT_END_NAMESPACE

#endif

