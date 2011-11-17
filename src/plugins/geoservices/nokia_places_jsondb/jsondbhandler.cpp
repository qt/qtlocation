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

#include "jsondbhandler.h"
#include "qgeoboundingcircle.h"
#include "qplacemanagerengine_jsondb.h"

#include <QtLocation/QPlaceSearchRequest>
#include <QtLocation/QPlaceCategory>
#include <jsondb-client.h>

#include <QDebug>
#include <QString>


JsonDbHandler::JsonDbHandler(QPlaceManagerEngineJsonDb *engine)
    : QObject(engine), m_engine(engine)
{
    m_db = new JsonDbClient(m_engine);
    if (!m_db->isConnected()) {
        qWarning() << "JsonDb not connected";
    } else  {
        connect(m_db, SIGNAL(response(int, const QVariant&)),
                this, SIGNAL(jsonDbResponse(int, const QVariant&)), Qt::QueuedConnection);
        connect(m_db, SIGNAL(error(int, int, const QString&)),
                this, SIGNAL(jsonDbError(int, int, const QString&)), Qt::QueuedConnection);
        connect(m_db, SIGNAL(response(int, const QVariant&)),
                this, SLOT(processJsonDbResponse(int,QVariant)), Qt::QueuedConnection);
        connect(m_db, SIGNAL(error(int, int, const QString&)),
                this, SLOT(processJsonDbError(int,int,QString)), Qt::QueuedConnection);
    }
}

int JsonDbHandler::write(const QVariant &jsonObj)
{
    return m_db->create(jsonObj);
}

int JsonDbHandler::update(const QVariant &jsonObj)
{
    return m_db->update(jsonObj);
}

int JsonDbHandler::query(const QVariant &jsonObj)
{
    return m_db->find(jsonObj);
}

int JsonDbHandler::queryPlaceByUuid(const QString &placeUuid)
{
    return m_db->query(QString("[?%1=\"%2\"][?%3 = \"%4\"]").arg(TYPE).arg(PLACE_TYPE)
                                                            .arg(UUID).arg(placeUuid));
}

int JsonDbHandler::queryCategoryByUuid(const QString &categoryUuid)
{
    return m_db->query(QString("[?%1=\"%2\"][?%3 = \"%4\"]").arg(TYPE).arg(PLACE_CATEGORY_TYPE)
                                                            .arg(UUID).arg(categoryUuid));
}

int JsonDbHandler::removePlace(const QString &placeUuid)
{
    QVariantMap jsonMap;
    jsonMap.insert(UUID, placeUuid);
    jsonMap.insert(TYPE, PLACE_TYPE);
    return m_db->remove(jsonMap);
}

int JsonDbHandler::removeCategory(const QString &categoryUuid)
{
    QVariantMap jsonMap;
    jsonMap.insert(UUID, categoryUuid);
    jsonMap.insert(TYPE, PLACE_CATEGORY_TYPE);
    return m_db->remove(jsonMap);
}

int JsonDbHandler::query(const QString &query)
{
    return m_db->query(query);
}

QVariant JsonDbHandler::convertToJsonVariant(const QPlace &place)
{
    QVariantMap map;
    map.insert("_type", PLACE_TYPE);
    if (!place.placeId().isEmpty())
        map.insert("_uuid", place.placeId());
    map.insert(DISPLAY_NAME, place.name());

    QVariantMap coordMap;
    coordMap.insert(LATITUDE, place.location().coordinate().latitude());
    coordMap.insert(LONGITUDE, place.location().coordinate().longitude());
    coordMap.insert(ALTITUDE, place.location().coordinate().altitude());

    QVariantMap addressMap;
    addressMap.insert(STREET, place.location().address().street());
    addressMap.insert(DISTRICT, place.location().address().district());
    addressMap.insert(CITY, place.location().address().city());
    addressMap.insert(COUNTY, place.location().address().county());
    addressMap.insert(STATE, place.location().address().state());
    addressMap.insert(COUNTRY, place.location().address().country());

    map.insert(COORDINATE, coordMap);
    map.insert(ADDRESS, addressMap);

    map.insert(ICON_URL, place.icon().fullUrl().toString());

    return map;
}

QVariant JsonDbHandler::convertToJsonVariant(const QPlaceCategory &category, bool isTopLevel)
{
    QVariantMap map;
    if (!category.categoryId().isEmpty())
        map.insert(UUID, category.categoryId());

    map.insert(TYPE, PLACE_CATEGORY_TYPE);
    map.insert(DISPLAY_NAME, category.name());
    map.insert(TOP_LEVEL_CATEGORY, isTopLevel);
    map.insert(ICON_URL, category.icon().fullUrl().toString());
    return map;
}

QString JsonDbHandler::convertToQueryString(const QPlaceSearchRequest &request)
{
    QString queryString;
    if (!request.searchTerm().isEmpty()) {
        queryString += QString::fromLatin1("[?%1=\"%2\"][?%3 =~ \"/%4.*/i\"]")
                        .arg(TYPE).arg(PLACE_TYPE).arg(DISPLAY_NAME).arg(request.searchTerm());
    }

    if (queryString.isEmpty())
        queryString = QString::fromLatin1("[?%1 = \"%2\"]").arg(TYPE).arg(PLACE_TYPE);

    if (request.relevanceHint() == QPlaceSearchRequest::LexicalPlaceNameHint)
        queryString += QString::fromLatin1("[/") + DISPLAY_NAME + QLatin1String("]");

    return queryString;
}

/* Expected response format
   {
   "data": <array of json objects>
   "explantion":
   "length": <integer>
   "offset": <integer>
   }
*/
QList<QPlace> JsonDbHandler::convertJsonResponseToPlaces(const QVariant &response)
{
    QList<QVariant> data = response.toMap().value("data").toList();
    QList<QPlace> places;
    foreach (const QVariant&var, data)
        places.append(JsonDbHandler::convertJsonVariantToPlace(var));

    return places;
}

