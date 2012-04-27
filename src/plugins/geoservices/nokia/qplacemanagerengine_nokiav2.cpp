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
** This file is part of the Ovi services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file OVI_SERVICES_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Ovi services
** plugin source code.
**
****************************************************************************/

#include "qplacemanagerengine_nokiav2.h"

#include "placesv2/qplacecategoriesreplyimpl.h"
#include "placesv2/qplacecontentreplyimpl.h"
#include "placesv2/qplacesearchsuggestionreplyimpl.h"
#include "placesv2/qplacesearchreplyimpl.h"
#include "placesv2/qplacerecommendationreplyimpl.h"
#include "placesv2/qplacedetailsreplyimpl.h"
#include "placesv2/qplaceidreplyimpl.h"
#include "qgeonetworkaccessmanager.h"

#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QStandardPaths>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkProxyFactory>
#ifdef USE_CHINA_NETWORK_REGISTRATION
#include <QtSystemInfo/QNetworkInfo>
#endif
#include <QtLocation/QPlaceContentRequest>
#include <QtLocation/QGeoBoundingCircle>

QT_BEGIN_NAMESPACE

static const char FIXED_CATEGORIES_string[] =
    "eat-drink\0"
    "going-out\0"
    "sights-museums\0"
    "transport\0"
    "accommodation\0"
    "shopping\0"
    "leisure-outdoor\0"
    "administrative-areas-buildings\0"
    "natural-geographical\0"
    "\0";

static const int FIXED_CATEGORIES_indices[] = {
       0,   10,   20,   35,   45,   59,   68,   84,
     115,   -1
};

static const char * const placesServerInternational = "http://api.places.lbs.maps.nokia.com/places";
static const char * const placesServerChina = "http://api.places.lbs.maps.nokia.com.cn/places";

QPlaceManagerEngineNokiaV2::QPlaceManagerEngineNokiaV2(
    QGeoNetworkAccessManager* networkManager,
    const QMap<QString, QVariant> &parameters,
    QGeoServiceProvider::Error *error,
    QString *errorString)
:   QPlaceManagerEngine(parameters), m_manager(networkManager)
{
    Q_ASSERT(networkManager);
    m_manager->setParent(this);

    m_locales.append(QLocale());

    // Unless specified in the plugin parameters set the international places server to the builtin
    // one.  This is the server used when not in China.
    m_host = parameters.value(QLatin1String("places.host"),
                              QLatin1String(placesServerInternational)).toString();

    m_appId = parameters.value(QLatin1String("app_id")).toString();
    m_appCode = parameters.value(QLatin1String("token")).toString();

#ifdef USE_CHINA_NETWORK_REGISTRATION
    m_networkInfo = new QNetworkInfo(this);
    connect(m_networkInfo, SIGNAL(currentMobileCountryCodeChanged(int,QString)),
            this, SLOT(currentMobileCountryCodeChanged(int,QString)));
    currentMobileCountryCodeChanged(0, m_networkInfo->currentMobileCountryCode(0));
#else
    m_placesServer = m_host;
#endif

    m_theme = parameters.value("places.theme", QString()).toString();

    if (m_theme == QLatin1String("default"))
        m_theme.clear();

    m_localDataPath = parameters.value(QLatin1String("local_data_path"), QString()).toString();
    if (m_localDataPath.isEmpty()) {
        QStringList dataLocations = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);

        if (!dataLocations.isEmpty() && !dataLocations.first().isEmpty()) {
            m_localDataPath = dataLocations.first()
                                + QLatin1String("/nokia/qtlocation/data");
        }
    }

    if (error)
        *error = QGeoServiceProvider::NoError;

    if (errorString)
        errorString->clear();
}

QPlaceManagerEngineNokiaV2::~QPlaceManagerEngineNokiaV2() {}

