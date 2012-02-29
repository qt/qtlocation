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

#include "jsondbutils.h"

#include <QtJsonDb/QJsonDbReadRequest>
#include <QtJsonDb/QJsonDbRemoveRequest>
#include <QtJsonDb/QJsonDbWriteRequest>

#include <QDebug>
#include <QMap>
#include <QString>
#include <QStringList>

const QLatin1String JsonDbUtils::Uuid("_uuid");
const QLatin1String JsonDbUtils::Type("_type");

const QLatin1String JsonDbUtils::Name("displayName");
const QLatin1String JsonDbUtils::PlaceType("com.nokia.mt.location.Place");

//only categories that are directly assigned to a place
const QLatin1String JsonDbUtils::CategoryUuids("categoryUuids");

//all categories that a place belongs to,
//it includes all ancestors of the assigned categories
const QLatin1String JsonDbUtils::AllCategoryUuids("allCategoryUuids");

const QLatin1String JsonDbUtils::CategoryType("com.nokia.mt.location.PlaceCategory");

//includes all ancestor category ids and also the current category id
//as the last element.   The first category is a top level category id.
const QLatin1String JsonDbUtils::Lineage("lineageUuids");

const QLatin1String JsonDbUtils::CategoryParentId("parentUuid");

const QLatin1String JsonDbUtils::NotificationType("notification");
const QLatin1String JsonDbUtils::Actions("actions");

//coord
const QLatin1String JsonDbUtils::Coordinate("geo");
const QLatin1String JsonDbUtils::Latitude("latitude");
const QLatin1String JsonDbUtils::Longitude("longitude");

//address
const QLatin1String JsonDbUtils::Address("address");
const QLatin1String JsonDbUtils::Street("street");
const QLatin1String JsonDbUtils::District("district");
const QLatin1String JsonDbUtils::City("locality");
const QLatin1String JsonDbUtils::County("county");
const QLatin1String JsonDbUtils::State("region");
const QLatin1String JsonDbUtils::Country("country");
const QLatin1String JsonDbUtils::CountryCode("countryCode");
const QLatin1String JsonDbUtils::PostalCode("postalCode");

const QLatin1String JsonDbUtils::Location("location");

//contact details
const QLatin1String JsonDbUtils::Phones("phones");
const QLatin1String JsonDbUtils::Emails("emails");
const QLatin1String JsonDbUtils::Websites("urls");

const QLatin1String JsonDbUtils::PhoneSubType("subType");
const QLatin1String JsonDbUtils::SubTypeFax("fax");
const QLatin1String JsonDbUtils::SubTypeLandline("landline");

const QLatin1String JsonDbUtils::Label("label");
const QLatin1String JsonDbUtils::Value("value");
const QLatin1String JsonDbUtils::Url("url");

const QLatin1String JsonDbUtils::ExtendedAttributes("extendedAttributes");
const QLatin1String JsonDbUtils::Text("text");

const QLatin1String JsonDbUtils::Height("height");
const QLatin1String JsonDbUtils::Width("width");

const QLatin1String JsonDbUtils::Thumbnails("thumbnails");
const QLatin1String JsonDbUtils::Small("small");
const QLatin1String JsonDbUtils::Medium("medium");
const QLatin1String JsonDbUtils::Large("large");
const QLatin1String JsonDbUtils::Fullscreen("fullscreen");

const QLatin1String JsonDbUtils::SmallIconParam("smallUrl");
const QLatin1String JsonDbUtils::MediumIconParam("mediumUrl");
const QLatin1String JsonDbUtils::LargeIconParam("largeUrl");
const QLatin1String JsonDbUtils::FullscreenIconParam("fullscreenUrl");

const QLatin1String JsonDbUtils::SmallIconSizeParam("smallSize");
const QLatin1String JsonDbUtils::MediumIconSizeParam("mediumSize");
const QLatin1String JsonDbUtils::LargeIconSizeParam("largeSize");
const QLatin1String JsonDbUtils::FullscreenIconSizeParam("fullscreenSize");

const QLatin1String JsonDbUtils::CreatedDateTime("createdDateTime");
const QLatin1String JsonDbUtils::ModifiedDateTime("modifiedDateTime");

JsonDbUtils::JsonDbUtils(QObject *parent)
    : QObject(parent)
{
    m_connection = new QJsonDbConnection;
    m_connection->connectToServer();
}

void JsonDbUtils::cleanDb()
{
    QJsonDbReadRequest *getPlacesRequest = new QJsonDbReadRequest(this);
    getPlacesRequest->setQuery(QString::fromLatin1("[?%1=\"%2\"]")
                               .arg(Type).arg(PlaceType));
    connect(getPlacesRequest, SIGNAL(finished()), this, SLOT(getPlacesFinished()));
    connect(getPlacesRequest, SIGNAL(finished()), getPlacesRequest, SLOT(deleteLater()));
    connect(getPlacesRequest, SIGNAL(error(QtJsonDb::QJsonDbRequest::ErrorCode,QString)),
            this, SLOT(requestError(QtJsonDb::QJsonDbRequest::ErrorCode,QString)));
    connect(getPlacesRequest, SIGNAL(error(QtJsonDb::QJsonDbRequest::ErrorCode,QString)),
            getPlacesRequest, SLOT(deleteLater()));
    m_connection->send(getPlacesRequest);
}

void JsonDbUtils::sendRequest(QJsonDbRequest *request)
{
    connect(request, SIGNAL(error(QtJsonDb::QJsonDbRequest::ErrorCode,QString)),
            this, SLOT(requestError(QtJsonDb::QJsonDbRequest::ErrorCode,QString)));
    m_connection->send(request);
}

