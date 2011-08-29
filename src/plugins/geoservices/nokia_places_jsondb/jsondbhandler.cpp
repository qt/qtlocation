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
** Public License version 3.0 as published by tOhe Free Software Foundation
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
#include "reply.h"
#include "savereply.h"
#include "searchreply.h"
#include "detailsreply.h"

#include <qplacesearchrequest.h>
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

int JsonDbHandler::queryPlaceDetails(const QString &placeId)
{
    return m_db->query(QString("[?_uuid = \"%1\"]").arg(placeId));
}

int JsonDbHandler::remove(const QString &uuid)
{
    QVariantMap jsonMap;
    jsonMap.insert(UUID, uuid);
    return m_db->remove(jsonMap);
}

QVariant JsonDbHandler::convertToJsonVariant(const QGeoPlace &place)
{
    QVariantMap map;
    map.insert("_type", PLACE_TYPE);
    if (!place.placeId().isEmpty())
        map.insert("_uuid", place.placeId());
    map.insert(PLACE_NAME, place.name());

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

    return map;
}

QVariant JsonDbHandler::convertToJsonVariant(const QPlaceSearchRequest &request)
{
    QVariantMap queryMap;
    QString queryString;
    if (!request.searchTerm().isEmpty())
        queryString += QString("[?name =~ \"/%1.*/i\"]").arg(request.searchTerm());

    if (queryString.isEmpty()) {
        queryString = QLatin1String("[?_type = \"place\"]");
    }
    queryMap.insert(QLatin1String("query"), queryString);
    return queryMap;
}

/* Expected response format
   {
   "data": <array of json objects>
   "explantion":
   "length": <integer>
   "offset": <integer>
   }
*/
QList<QGeoPlace> JsonDbHandler::convertJsonResponseToPlaces(const QVariant &response)
{
    QList<QVariant> data = response.toMap().value("data").toList();
    QList<QGeoPlace> places;
    foreach (const QVariant&var, data)
        places.append(JsonDbHandler::convertJsonVariantToPlace(var));

    return places;
}

QGeoPlace JsonDbHandler::convertJsonVariantToPlace(const QVariant &variant)
{
    QVariantMap placeJson = variant.toMap();

    QGeoPlace place;
    place.setName(placeJson.value(PLACE_NAME).toString());
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

    return place;
}

bool JsonDbHandler::isConnected()
{
    return m_db->isConnected();
}

