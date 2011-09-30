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

#define UUID "_uuid"
#define TYPE "_type"

#define PLACE_TYPE "place"
#define DISPLAY_NAME "displayName"
#define PLACE_CATEGORY_TYPE "com.nokia.mp.location.PlaceCategory"
#define TOP_LEVEL_CATEGORY "topLevel"
#define CHILDREN_UUIDS "childrenUuids"
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
class QPlace;
class QPlaceSearchRequest;
class QPlaceCategory;

class JsonDbHandler : public QObject
{
    Q_OBJECT
#include <QVariant>
public:
    JsonDbHandler(QPlaceManagerEngineJsonDb *manager);

    int write(const QVariant &jsonObj);
    int update(const QVariant &jsonObj);
    int query(const QVariant &jsonObj);
    int queryByUuid(const QString &uuid);
    int remove(const QString &uuid);
    int query(const QString &query);


    static QVariant convertToJsonVariant(const QPlace &place);
    static QVariant convertToJsonVariant(const QPlaceCategory &category, bool isTopLevel);
    static QString convertToQueryString(const QPlaceSearchRequest &query);

    static QList<QPlace> convertJsonResponseToPlaces(const QVariant &response);
    static QList<QPlaceCategory> convertJsonResponseToCategories(const QVariant &response);
    static QPlace convertJsonVariantToPlace(const QVariant &variant);
    static QPlaceCategory convertJsonVariantToCategory(const QVariant &variant);

    bool isConnected();

    QVariantMap waitForRequest(int reqId);
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

