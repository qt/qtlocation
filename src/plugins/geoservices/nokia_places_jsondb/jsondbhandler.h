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
#ifndef JSONHANDLER_H
#define JSONHANDLER_H

#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtCore/QEventLoop>

#include <jsondb-global.h>

#define UUID QLatin1String("_uuid")
#define TYPE QLatin1String("_type")

#define PLACE_TYPE QLatin1String("com.nokia.mp.location.Place")
#define DISPLAY_NAME QLatin1String("displayName")
#define PLACE_CATEGORY_TYPE QLatin1String("com.nokia.mp.location.PlaceCategory")
#define TOP_LEVEL_CATEGORY QLatin1String("topLevel")
#define CHILDREN_UUIDS QLatin1String("childrenUuids")

//coord
#define COORDINATE QLatin1String("geo")
#define LATITUDE QLatin1String("latitude")
#define LONGITUDE QLatin1String("longitude")

//address
#define ADDRESS QLatin1String("address")
#define STREET QLatin1String("street")
#define DISTRICT QLatin1String("district")
#define CITY QLatin1String("locality")
#define COUNTY QLatin1String("county")
#define STATE QLatin1String("state")
#define COUNTRY QLatin1String("country")
#define POSTALCODE QLatin1String("postalCode")

#define LOCATION QLatin1String("location")

//contact details
#define PHONES QLatin1String("phones")
#define EMAILS QLatin1String("emails")
#define WEBSITES QLatin1String("urls")

#define PHONE_SUBTYPE QLatin1String("subType")
#define SUBTYPE_FAX QLatin1String("Fax")
#define SUBTYPE_LANDLINE QLatin1String("Landline")

#define LABEL QLatin1String("label")
#define VALUE QLatin1String("value")
#define URL QLatin1String("url")

#define ICON_URL QLatin1String("iconUrl")

Q_ADDON_JSONDB_BEGIN_NAMESPACE
class JsonDbClient;
Q_ADDON_JSONDB_END_NAMESPACE
Q_USE_JSONDB_NAMESPACE

class QPlaceManagerEngineJsonDb;
class QPlace;
class QPlaceSearchRequest;
class QPlaceCategory;

class JsonDbHandler : public QObject
{
    Q_OBJECT

public:
    JsonDbHandler(QPlaceManagerEngineJsonDb *manager);

    int write(const QVariant &jsonObj);
    int update(const QVariant &jsonObj);
    int query(const QVariant &jsonObj);
    int queryPlaceByUuid(const QString &placeUuid);
    int queryCategoryByUuid(const QString &categoryUuid);
    int removePlace(const QString &placeUuid);
    int removeCategory(const QString &categoryUuid);
    int query(const QString &query);


    static QVariant convertToJsonVariant(const QPlace &place);
    static QVariant convertToJsonVariant(const QPlaceCategory &category, bool isTopLevel);
    static QString convertToQueryString(const QPlaceSearchRequest &query);

    QList<QPlace> convertJsonResponseToPlaces(const QVariant &response);
    QList<QPlaceCategory> convertJsonResponseToCategories(const QVariant &response);
    QPlace convertJsonVariantToPlace(const QVariant &variant);
    QPlaceCategory convertJsonVariantToCategory(const QVariant &variant);

    bool isConnected();

    bool waitForRequest(int reqId, QVariantMap *variantMap = 0);
    QVariantMap findParentCategoryJson(const QString &categoryId);
    QVariantMap findCategoryJson(const QString &categoryId);
    QPlaceCategory findCategory(const QString &categoryId);

signals:
    void jsonDbResponse( int id, const QVariant& data);
    void jsonDbError( int id, int code, const QString& data);

private slots:
    void processJsonDbResponse(int id, const QVariant &data);
    void processJsonDbError(int id, int code, const QString &data);

private:
    JsonDbClient *m_db;
    QPlaceManagerEngineJsonDb *m_engine;
    QMap<int, QVariant> m_helperMap;
    QEventLoop m_eventLoop;
};

#endif