QPlaceDetailsReply *QPlaceManagerEngineNokiaV2::getPlaceDetails(const QString &placeId)
{
    QUrl requestUrl(m_placesServer + QLatin1String("/v1/places/") + placeId);

    QList<QPair<QString, QString> > queryItems;

    queryItems.append(qMakePair<QString, QString>(QLatin1String("tf"), QLatin1String("html")));
    //queryItems.append(qMakePair<QString, QString>(QLatin1String("size"), QString::number(5)));
    //queryItems.append(qMakePair<QString, QString>(QLatin1String("image_dimensions"), QLatin1String("w64-h64,w100")));

    requestUrl.setQueryItems(queryItems);

    QNetworkReply *networkReply = sendRequest(requestUrl);

    QPlaceDetailsReplyImpl *reply = new QPlaceDetailsReplyImpl(networkReply, this);
    connect(reply, SIGNAL(finished()), this, SLOT(replyFinished()));
    connect(reply, SIGNAL(error(QPlaceReply::Error,QString)),
            this, SLOT(replyError(QPlaceReply::Error,QString)));

    return reply;
}

QPlaceContentReply *QPlaceManagerEngineNokiaV2::getPlaceContent(const QString &placeId,
                                                              const QPlaceContentRequest &request)
{
    QUrl requestUrl(m_placesServer + QLatin1String("/v1/places/") + placeId + QLatin1String("/media/"));

    QNetworkReply *networkReply = 0;

    QList<QPair<QString, QString> > queryItems;

    switch (request.contentType()) {
    case QPlaceContent::ImageType:
        requestUrl.setPath(requestUrl.path() + QLatin1String("images"));

        queryItems.append(qMakePair<QString, QString>(QLatin1String("tf"), QLatin1String("html")));

        if (request.limit() > 0) {
            queryItems.append(qMakePair<QString, QString>(QLatin1String("size"),
                                                          QString::number(request.limit())));
        }
        if (request.offset() > -1) {
            queryItems.append(qMakePair<QString, QString>(QLatin1String("offset"),
                                                          QString::number(request.offset())));
        }

        //queryItems.append(qMakePair<QString, QString>(QLatin1String("image_dimensions"), QLatin1String("w64-h64,w100")));

        requestUrl.setQueryItems(queryItems);

        networkReply = sendRequest(requestUrl);
        break;
    case QPlaceContent::ReviewType:
        requestUrl.setPath(requestUrl.path() + QLatin1String("reviews"));

        queryItems.append(qMakePair<QString, QString>(QLatin1String("tf"), QLatin1String("html")));

        if (request.limit() > 0) {
            queryItems.append(qMakePair<QString, QString>(QLatin1String("size"),
                                                          QString::number(request.limit())));
        }
        if (request.offset() > -1) {
            queryItems.append(qMakePair<QString, QString>(QLatin1String("offset"),
                                                          QString::number(request.offset())));
        }

        requestUrl.setQueryItems(queryItems);

        networkReply = sendRequest(requestUrl);
        break;
    case QPlaceContent::EditorialType:
        requestUrl.setPath(requestUrl.path() + QLatin1String("editorials"));

        queryItems.append(qMakePair<QString, QString>(QLatin1String("tf"), QLatin1String("html")));

        if (request.limit() > 0) {
            queryItems.append(qMakePair<QString, QString>(QLatin1String("size"),
                                                          QString::number(request.limit())));
        }
        if (request.offset() > -1) {
            queryItems.append(qMakePair<QString, QString>(QLatin1String("offset"),
                                                          QString::number(request.offset())));
        }

        requestUrl.setQueryItems(queryItems);

        networkReply = sendRequest(requestUrl);
        break;
    case QPlaceContent::NoType:
        ;
    }

    QPlaceContentReply *reply = new QPlaceContentReplyImpl(request, networkReply, this);
    connect(reply, SIGNAL(finished()), this, SLOT(replyFinished()));
    connect(reply, SIGNAL(error(QPlaceReply::Error,QString)),
            this, SLOT(replyError(QPlaceReply::Error,QString)));

    if (!networkReply) {
        QMetaObject::invokeMethod(reply, "setError", Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply::Error, QPlaceReply::UnsupportedError),
                                  Q_ARG(QString, QString("Retrieval of given content type not supported.")));
    }

    return reply;
}

