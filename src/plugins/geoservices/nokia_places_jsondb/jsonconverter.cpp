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

#include "jsonconverter.h"
#include "qgeoboundingcircle.h"
#include "qplacemanagerengine_jsondb.h"

#include <QtLocation/QPlaceSearchRequest>
#include <QtLocation/QPlaceCategory>
#include <jsondb-client.h>

#include <QDebug>
#include <QString>

QVariantMap JsonConverter::convertToJsonMap(const QPlace &place)
{
    return addToJsonMap(QVariant(), place);
}

QVariantMap JsonConverter::convertToJsonMap(const QPlaceCategory &category, const QStringList &ancestors)
{
    QVariantMap map;
    map.insert(JsonConverter::Ancestors, ancestors);
    return addToJsonMap(map, category);
}

QString JsonConverter::convertToQueryString(const QPlaceSearchRequest &request)
{
    QString queryString;
    if (!request.searchTerm().isEmpty()) {
        queryString += QString::fromLatin1("[?%1=\"%2\"][?%3 =~ \"/%4.*/i\"]")
                        .arg(JsonConverter::Type).arg(JsonConverter::PlaceType).arg(JsonConverter::Name).arg(request.searchTerm());
    }

    if (queryString.isEmpty())
        queryString = QString::fromLatin1("[?%1 = \"%2\"]").arg(JsonConverter::Type).arg(JsonConverter::PlaceType);

    if (request.relevanceHint() == QPlaceSearchRequest::LexicalPlaceNameHint)
        queryString += QString::fromLatin1("[/") + JsonConverter::Name + QLatin1String("]");

    return queryString;
}

QList<QVariantMap> JsonConverter::convertToListOfMaps(const QVariant &data)
{
    QList<QVariantMap> maps;
    foreach (const QVariant &var, data.toMap().value(JsonConverter::Data).toList())
        maps.append(var.toMap());
    return maps;
}

QVariantMap JsonConverter::addToJsonMap(const QVariant &var, const QPlaceCategory &category)
{
    QVariantMap map = var.toMap();
    if (!category.categoryId().isEmpty())
        map.insert(JsonConverter::Uuid, category.categoryId());

    map.insert(JsonConverter::Type, JsonConverter::CategoryType);
    map.insert(JsonConverter::Name, category.name());
    map.insert(JsonConverter::IconUrl, category.icon().fullUrl().toString());
    return map;
}