QList<QJsonObject> JsonDbUtils::results()
{
    return m_results;
}

void JsonDbUtils::fetchPlaceJson(const QString &uuid)
{
    QJsonDbReadRequest *request = new QJsonDbReadRequest(this);
    request->setQuery(QStringLiteral("[?_type=%type][?_uuid=%uuid]"));
    request->bindValue(QStringLiteral("type"), PlaceType);
    request->bindValue(QStringLiteral("uuid"), uuid);
    makeConnections(request, this, SLOT(fetchPlaceJsonFinished()));
    m_connection->send(request);
}

void JsonDbUtils::savePlaceJson(const QJsonObject &object)
{
    QJsonDbWriteRequest *request = new QJsonDbWriteRequest(this);
    QList<QJsonObject> objects;
    objects << object;
    request->setObjects(objects);
    makeConnections(request, this, SLOT(savePlaceJsonFinished()));
    m_connection->send(request);
}

void JsonDbUtils::getPlacesFinished()
{
    QList<QJsonObject> results = qobject_cast<QJsonDbReadRequest *>(sender())
                                    ->takeResults();

    if (!results.isEmpty()) {
        QJsonDbRemoveRequest *removePlacesRequest
                = new QJsonDbRemoveRequest(results, this);
        connect(removePlacesRequest, SIGNAL(finished()),
                this, SLOT(removePlacesFinished()));
        connect(removePlacesRequest, SIGNAL(finished()),
                removePlacesRequest, SLOT(deleteLater()));
        connect(removePlacesRequest,
                SIGNAL(error(QtJsonDb::QJsonDbRequest::ErrorCode,QString)),
                this, SLOT(requestError(QtJsonDb::QJsonDbRequest::ErrorCode,QString)));
        connect(removePlacesRequest,
                SIGNAL(error(QtJsonDb::QJsonDbRequest::ErrorCode,QString)),
                removePlacesRequest, SLOT(deleteLater()));
        m_connection->send(removePlacesRequest);
    } else {
        removePlacesFinished();
    }
}

void JsonDbUtils::removePlacesFinished()
{
    QJsonDbReadRequest *getCategoriesRequest = new QJsonDbReadRequest(this);
    getCategoriesRequest->setQuery(QString::fromLatin1("[?%1=\"%2\"]")
                                   .arg(Type).arg(CategoryType));
    connect(getCategoriesRequest, SIGNAL(finished()),
            this, SLOT(getCategoriesFinished()));
    connect(getCategoriesRequest, SIGNAL(finished()),
            getCategoriesRequest, SLOT(deleteLater()));
    connect(getCategoriesRequest,
            SIGNAL(error(QtJsonDb::QJsonDbRequest::ErrorCode,QString)),
            this, SLOT(requestError(QtJsonDb::QJsonDbRequest::ErrorCode,QString)));
    connect(getCategoriesRequest,
            SIGNAL(error(QtJsonDb::QJsonDbRequest::ErrorCode,QString)),
            getCategoriesRequest, SLOT(deleteLater()));
    m_connection->send(getCategoriesRequest);
}

void JsonDbUtils::getCategoriesFinished()
{
    QList<QJsonObject> results = qobject_cast<QJsonDbReadRequest *>(sender())
                                    ->takeResults();
    if (!results.isEmpty()) {
        QJsonDbRemoveRequest *removeCategoriesRequest
                = new QJsonDbRemoveRequest(results, this);
        connect(removeCategoriesRequest, SIGNAL(finished()),
                this, SLOT(removeCategoriesFinished()));
        connect(removeCategoriesRequest, SIGNAL(finished()),
                removeCategoriesRequest, SLOT(deleteLater()));
        connect(removeCategoriesRequest,
                SIGNAL(error(QtJsonDb::QJsonDbRequest::ErrorCode,QString)),
                this, SLOT(requestError(QtJsonDb::QJsonDbRequest::ErrorCode,QString)));
        connect(removeCategoriesRequest,
                SIGNAL(error(QtJsonDb::QJsonDbRequest::ErrorCode,QString)),
                removeCategoriesRequest, SLOT(deleteLater()));
        m_connection->send(removeCategoriesRequest);
    } else {
        removeCategoriesFinished();
    }
}

void JsonDbUtils::removeCategoriesFinished()
{
    emit dbCleaned();
}

void JsonDbUtils::fetchPlaceJsonFinished()
{
    QJsonDbRequest *request = qobject_cast<QJsonDbRequest *>(sender());
    emit placeFetched(request->takeResults().first());
}

void JsonDbUtils::savePlaceJsonFinished()
{
    emit placeSaved();
}

void JsonDbUtils::requestError(QJsonDbRequest::ErrorCode error,
                               const QString &errorString)
{
    qWarning() << Q_FUNC_INFO << QStringLiteral(" Error code: ") << error
             << QStringLiteral(" Error String: ") << errorString;
}

void JsonDbUtils::makeConnections(QJsonDbRequest *request, QObject *parent,
                                  const char *slot)
{
    Q_ASSERT(request);
    Q_ASSERT(parent);
    Q_ASSERT(slot);

    QObject::connect(request, SIGNAL(finished()), parent, slot);

    QObject::connect(request, SIGNAL(finished()), request, SLOT(deleteLater()));
    QObject::connect(request,
                     SIGNAL(error(QtJsonDb::QJsonDbRequest::ErrorCode,QString)),
                     parent,
                     SLOT(requestError(QtJsonDb::QJsonDbRequest::ErrorCode,QString)));
    QObject::connect(request,
                     SIGNAL(error(QtJsonDb::QJsonDbRequest::ErrorCode,QString)),
                     request,
                     SLOT(deleteLater()));
}