static void addAtForBoundingArea(const QGeoBoundingArea &area,
                                 QList<QPair<QString, QString> > *queryItems)
{
    QGeoCoordinate center;
    switch (area.type()) {
    case QGeoBoundingArea::BoxType:
        center = QGeoBoundingBox(area).center();
        break;
    case QGeoBoundingArea::CircleType:
        center = QGeoBoundingCircle(area).center();
        break;
    case QGeoBoundingArea::UnknownType:
        break;
    }

    if (!center.isValid())
        return;

    queryItems->append(qMakePair<QString, QString>(QLatin1String("at"),
                                                   QString::number(center.latitude()) +
                                                   QLatin1Char(',') +
                                                   QString::number(center.longitude())));
}

QPlaceSearchReply *QPlaceManagerEngineNokiaV2::search(const QPlaceSearchRequest &query)
{
    bool unsupported = false;

    unsupported |= query.visibilityScope() != QtLocation::UnspecifiedVisibility &&
                   query.visibilityScope() != QtLocation::PublicVisibility;

    unsupported |= !query.searchTerm().isEmpty() && query.offset() > 0;

    // Both a search term and search categories are not supported.
    unsupported |= !query.searchTerm().isEmpty() && !query.categories().isEmpty();

    if (unsupported) {
        QPlaceSearchReplyImpl *reply = new QPlaceSearchReplyImpl(0, this);
        QMetaObject::invokeMethod(reply, "setError", Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply::Error, QPlaceReply::BadArgumentError),
                                  Q_ARG(QString, "Unsupported search request options specified."));

        return reply;
    }

    if (!query.searchTerm().isEmpty()) {
        // search term query
        QUrl requestUrl(m_placesServer + QLatin1String("/v1/discover/search"));

        QList<QPair<QString, QString> > queryItems;

        queryItems.append(qMakePair<QString, QString>(QLatin1String("q"), query.searchTerm()));

        addAtForBoundingArea(query.searchArea(), &queryItems);

        queryItems.append(qMakePair<QString, QString>(QLatin1String("tf"), QLatin1String("html")));

        if (query.limit() > 0) {
            queryItems.append(qMakePair<QString, QString>(QLatin1String("size"),
                                                          QString::number(query.limit())));
        }

        requestUrl.setQueryItems(queryItems);

        QNetworkReply *networkReply = sendRequest(requestUrl);

        QPlaceSearchReplyImpl *reply = new QPlaceSearchReplyImpl(networkReply, this);
        connect(reply, SIGNAL(finished()), this, SLOT(replyFinished()));
        connect(reply, SIGNAL(error(QPlaceReply::Error,QString)),
                this, SLOT(replyError(QPlaceReply::Error,QString)));

        return reply;
    } else {
        // category search
        // The request URL should be "/v1/discover/explore" but that returns both places and
        // clusters of places.  We don't support clusters so we use the undocumented
        // "/v1/discover/explore/places" instead which only returns places.
        QUrl requestUrl(m_placesServer + QLatin1String("/v1/discover/explore/places"));

        QList<QPair<QString, QString> > queryItems;

        QStringList ids;
        foreach (const QPlaceCategory &category, query.categories())
            ids.append(category.categoryId());

        if (ids.count() > 0) {
            queryItems.append(qMakePair<QString, QString>(QLatin1String("cat"),
                                                          ids.join(QLatin1String(","))));
        }

        addAtForBoundingArea(query.searchArea(), &queryItems);

        queryItems.append(qMakePair<QString, QString>(QLatin1String("tf"), QLatin1String("html")));

        if (query.limit() > 0) {
            queryItems.append(qMakePair<QString, QString>(QLatin1String("size"),
                                                          QString::number(query.limit())));
        }
        if (query.offset() > -1) {
            queryItems.append(qMakePair<QString, QString>(QLatin1String("offset"),
                                                          QString::number(query.offset())));
        }

        requestUrl.setQueryItems(queryItems);

        QNetworkReply *networkReply = sendRequest(requestUrl);

        QPlaceSearchReplyImpl *reply = new QPlaceSearchReplyImpl(networkReply, this);
        connect(reply, SIGNAL(finished()), this, SLOT(replyFinished()));
        connect(reply, SIGNAL(error(QPlaceReply::Error,QString)),
                this, SLOT(replyError(QPlaceReply::Error,QString)));

        return reply;
    }
}