QVariantMap JsonConverter::addToJsonMap(const QVariant &var, const QPlace &place)
{
    QVariantMap map = var.toMap();
    map.insert(JsonConverter::Type, JsonConverter::PlaceType);
    if (!place.placeId().isEmpty())
        map.insert(JsonConverter::Uuid, place.placeId());
    map.insert(JsonConverter::Name, place.name());

    QVariantMap coordMap;
    coordMap.insert(JsonConverter::Latitude, place.location().coordinate().latitude());
    coordMap.insert(JsonConverter::Longitude, place.location().coordinate().longitude());

    QVariantMap addressMap;
    addressMap.insert(JsonConverter::Street, place.location().address().street());
    addressMap.insert(JsonConverter::District, place.location().address().district());
    addressMap.insert(JsonConverter::City, place.location().address().city());
    addressMap.insert(JsonConverter::County, place.location().address().county());
    addressMap.insert(JsonConverter::State, place.location().address().state());
    addressMap.insert(JsonConverter::Country, place.location().address().country());
    addressMap.insert(JsonConverter::PostalCode, place.location().address().postalCode());

    QVariantMap locationMap;
    locationMap.insert(JsonConverter::Coordinate, coordMap);
    locationMap.insert(JsonConverter::Address, addressMap);
    map.insert(JsonConverter::Location, locationMap);

    foreach (const QString &contactType, place.contactTypes()) {
        if (contactType != QPlaceContactDetail::Phone
                && contactType != QPlaceContactDetail::Fax
                && contactType != QPlaceContactDetail::Website
                && contactType != QPlaceContactDetail::Email) {
            qWarning() << "Contact type ignored when saving place:" << contactType;
        }
    }

    QVariantList phonesJson;
    QVariantMap detailMap;
    foreach (const QPlaceContactDetail &phone, place.contactDetails(QPlaceContactDetail::Phone)) {
        detailMap.insert(JsonConverter::PhoneSubType, JsonConverter::SubTypeLandline);
        detailMap.insert(JsonConverter::Label, phone.label());
        detailMap.insert(JsonConverter::Value, phone.value());
        phonesJson.append(detailMap);
    }

    foreach (const QPlaceContactDetail &fax, place.contactDetails(QPlaceContactDetail::Fax)) {
        detailMap.insert(JsonConverter::PhoneSubType, JsonConverter::SubTypeFax);
        detailMap.insert(JsonConverter::Label, fax.label());
        detailMap.insert(JsonConverter::Value, fax.value());
        phonesJson.append(detailMap);
    }
    map.insert(JsonConverter::Phones, phonesJson);

    QVariantList websitesJson;
    detailMap.clear();
    foreach (const QPlaceContactDetail &website, place.contactDetails(QPlaceContactDetail::Website)) {
        detailMap.insert(JsonConverter::Label, website.label());
        detailMap.insert(JsonConverter::Url, website.value());
        websitesJson.append(detailMap);
    }
    map.insert(JsonConverter::Websites, websitesJson);

    QVariantList emailsJson;
    detailMap.clear();
    foreach (const QPlaceContactDetail &email, place.contactDetails(QPlaceContactDetail::Email)) {
        detailMap.insert(JsonConverter::Label, email.label());
        detailMap.insert(JsonConverter::Value, email.value());
        emailsJson.append(detailMap);
    }
    map.insert(JsonConverter::Emails, emailsJson);

    map.insert(JsonConverter::IconUrl, place.icon().fullUrl().toString());

    QStringList categoryUuids;
    foreach (const QPlaceCategory &category, place.categories())
        categoryUuids.append(category.categoryId());
    map.insert(JsonConverter::CategoryUuids, categoryUuids);

    return map;
}

/* Expected response format
   {
   "data": <array of json objects>
   "explantion":
   "length": <integer>
   "offset": <integer>
   }
*/
QList<QPlace> JsonConverter::convertJsonResponseToPlaces(const QVariant &response,
                                                         const QPlaceManagerEngineJsonDb *engine)
{
    QList<QVariant> data = response.toMap().value(JsonConverter::Data).toList();
    QList<QPlace> places;
    foreach (const QVariant&var, data)
        places.append(JsonConverter::convertJsonMapToPlace(var.toMap(),engine));

    return places;
}

QList<QPlaceCategory> JsonConverter::convertJsonResponseToCategories(const QVariant &response,
                                                                     const QPlaceManagerEngineJsonDb *engine)
{
    QList<QVariant> data = response.toMap().value(JsonConverter::Data).toList();
    QList<QPlaceCategory> categories;
    foreach (const QVariant &var, data)
        categories.append(JsonConverter::convertJsonMapToCategory(var.toMap(),engine));

    return categories;
}

