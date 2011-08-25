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
#ifndef JSONHANDLER_H
#define JSONHANDLER_H

#include <QObject>
#include <QVariant>

#include <jsondb-global.h>

#define UUID "_uuid"
#define TYPE "_type"

#define PLACE_TYPE "place"
#define PLACE_NAME "name"
#define COORDINATE "coordinate"

//coord
#define COORDINATE "coordinate"
#define LATITUDE QLatin1String("latitude")
#define LONGITUDE QLatin1String("longitude")
#define ALTITUDE QLatin1String("altitude")

//address
#define ADDRESS "address"
#define STREET "street"
#define POSTCODE "postcode"
#define DISTRICT "district"
#define CITY "locality"
#define STATE "state"
#define COUNTY "county"
#define COUNTRY "country"

Q_ADDON_JSONDB_BEGIN_NAMESPACE
class JsonDbClient;
Q_ADDON_JSONDB_END_NAMESPACE
Q_USE_JSONDB_NAMESPACE

class QPlaceManagerEngineJsonDb;
class QGeoPlace;
class QPlaceSearchRequest;

class JsonDbHandler : public QObject
{
    Q_OBJECT
public:
    JsonDbHandler(QPlaceManagerEngineJsonDb *manager);

    int write(const QVariant &jsonObj);
    int update(const QVariant &jsonObj);
    int query(const QVariant &jsonObj);
    int queryPlaceDetails(const QString &placeId);
    int remove(const QString &uuid);

    static QVariant convertToJsonVariant(const QGeoPlace &place);
    static QVariant convertToJsonVariant(const QPlaceSearchRequest &query);
    static QList<QGeoPlace> convertJsonResponseToPlaces(const QVariant &response);
    static QGeoPlace convertJsonVariantToPlace(const QVariant &variant);

    bool isConnected();
signals:
    void jsonDbResponse( int id, const QVariant& data);
    void jsonDbError( int id, int code, const QString& data);

private:
    JsonDbClient *m_db;
    QPlaceManagerEngineJsonDb *m_engine;
};

#endif