QPlaceSearchReply *QPlaceManagerEngineNokiaV2::recommendations(const QString &placeId, const QPlaceSearchRequest &query)
{
    QUrl requestUrl(m_placesServer + QLatin1String("/v1/places/") + placeId + QLatin1String("/related/recommended"));

    QList<QPair<QString, QString> > queryItems;

    queryItems.append(qMakePair<QString, QString>(QLatin1String("tf"), QLatin1String("html")));

    if (query.limit() > 0) {
        queryItems.append(qMakePair<QString, QString>(QLatin1String("size"),
                                                      QString::number(query.limit())));
    }
    if (query.offset() > -1) {
        queryItems.append(qMakePair<QString, QString>(QLatin1String("offset"),
                                                      QString::number(query.offset())));
    }

    requestUrl.setQueryItems(queryItems);

    QNetworkReply *networkReply = sendRequest(requestUrl);

    QPlaceRecommendationReplyImpl *reply = new QPlaceRecommendationReplyImpl(networkReply, this);
    connect(reply, SIGNAL(finished()), this, SLOT(replyFinished()));
    connect(reply, SIGNAL(error(QPlaceReply::Error,QString)),
            this, SLOT(replyError(QPlaceReply::Error,QString)));

    return reply;
}

QPlaceSearchSuggestionReply *QPlaceManagerEngineNokiaV2::searchSuggestions(const QPlaceSearchRequest &query)
{
    QUrl requestUrl(m_placesServer + QLatin1String("/v1/suggest"));

    QList<QPair<QString, QString> > queryItems;

    queryItems.append(qMakePair<QString, QString>(QLatin1String("q"), query.searchTerm()));

    addAtForBoundingArea(query.searchArea(), &queryItems);

    requestUrl.setQueryItems(queryItems);

    QNetworkReply *networkReply = sendRequest(requestUrl);

    QPlaceSearchSuggestionReplyImpl *reply = new QPlaceSearchSuggestionReplyImpl(networkReply, this);
    connect(reply, SIGNAL(finished()), this, SLOT(replyFinished()));
    connect(reply, SIGNAL(error(QPlaceReply::Error,QString)),
            this, SLOT(replyError(QPlaceReply::Error,QString)));

    return reply;
}

QPlaceIdReply *QPlaceManagerEngineNokiaV2::savePlace(const QPlace &place)
{
    QPlaceIdReplyImpl *reply = new QPlaceIdReplyImpl(QPlaceIdReply::SavePlace, this);
    reply->setId(place.placeId());
    QMetaObject::invokeMethod(reply, "setError", Qt::QueuedConnection,
                              Q_ARG(QPlaceReply::Error, QPlaceReply::UnsupportedError),
                              Q_ARG(QString, tr("Saving places is not supported.")));
    connect(reply, SIGNAL(finished()), this, SLOT(replyFinished()));
    connect(reply, SIGNAL(error(QPlaceReply::Error,QString)),
            this, SLOT(replyError(QPlaceReply::Error,QString)));
    return reply;
}

QPlaceIdReply *QPlaceManagerEngineNokiaV2::removePlace(const QString &placeId)
{
    QPlaceIdReplyImpl *reply = new QPlaceIdReplyImpl(QPlaceIdReply::RemovePlace, this);
    reply->setId(placeId);
    QMetaObject::invokeMethod(reply, "setError", Qt::QueuedConnection,
                              Q_ARG(QPlaceReply::Error, QPlaceReply::UnsupportedError),
                              Q_ARG(QString, tr("Removing places is not supported.")));
    connect(reply, SIGNAL(finished()), this, SLOT(replyFinished()));
    connect(reply, SIGNAL(error(QPlaceReply::Error,QString)),
            this, SLOT(replyError(QPlaceReply::Error,QString)));
    return reply;
}