QPlace JsonConverter::convertJsonMapToPlace(const QVariantMap &placeMap,
                                            const QPlaceManagerEngineJsonDb *engine)
{
    QPlace place;
    place.setName(placeMap.value(JsonConverter::Name).toString());
    place.setPlaceId(placeMap.value(JsonConverter::Uuid).toString());

    QVariantMap locationMap = placeMap.value(JsonConverter::Location).toMap();
    QGeoLocation location;

    QVariantMap coordMap = locationMap.value(JsonConverter::Coordinate).toMap();
    QGeoCoordinate coord;
    coord.setLatitude(coordMap.value(JsonConverter::Latitude).toDouble());
    coord.setLongitude(coordMap.value(JsonConverter::Longitude).toDouble());
    location.setCoordinate(coord);

    QVariantMap addressMap = locationMap.value(JsonConverter::Address).toMap();
    QGeoAddress address;
    address.setStreet(addressMap.value(JsonConverter::Street).toString());
    address.setDistrict(addressMap.value(JsonConverter::District).toString());
    address.setCity(addressMap.value(JsonConverter::City).toString());
    address.setCounty(addressMap.value(JsonConverter::County).toString());
    address.setState(addressMap.value(JsonConverter::State).toString());
    address.setCountry(addressMap.value(JsonConverter::Country).toString());
    address.setPostalCode(addressMap.value(JsonConverter::PostalCode).toString());

    location.setAddress(address);
    place.setLocation(location);

    QVariantList phonesJson = placeMap.value(JsonConverter::Phones).toList();

    QPlaceContactDetail detail;
    foreach (const QVariant &phoneJson, phonesJson) {
        detail.setLabel(phoneJson.toMap().value(JsonConverter::Label).toString());
        detail.setValue(phoneJson.toMap().value(JsonConverter::Value).toString());
        if (phoneJson.toMap().value(JsonConverter::PhoneSubType).toString() == JsonConverter::SubTypeLandline)
            place.appendContactDetail(QPlaceContactDetail::Phone, detail);
        else if (phoneJson.toMap().value(JsonConverter::PhoneSubType).toString() == JsonConverter::SubTypeFax)
            place.appendContactDetail(QPlaceContactDetail::Fax, detail);
    }

    QVariantList websitesJson = placeMap.value(JsonConverter::Websites).toList();
    foreach (const QVariant &websiteJson, websitesJson) {
        detail.setLabel(websiteJson.toMap().value(JsonConverter::Label).toString());
        detail.setValue(websiteJson.toMap().value(JsonConverter::Url).toString());
        place.appendContactDetail(QPlaceContactDetail::Website, detail);
    }

    QVariantList emailsJson = placeMap.value(JsonConverter::Emails).toList();
    foreach (const QVariant &emailJson, emailsJson) {
        detail.setLabel(emailJson.toMap().value(JsonConverter::Label).toString());
        detail.setValue(emailJson.toMap().value(JsonConverter::Value).toString());
        place.appendContactDetail(QPlaceContactDetail::Email, detail);
    }

    if (placeMap.keys().contains(JsonConverter::IconUrl) && !placeMap.value(JsonConverter::IconUrl).toString().isEmpty()) {
        QPlaceIcon icon;
        icon.setFullUrl(QUrl(placeMap.value(JsonConverter::IconUrl).toUrl()));
        icon.setManager(engine->manager());
        place.setIcon(icon);
    }

    QStringList categoryUuidList = placeMap.value(JsonConverter::CategoryUuids).toStringList();
    QPlaceCategory cat;
    QList<QPlaceCategory> categories;
    foreach (const QString &categoryUuid, categoryUuidList) {
        cat.setCategoryId(categoryUuid);
        categories << cat;
    }

    place.setCategories(categories);

    return place;
}

QPlaceCategory JsonConverter::convertJsonMapToCategory(const QVariantMap &categoryMap,
                                                       const QPlaceManagerEngineJsonDb *engine)
{
    QPlaceCategory category;
    category.setName(categoryMap.value(JsonConverter::Name).toString());
    category.setCategoryId(categoryMap.value(JsonConverter::Uuid).toString());

    if (categoryMap.keys().contains(JsonConverter::IconUrl) && !categoryMap.value(JsonConverter::IconUrl).toString().isEmpty()) {
        QPlaceIcon icon;
        icon.setFullUrl(QUrl(categoryMap.value(JsonConverter::IconUrl).toUrl()));
        icon.setManager(engine->manager());
        category.setIcon(icon);
    }

    return category;
}