QList<QPlaceCategory> JsonDbHandler::convertJsonResponseToCategories(const QVariant &response)
{
    QList<QVariant> data = response.toMap().value("data").toList();
    QList<QPlaceCategory> categories;
    foreach (const QVariant &var, data)
        categories.append(JsonDbHandler::convertJsonVariantToCategory(var));

    return categories;
}

QPlace JsonDbHandler::convertJsonVariantToPlace(const QVariant &variant)
{
    QVariantMap placeJson = variant.toMap();

    QPlace place;
    place.setName(placeJson.value(DISPLAY_NAME).toString());
    place.setPlaceId(placeJson.value(UUID).toString());

    QVariantMap coordMap = placeJson.value(COORDINATE).toMap();
    QGeoLocation location;
    QGeoCoordinate coord;
    coord.setLatitude(coordMap.value(LATITUDE).toDouble());
    coord.setLongitude(coordMap.value(LONGITUDE).toDouble());
    coord.setAltitude(coordMap.value(ALTITUDE).toDouble());
    location.setCoordinate(coord);

    QVariantMap addressMap = placeJson.value(ADDRESS).toMap();
    QGeoAddress address;
    address.setStreet(addressMap.value(STREET).toString());
    address.setDistrict(addressMap.value(DISTRICT).toString());
    address.setCity(addressMap.value(CITY).toString());
    address.setCounty(addressMap.value(COUNTY).toString());
    address.setState(addressMap.value(STATE).toString());
    address.setCountry(addressMap.value(COUNTRY).toString());
    location.setAddress(address);

    place.setLocation(location);

    if (placeJson.keys().contains(ICON_URL) && !placeJson.value(ICON_URL).toString().isEmpty()) {
        QPlaceIcon icon;
        icon.setFullUrl(QUrl(placeJson.value(ICON_URL).toUrl()));
        icon.setManager(m_engine->manager());
        place.setIcon(icon);
    }

    return place;
}

QPlaceCategory JsonDbHandler::convertJsonVariantToCategory(const QVariant &variant)
{
    QVariantMap categoryMap = variant.toMap();
    QPlaceCategory category;
    category.setName(categoryMap.value(DISPLAY_NAME).toString());
    category.setCategoryId(categoryMap.value(UUID).toString());

    if (categoryMap.keys().contains(ICON_URL) && !categoryMap.value(ICON_URL).toString().isEmpty()) {
        QPlaceIcon icon;
        icon.setFullUrl(QUrl(categoryMap.value(ICON_URL).toUrl()));
        icon.setManager(m_engine->manager());
        category.setIcon(icon);
    }

    return category;
}

bool JsonDbHandler::isConnected()
{
    return m_db->isConnected();
}

bool JsonDbHandler::waitForRequest(int reqId, QVariantMap *variantMap)
{
    m_helperMap.insert(reqId, QVariant());
    m_eventLoop.exec(QEventLoop::AllEvents);

    QVariant response = m_helperMap.value(reqId);
    if (response.type() == QVariant::Bool) {
        if (variantMap)
            *variantMap = QVariantMap();
        return false;
    } else {
        if (variantMap)
            *variantMap = response.toMap();
        return true;
    }
}

QVariantMap JsonDbHandler::findParentCategoryJson(const QString &categoryId)
{
    QVariantMap parentMap;
    int reqId = query(QString("[?%1 = \"%2\"]").arg(TYPE).arg(PLACE_CATEGORY_TYPE));
    QVariantMap responseMap;
    if (!waitForRequest(reqId, &responseMap))
        return QVariantMap(); //TODO: correctly handle error condition
    QList<QVariant> categoriesJson = responseMap.value(QLatin1String("data")).toList();
    foreach (const QVariant &categoryJson, categoriesJson) {
        QStringList childrenUuids = categoryJson.toMap().value(CHILDREN_UUIDS).toStringList();
        if (childrenUuids.contains(categoryId)) {
            parentMap = categoryJson.toMap();
            break;
        }
    }
    return parentMap;
}

QVariantMap JsonDbHandler::findCategoryJson(const QString &categoryId)
{
    int reqId = queryCategoryByUuid(categoryId);
    QVariantMap responseMap;
    if (!waitForRequest(reqId, &responseMap))
            return QVariantMap(); //TODO: correctly handle error condition

    if (responseMap.value(QLatin1String("length")).toInt() <= 0)
        return QVariantMap();
    else
        return responseMap.value(QLatin1String("data")).toList().at(0).toMap();
}

QPlaceCategory JsonDbHandler::findCategory(const QString &categoryId)
{
    if (categoryId.isEmpty())
        return QPlaceCategory();

    int reqId = queryCategoryByUuid(categoryId);
    QVariantMap response;
    if (!waitForRequest(reqId, &response))
        return QPlaceCategory();//TODO: correctly handle error condition

    if (response.value(QLatin1String("length")).toInt() <= 0) {
        return QPlaceCategory();
    }

    return convertJsonVariantToCategory(response.value(QLatin1String("data")).toList().at(0));
}

void JsonDbHandler::processJsonDbResponse(int id, const QVariant &data)
{
    if (m_helperMap.contains(id)) {
        m_helperMap.insert(id, data);
        m_eventLoop.exit();
    }
}

void JsonDbHandler::processJsonDbError(int id, int code, const QString &jsonDbErrorString)
{
    Q_UNUSED(code)
    Q_UNUSED(jsonDbErrorString)

    if (m_helperMap.contains(id)) {
        m_helperMap.insert(id, false);
        m_eventLoop.exit();
    }
}