QPlaceIdReply *QPlaceManagerEngineNokiaV2::saveCategory(const QPlaceCategory &category, const QString &parentId)
{
    Q_UNUSED(parentId)

    QPlaceIdReplyImpl *reply = new QPlaceIdReplyImpl(QPlaceIdReply::SaveCategory, this);
    reply->setId(category.categoryId());
    QMetaObject::invokeMethod(reply, "setError", Qt::QueuedConnection,
                              Q_ARG(QPlaceReply::Error, QPlaceReply::UnsupportedError),
                              Q_ARG(QString, tr("Saving categories is not supported.")));
    connect(reply, SIGNAL(finished()), this, SLOT(replyFinished()));
    connect(reply, SIGNAL(error(QPlaceReply::Error,QString)),
            this, SLOT(replyError(QPlaceReply::Error,QString)));
    return reply;
}

QPlaceIdReply *QPlaceManagerEngineNokiaV2::removeCategory(const QString &categoryId)
{
    QPlaceIdReplyImpl *reply = new QPlaceIdReplyImpl(QPlaceIdReply::RemoveCategory, this);
    reply->setId(categoryId);
    QMetaObject::invokeMethod(reply, "setError", Qt::QueuedConnection,
                              Q_ARG(QPlaceReply::Error, QPlaceReply::UnsupportedError),
                              Q_ARG(QString, tr("Removing categories is not supported.")));
    connect(reply, SIGNAL(finished()), this, SLOT(replyFinished()));
    connect(reply, SIGNAL(error(QPlaceReply::Error,QString)),
            this, SLOT(replyError(QPlaceReply::Error,QString)));
    return reply;
}

QPlaceReply *QPlaceManagerEngineNokiaV2::initializeCategories()
{
    if (m_categoryReply)
        return m_categoryReply.data();

    for (int i = 0; FIXED_CATEGORIES_indices[i] != -1; ++i) {
        const QString id = QString::fromLatin1(FIXED_CATEGORIES_string +
                                               FIXED_CATEGORIES_indices[i]);

        QUrl requestUrl(m_placesServer + QLatin1String("/v1/categories/places/") + id);
        QNetworkReply *networkReply = sendRequest(requestUrl);
        connect(networkReply, SIGNAL(finished()), this, SLOT(categoryReplyFinished()));
        connect(networkReply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(categoryReplyError()));

        m_categoryRequests.insert(id, networkReply);
    }

    QPlaceCategoriesReplyImpl *reply = new QPlaceCategoriesReplyImpl(this);
    connect(reply, SIGNAL(finished()), this, SLOT(replyFinished()));
    connect(reply, SIGNAL(error(QPlaceReply::Error,QString)),
            this, SLOT(replyError(QPlaceReply::Error,QString)));

    m_categoryReply = reply;
    return reply;
}

QString QPlaceManagerEngineNokiaV2::parentCategoryId(const QString &categoryId) const
{
    return m_categoryTree.value(categoryId).parentId;
}

QStringList QPlaceManagerEngineNokiaV2::childCategoryIds(const QString &categoryId) const
{
    return m_categoryTree.value(categoryId).childIds;
}

QPlaceCategory QPlaceManagerEngineNokiaV2::category(const QString &categoryId) const
{
    return m_categoryTree.value(categoryId).category;
}

QList<QPlaceCategory> QPlaceManagerEngineNokiaV2::childCategories(const QString &parentId) const
{
    QList<QPlaceCategory> results;
    foreach (const QString &childId, m_categoryTree.value(parentId).childIds)
        results.append(m_categoryTree.value(childId).category);
    return results;
}

QList<QLocale> QPlaceManagerEngineNokiaV2::locales() const
{
    return m_locales;
}

void QPlaceManagerEngineNokiaV2::setLocales(const QList<QLocale> &locales)
{
    m_locales = locales;
}

QString QPlaceManagerEngineNokiaV2::iconPath(const QString &remotePath) const
{
    if (remotePath.isEmpty())
        return QString();

    QString remoteIcon = remotePath
            + (!m_theme.isEmpty() ? QLatin1Char('.') + m_theme : QString());

    if (!remotePath.contains(QLatin1String("/icons/categories/"))
            || m_localDataPath.isEmpty()) {
        return remoteIcon;
    }

    QString localIcon = remotePath.mid(remotePath.lastIndexOf(QLatin1Char('/')) + 1);
    localIcon.prepend(m_localDataPath + QLatin1String("/icons/categories/"));

    if (!m_theme.isEmpty())
        localIcon.append(QLatin1Char('.') + m_theme);

    if (QFile::exists(localIcon))
        return QString::fromLatin1("file://") + localIcon;
    else
        return remoteIcon;
}

