/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
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
#include <QLocalSocket>
#include <QDir>
#include <QTest>
#include <QSignalSpy>
#include <QUuid>

#include <sys/types.h>
#include <unistd.h>

#ifndef WAIT_UNTIL
#define WAIT_UNTIL(__expr) \
        do { \
        const int __step = 50; \
        const int __timeout = 10000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            QTest::qWait(__step); \
        } \
    } while (0)
#endif

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
//as the last object.   The first category is a top level category id.
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

const QLatin1String JsonDbUtils::DefaultPartition("");
const QLatin1String JsonDbUtils::PartitionType("Partition");

const QLatin1String JsonDbUtils::LongitudeIndex("placeLongitudeIndex");
const QLatin1String JsonDbUtils::LatitudeIndex("placeLatitudeIndex");
const QLatin1String JsonDbUtils::PlaceNameIndex("placeDisplayNameIndex");

JsonDbUtils::JsonDbUtils(QObject *parent)
    : QObject(parent)
{
    QStringList args;
    args <<"-reject-stale-updates";
    m_jsondbProcess = launchJsonDbDaemon(args);
    m_connection = new QJsonDbConnection;
    m_connection->connectToServer();
}

JsonDbUtils::~JsonDbUtils()
{
    if (m_jsondbProcess)
        m_jsondbProcess->kill();
}

void JsonDbUtils::cleanDb()
{
    QJsonDbReadRequest *getPlacesRequest = new QJsonDbReadRequest(this);
    getPlacesRequest->setPartition(m_currentPartition);
    getPlacesRequest->setQuery(QString::fromLatin1("[?%1=\"%2\"]")
                               .arg(Type).arg(PlaceType));

    setupRequest(getPlacesRequest, this, SLOT(getPlacesFinished()));
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
    request->setPartition(m_currentPartition);
    request->setQuery(QStringLiteral("[?_type=%type][?_uuid=%uuid]"));
    request->bindValue(QStringLiteral("type"), PlaceType);
    request->bindValue(QStringLiteral("uuid"), uuid);
    setupRequest(request, this, SLOT(fetchPlaceJsonFinished()));
    m_connection->send(request);
}

void JsonDbUtils::savePlaceJson(const QJsonObject &object)
{
    QJsonDbWriteRequest *request = new QJsonDbWriteRequest(this);
    request->setPartition(m_currentPartition);
    QList<QJsonObject> objects;
    objects << object;
    request->setObjects(objects);
    setupRequest(request, this, SLOT(savePlaceJsonFinished()));
    m_connection->send(request);
}

bool JsonDbUtils::hasJsonDbConnection() const
{
    return  m_jsondbProcess &&
            m_connection &&
            m_connection->status() == QtJsonDb::QJsonDbConnection::Connected;
}

void JsonDbUtils::setCurrentPartition(const QString &partition)
{
    m_currentPartition = partition;
}

void JsonDbUtils::setupPartition(const QString &partition)
{
    QJsonDbReadRequest *readPartitionRequest = new QJsonDbReadRequest();
    readPartitionRequest->setQuery(QStringLiteral("[?_type=%type][?name=%name]"));
    readPartitionRequest->bindValue(QStringLiteral("type"), JsonDbUtils::PartitionType);
    readPartitionRequest->bindValue(QStringLiteral("name"), partition);
    QSignalSpy readSpy(readPartitionRequest, SIGNAL(finished()));
    sendRequest(readPartitionRequest);
    WAIT_UNTIL(readSpy.count() == 1);
    if (readSpy.isEmpty()) {
        qWarning() << "No finished signal emitted when trying to read partition: " << partition;
        return;
    }

    QList<QJsonObject> results = readPartitionRequest->takeResults();
    if (results.isEmpty()) {
        //create user partition because it doesn't exist
        QJsonObject userPartition;
        userPartition.insert(JsonDbUtils::Uuid, QUuid::createUuid().toString());
        userPartition.insert(JsonDbUtils::Type, JsonDbUtils::PartitionType);
        userPartition.insert(QLatin1String("name"), partition);

        QJsonDbWriteRequest *writePartitionRequest = new QJsonDbWriteRequest(this);
        QList<QJsonObject> objects;
        objects << userPartition;
        writePartitionRequest->setObjects(objects);

        QSignalSpy writeSpy(writePartitionRequest, SIGNAL(finished()));
        sendRequest(writePartitionRequest);
        WAIT_UNTIL(writeSpy.count() == 1);
        if (writeSpy.isEmpty()) {
            qWarning() << "No finished signal emitted when trying to create partition: " << partition;
            return;
        }
    }

    QMetaObject::invokeMethod(this, "partitionSetupDone", Qt::QueuedConnection);
}

