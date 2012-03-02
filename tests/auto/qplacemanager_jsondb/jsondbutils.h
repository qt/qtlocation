/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the test suite of the Qt Toolkit.
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
#ifndef JSONDBUTILS_H
#define JSONDBUTILS_H

#include <QObject>
#include <QMap>
#include <QVariant>
#include <QEventLoop>
#include <QProcess>

#include <QtCore/QJsonObject>
#include <QtJsonDb/QJsonDbConnection>
#include <QtJsonDb/QJsonDbRequest>

QT_USE_NAMESPACE_JSONDB

class JsonDbUtils : public QObject
{
    Q_OBJECT
public:
    ~JsonDbUtils();
    JsonDbUtils(QObject *parent = 0);
    void cleanDb();
    void sendRequest(QJsonDbRequest *request);

    QList<QJsonObject> results();
    void fetchPlaceJson(const QString &uuid);
    void savePlaceJson(const QJsonObject &object);
    bool hasJsonDbConnection() const;

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

    static const QLatin1String NotificationType;
    static const QLatin1String Actions;

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

    static const QLatin1String Height;
    static const QLatin1String Width;

    static const QLatin1String Thumbnails;
    static const QLatin1String Small;
    static const QLatin1String Medium;
    static const QLatin1String Large;
    static const QLatin1String Fullscreen;

    static const QLatin1String SmallIconParam;
    static const QLatin1String MediumIconParam;
    static const QLatin1String LargeIconParam;
    static const QLatin1String FullscreenIconParam;

    static const QLatin1String SmallIconSizeParam;
    static const QLatin1String MediumIconSizeParam;
    static const QLatin1String LargeIconSizeParam;
    static const QLatin1String FullscreenIconSizeParam;

    static const QLatin1String CreatedDateTime;
    static const QLatin1String ModifiedDateTime;

public slots:
    void getPlacesFinished();
    void removePlacesFinished();
    void getCategoriesFinished();
    void removeCategoriesFinished();
    void requestError(QtJsonDb::QJsonDbRequest::ErrorCode error, const QString &errorString);

    void fetchPlaceJsonFinished();
    void savePlaceJsonFinished();

signals:
    void dbCleaned();
    void placeFetched(QJsonObject placeJson);
    void placeSaved();

private:
    void makeConnections(QJsonDbRequest *request, QObject *parent, const char *slot);
    static QProcess *launchJsonDbDaemon(const QStringList &args = QStringList());

    QProcess *m_jsondbProcess;
    QJsonDbConnection *m_connection;
    QList<QJsonObject> m_results;
};

#endif