void QPlaceManagerEngineNokiaV2::replyFinished()
{
    QPlaceReply *reply = qobject_cast<QPlaceReply *>(sender());
    if (reply)
        emit finished(reply);
}

void QPlaceManagerEngineNokiaV2::replyError(QPlaceReply::Error error_, const QString &errorString)
{
    QPlaceReply *reply = qobject_cast<QPlaceReply *>(sender());
    if (reply)
        emit error(reply, error_, errorString);
}

void QPlaceManagerEngineNokiaV2::categoryReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
        if (!document.isObject()) {
            if (m_categoryReply) {
                QMetaObject::invokeMethod(m_categoryReply.data(), "setError", Qt::QueuedConnection,
                                          Q_ARG(QPlaceReply::Error, QPlaceReply::ParseError),
                                          Q_ARG(QString, tr("Error parsing response.")));
            }
            return;
        }

        QJsonObject category = document.object();

        PlaceCategoryNode node;
        node.category.setCategoryId(category.value(QLatin1String("categoryId")).toString());
        node.category.setName(category.value(QLatin1String("name")).toString());

        QString iconPath = QPlaceManagerEngineNokiaV2::iconPath(
                        category.value(QLatin1String("icon")).toString());
        QVariantMap parameters;
        parameters.insert(QPlaceIcon::SingleUrl,
                          QUrl(iconPath));
        QPlaceIcon icon;
        icon.setParameters(parameters);
        node.category.setIcon(icon);

        m_categoryTree.insert(node.category.categoryId(), node);

        m_categoryTree[QString()].childIds.append(node.category.categoryId());
    }

    m_categoryRequests.remove(m_categoryRequests.key(reply));
    reply->deleteLater();

    if (m_categoryRequests.isEmpty() && m_categoryReply)
        m_categoryReply.data()->emitFinished();
}

void QPlaceManagerEngineNokiaV2::categoryReplyError()
{
    if (m_categoryReply) {
        QMetaObject::invokeMethod(m_categoryReply.data(), "setError", Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply::Error, QPlaceReply::CommunicationError),
                                  Q_ARG(QString, tr("Network error.")));
    }
}

#ifdef USE_CHINA_NETWORK_REGISTRATION
void QPlaceManagerEngineNokiaV2::currentMobileCountryCodeChanged(int interface, const QString &mcc)
{
    Q_UNUSED(interface)

    if (mcc == QLatin1String("460") || mcc == QLatin1String("461") ||
        mcc == QLatin1String("454") || mcc == QLatin1String("455")) {
        m_placesServer = QLatin1String(placesServerChina);
    } else {
        m_placesServer = m_host;
    }
}
#endif

QNetworkReply *QPlaceManagerEngineNokiaV2::sendRequest(const QUrl &url)
{
    QList<QPair<QString, QString> > queryItems = url.queryItems();
    queryItems.append(qMakePair<QString, QString>(QLatin1String("app_id"), m_appId));
    queryItems.append(qMakePair<QString, QString>(QLatin1String("app_code"), m_appCode));

    QUrl requestUrl = url;
    requestUrl.setQueryItems(queryItems);

    QNetworkRequest request;
    request.setUrl(requestUrl);

    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("Accept-Language", createLanguageString());

    return m_manager->get(request);
}

QByteArray QPlaceManagerEngineNokiaV2::createLanguageString() const
{
    QByteArray language;

    QList<QLocale> locales = m_locales;
    if (locales.isEmpty())
        locales << QLocale();

    foreach (const QLocale &loc, locales) {
        language.append(loc.name().replace(2, 1, QLatin1Char('-')).toAscii());
        language.append(", ");
    }
    language.chop(2);

    return language;
}

QT_END_NAMESPACE