void JsonDbUtils::getPlacesFinished()
{
    QList<QJsonObject> results = qobject_cast<QJsonDbReadRequest *>(sender())
                                    ->takeResults();

    if (!results.isEmpty()) {
        QJsonDbRemoveRequest *removePlacesRequest
                = new QJsonDbRemoveRequest(results, this);
        setupRequest(removePlacesRequest,this, SLOT(removePlacesFinished()));
        m_connection->send(removePlacesRequest);
    } else {
        removePlacesFinished();
    }
}

void JsonDbUtils::removePlacesFinished()
{
    QJsonDbReadRequest *getCategoriesRequest = new QJsonDbReadRequest(this);
    getCategoriesRequest->setPartition(m_currentPartition);
    getCategoriesRequest->setQuery(QString::fromLatin1("[?%1=\"%2\"]")
                                   .arg(Type).arg(CategoryType));
    setupRequest(getCategoriesRequest,this, SLOT(getCategoriesFinished()));
    m_connection->send(getCategoriesRequest);
}

void JsonDbUtils::getCategoriesFinished()
{
    QList<QJsonObject> results = qobject_cast<QJsonDbReadRequest *>(sender())
                                    ->takeResults();
    if (!results.isEmpty()) {
        QJsonDbRemoveRequest *removeCategoriesRequest
                = new QJsonDbRemoveRequest(results, this);
        setupRequest(removeCategoriesRequest, this, SLOT(removeCategoriesFinished()));
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
    QList<QJsonObject> results = request->takeResults();
    if (results.count() == 1)
        emit placeFetched(results.first());
    else
        emit placeFetched(QJsonObject());
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

void JsonDbUtils::setupRequest(QJsonDbRequest *request, QObject *parent,
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
    request->setPartition(m_currentPartition);
}

QProcess* JsonDbUtils::launchJsonDbDaemon(const QStringList &args)
{
    const QString socketName = QString("tst_qplacemanger_jsondb_%1").arg(getpid());
    QString jsondb_app = QString::fromLocal8Bit(JSONDB_DAEMON_BASE) + QDir::separator() + "jsondb";
    if (!QFile::exists(jsondb_app))
        jsondb_app = QLatin1String("jsondb"); // rely on the PATH

    QProcess *process = new QProcess;
    process->setProcessChannelMode( QProcess::ForwardedChannels );

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("JSONDB_SOCKET", socketName);
    process->setProcessEnvironment(env);
    ::setenv("JSONDB_SOCKET", qPrintable(socketName), 1);
    qDebug() << "Starting process" << jsondb_app << args << "with socket" << socketName;
    process->start(jsondb_app, args);
    if (!process->waitForStarted())
        qFatal("Unable to start jsondb database process");

    /* Wait until the jsondb is accepting connections */
    int tries = 0;
    bool connected = false;
    while (!connected && tries++ < 100) {
        QLocalSocket socket;
        socket.connectToServer(socketName);
        if (socket.waitForConnected())
            connected = true;
        QTest::qWait(250);
    }

    if (!connected)
        qFatal("Unable to connect to jsondb process");

    //Start jsondb-client and get it to load a prepopulation file that contains
    //Indexes
    QProcess *prepopulateProcess = new QProcess;
    prepopulateProcess->setProcessEnvironment(env);

    QString jsondb_client = QString::fromLocal8Bit(JSONDB_DAEMON_BASE)
                            + QDir::separator() + "jsondb-client";
    if (!QFile::exists(jsondb_client))
        jsondb_client = QLatin1String("jsondb-client"); // rely on the PATH
    QStringList clientArgs;
    clientArgs << QLatin1String("-load");
    clientArgs << QLatin1String(JSONFILE);
    clientArgs << QLatin1String("-terminate");
    prepopulateProcess->setProcessChannelMode( QProcess::ForwardedChannels);
    prepopulateProcess->start(jsondb_client, clientArgs);

    if (!prepopulateProcess->waitForFinished())
        qFatal("Unable to complete loading index via jsondb-client");

    return process;
}
