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
#ifndef JSONDB_H
#define JSONDB_H

#include <QtCore/QJsonObject>
#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtJsonDb/QJsonDbRequest>
#include <QtJsonDb/QJsonDbNotification>
#include <QtJsonDb/QJsonDbConnection>

QT_BEGIN_NAMESPACE

QT_USE_NAMESPACE_JSONDB

class QPlaceManagerEngineJsonDb;
class QPlace;
class QPlaceSearchRequest;
class QPlaceCategory;
class QPlaceIcon;

class JsonDb : public QObject {
    Q_OBJECT
public:
    JsonDb(const QString &partition);
    ~JsonDb();

    static void addToJson(QJsonObject *jsonObj, const QPlaceCategory &category);
    static void addToJson(QJsonObject *jsonObj, const QPlace &place);
    static QJsonObject convertToJson(const QPlace &place);

    static QList<QPlace> convertJsonObjectsToPlaces(const QList<QJsonObject> &objects, const QPlaceManagerEngineJsonDb *engine);
    static QList<QPlaceCategory> convertJsonObjectsToCategories(const QList<QJsonObject> &objects , const QPlaceManagerEngineJsonDb *engine);
    static QPlace convertJsonObjectToPlace(const QJsonObject &placeJson, const QPlaceManagerEngineJsonDb *engine);
    static QPlaceCategory convertJsonObjectToCategory(const QJsonObject &object, const QPlaceManagerEngineJsonDb *engine);
    static QPlaceIcon convertJsonObjectToIcon(const QJsonObject &thumbnailsJson, const QPlaceManagerEngineJsonDb *engine,
                                              const QStringList &nokiaCategoryIds = QStringList());

    void getCategory(const QString &uuid, QObject *parent, const char *slot);
    void getCategories(const QList<QPlaceCategory> &categories, QObject *parent, const char *slot);
    void getCategories(const QStringList &uuids, QObject *parent, const char *slot);
    void getChildCategories(const QString &uuid, QObject *parent, const char *slot);
    void getPlace(const QString &uuid,QObject *parent, const char * slot);
    void findPlacesByAlternativeId(const QString exernalIdName, const QStringList &alternativeIds, QObject *parent, const char *slot);
    void findAllPlaces(QObject *parent, const char *slot);

    void read(const QString &query, QObject *parent, const char *slot);
    void write(const QJsonObject &jsonObject, QObject *parent, const char *slot);
    void write(const QList<QJsonObject> &jsonObjects, QObject *parent, const char *slot);
    void remove(const QJsonObject &jsonObject, QObject *parent, const char *slot);
    void remove(const QList<QJsonObject> &jsonObjects, QObject *parent, const char *slot);
    void searchForPlaces(const QPlaceSearchRequest &request, QObject *parent, const char *slot);

    void setupRequest(QJsonDbRequest *request, QObject *parent, const char *slot);

    static QString query(const QString &expression);

    bool parseIconMapping(const QString &fileName);

    static QStringList categoryIds(QList<QPlace> places);

    static const QLatin1String Uuid;
    static const QLatin1String Type;
    static const QLatin1String Query;
    static const QLatin1String Bindings;

    static const QLatin1String Length;
    static const QLatin1String Data;

    static const QLatin1String Name;
    static const QLatin1String PlaceType;
    static const QLatin1String CategoryUuids; //only categories that are directly assigned to a place
    static const QLatin1String AllCategoryUuids; //all categories that a place belongs to,
                                                             //it includes all ancestors of the assigned categories

    static const QLatin1String CategoryType;
    static const QLatin1String Lineage;  //includes all ancestor category ids and also the current category id
                                                  //as the last element.   The first category is a top level category id.
    static const QLatin1String CategoryParentId;

    //coord
    static const QLatin1String Coordinate;
    static const QLatin1String Latitude;
    static const QLatin1String Longitude;

    //address
    static const QLatin1String Address;
    static const QLatin1String Street;
    static const QLatin1String District;
    static const QLatin1String City;
    static const QLatin1String County;
    static const QLatin1String State;
    static const QLatin1String Country;
    static const QLatin1String CountryCode;
    static const QLatin1String PostalCode;

    static const QLatin1String Location;

    //contact details
    static const QLatin1String Phones;
    static const QLatin1String Emails;
    static const QLatin1String Websites;

    static const QLatin1String PhoneSubType;
    static const QLatin1String SubTypeFax;
    static const QLatin1String SubTypeLandline;

    static const QLatin1String Label;
    static const QLatin1String Value;
    static const QLatin1String Url;

    static const QLatin1String ExtendedAttributes;
    static const QLatin1String Text;
    static const QLatin1String NokiaCategoryIds;

    static const QLatin1String Height;
    static const QLatin1String Width;

    static const QLatin1String Thumbnails;
    static const QLatin1String Small;
    static const QLatin1String Medium;
    static const QLatin1String Large;
    static const QLatin1String Fullscreen;
    static const QLatin1String NokiaIcon;

    static const QLatin1String VisibilityScope;
    static const QLatin1String DeviceVisibility;

    static const QLatin1String CreatedDateTime;
    static const QLatin1String ModifiedDateTime;

signals:
    void placeNotifications(QList<QJsonDbNotification> notifications);
    void categoryNotifications(QList<QJsonDbNotification> notifications);
    void notificationsError(QtJsonDb::QJsonDbWatcher::ErrorCode code, const QString &errorString);

private slots:
    void processPlaceNotifications();
    void processCategoryNotifications();

private:
    QtJsonDb::QJsonDbConnection *m_connection;
    QJsonDbWatcher *m_placeWatcher;
    QJsonDbWatcher *m_categoryWatcher;
    QString m_partition;
    QHash<QString, QString> m_restIdToIconHash;
};

QT_END_NAMESPACE

#endif

