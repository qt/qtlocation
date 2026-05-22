// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qregularexpression.h>
#include <QtCore/qstringlist.h>
#include <QtLocation/private/unsupportedreplies_p.h>
#include <QtLocation/qplacecategory.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <qplacemanagerengineohosmapkit.h>
#include <qplacesearchreplyohosmapkit.h>
#include <qplacesearchsuggestionreplyohosmapkit.h>
#include <qohosmapkitcommon.h>
#include <qpoicategoryohosmapkit.h>

QT_BEGIN_NAMESPACE

namespace {

const QString placeSearchApiUrlBase =
    QLatin1String("https://siteapi.cloud.huawei.com/mapApi/v1/siteService/");
const auto searchByTextApiUrl = QUrl(placeSearchApiUrlBase + QLatin1String("searchByText"));
const auto autoCompleteApiUrl = QUrl(placeSearchApiUrlBase + QLatin1String("queryAutoComplete"));

class QPlaceManagerEngineOhosMapKit : public QPlaceManagerEngine
{
public:
    QPlaceManagerEngineOhosMapKit(
        const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString);

    QPlaceSearchReply *search(const QPlaceSearchRequest &) override;
    QPlaceSearchSuggestionReply *searchSuggestions(const QPlaceSearchRequest &request) override;

    QPlaceReply *initializeCategories() override;
    QString parentCategoryId(const QString &categoryId) const override;
    QStringList childCategoryIds(const QString &categoryId) const override;
    QPlaceCategory category(const QString &categoryId) const override;
    QList<QPlaceCategory> childCategories(const QString &parentId) const override;

private:
    void onReplyFinished();
    void onReplyError(QPlaceReply::Error errorCode, const QString &errorString);

    QNetworkAccessManager m_networkManager;
    QString m_userAgent;
    QString m_authenticationKey;
};

class DummyPlaceReply : public QPlaceReply
{
public:
    DummyPlaceReply(QObject *parent);

    void finish();
};

DummyPlaceReply::DummyPlaceReply(QObject *parent)
    : QPlaceReply(parent)
{}

void DummyPlaceReply::finish()
{
    setFinished(true);
    Q_EMIT finished();
}

QJsonObject createPlaceSearchRequestBody(const QPlaceSearchRequest &request)
{
    QJsonObject placeSearchRequestBody;

    placeSearchRequestBody.insert(QStringLiteral("query"), request.searchTerm());

    auto searchArea = request.searchArea();
    if (searchArea.isValid() && !searchArea.isEmpty()) {
        constexpr int restApiMinRadiusInMeters = 1;
        constexpr int restApiMaxRadiusInMeters = 50000;
        auto radius = qBound(
            restApiMinRadiusInMeters,
            qRound(searchArea.center().distanceTo(searchArea.boundingGeoRectangle().topLeft())),
            restApiMaxRadiusInMeters);

        placeSearchRequestBody.insert(
            QStringLiteral("location"),
            OhosMapKit::CoordinateJson::tryConvertFromQGeoCoordinate(searchArea.center()));
        placeSearchRequestBody.insert(QStringLiteral("radius"), radius);
    }

    placeSearchRequestBody.insert(QStringLiteral("language"), OhosMapKit::getLanguageCode());

    return placeSearchRequestBody;
}

QString convertCategoryIdToCategoryName(const QString &categoryId)
{
    auto base = categoryId;
    if (!base.startsWith("ADMINISTRATIVE_AREA"))
        base.remove(QRegularExpression(R"((_LEVEL_\d+)$)"));

    auto parts = base.split(QLatin1Char('_'), Qt::SkipEmptyParts);
    for (int i = 0; i < parts.size(); ++i) {
        parts[i] = parts[i].toLower();
        if (i == 0 && !parts[i].isEmpty())
            parts[i][0] = parts[i][0].toUpper();
    }
    return parts.join(QLatin1Char(' '));
}


QPlaceManagerEngineOhosMapKit::QPlaceManagerEngineOhosMapKit(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString)
    : QPlaceManagerEngine(parameters)
    , m_userAgent(OhosMapKit::getUserAgentParameterOrDefault(parameters))
    , m_authenticationKey(OhosMapKit::getAuthenticationKeyParameterOrEmpty(parameters))
{
    if (error != nullptr)
        *error = QGeoServiceProvider::NoError;

    if (errorString != nullptr)
        errorString->clear();
}

QPlaceSearchReply *QPlaceManagerEngineOhosMapKit::search(const QPlaceSearchRequest &request)
{
    if (request.categories().size() > 1) {
        return new QPlaceSearchReplyUnsupported(
            QPlaceReply::UnsupportedError,
            tr("OHOS Map Kit allows search of only one category type."),
            this);
    }

    auto placeSearchRequestBody = createPlaceSearchRequestBody(request);

    constexpr auto restApiMinNumOfResultsPerPage = 1;
    constexpr auto restApiMaxNumOfResultsPerPage = 20;
    if (request.limit() > 0) {
        placeSearchRequestBody.insert(
            QStringLiteral("pageSize"),
            qBound(restApiMinNumOfResultsPerPage, request.limit(), restApiMaxNumOfResultsPerPage));
    }

    if (!request.categories().isEmpty())
        placeSearchRequestBody.insert(QStringLiteral("hwPoiType"), request.categories().first().categoryId());

    auto *reply = makeQPlaceSearchReplyOhosMapKit(
        request,
        m_networkManager.post(
            OhosMapKit::createOhosMapKitNetworkRequestWithJsonBody(
                searchByTextApiPath, m_userAgent, m_authenticationKey),
            QJsonDocument(placeSearchRequestBody).toJson(QJsonDocument::Compact)),
        this);

    connect(
        reply, &QPlaceSearchReply::finished,
        this, &QPlaceManagerEngineOhosMapKit::onReplyFinished);
    connect(
        reply,
        &QPlaceSearchReply::errorOccurred,
        this,
        &QPlaceManagerEngineOhosMapKit::onReplyError);

    return reply;
}

QPlaceSearchSuggestionReply *QPlaceManagerEngineOhosMapKit::searchSuggestions(
    const QPlaceSearchRequest &request)
{
    auto *reply = makeQPlaceSearchSuggestionReplyOhosMapKit(
        m_networkManager.post(
            OhosMapKit::createOhosMapKitNetworkRequestWithJsonBody(
                autoCompleteApiPath, m_userAgent, m_authenticationKey),
            QJsonDocument(createPlaceSearchRequestBody(request)).toJson(QJsonDocument::Compact)),
        this);

    connect(
        reply, &QPlaceSearchSuggestionReply::finished,
        this, &QPlaceManagerEngineOhosMapKit::onReplyFinished);
    connect(
        reply,
        &QPlaceSearchSuggestionReply::errorOccurred,
        this,
        &QPlaceManagerEngineOhosMapKit::onReplyError);

    return reply;
}

QPlaceReply *QPlaceManagerEngineOhosMapKit::initializeCategories()
{
    auto *reply = new DummyPlaceReply(this);
    connect(reply, &QPlaceReply::finished, this, &QPlaceManagerEngineOhosMapKit::onReplyFinished);
    QMetaObject::invokeMethod(reply, &DummyPlaceReply::finish, Qt::QueuedConnection);

    return reply;
}

QString QPlaceManagerEngineOhosMapKit::parentCategoryId(const QString &categoryId) const
{
    return QPoiCategoryOhosMapKit::tryGetParentCategoryIdOrEmpty(categoryId);
}

QStringList QPlaceManagerEngineOhosMapKit::childCategoryIds(const QString &categoryId) const
{
    return QPoiCategoryOhosMapKit::tryGetChildCategoryIdsOrEmpty(categoryId);
}

QPlaceCategory QPlaceManagerEngineOhosMapKit::category(const QString &categoryId) const
{
    QPlaceCategory placeCategory;
    placeCategory.setCategoryId(categoryId);
    placeCategory.setName(convertCategoryIdToCategoryName(categoryId));
    return placeCategory;
}

QList<QPlaceCategory> QPlaceManagerEngineOhosMapKit::childCategories(const QString &parentId) const
{
    auto categoryIds = childCategoryIds(parentId);

    QList<QPlaceCategory> childCategories;
    for (const auto &categoryId : categoryIds)
        childCategories.push_back(category(categoryId));

    return childCategories;
}

void QPlaceManagerEngineOhosMapKit::onReplyFinished()
{
    auto *reply = qobject_cast<QPlaceReply *>(sender());
    if (reply != nullptr)
        Q_EMIT finished(reply);
}

void QPlaceManagerEngineOhosMapKit::onReplyError(
    QPlaceReply::Error errorCode, const QString &errorString)
{
    auto *reply = qobject_cast<QPlaceReply *>(sender());
    if (reply != nullptr)
        Q_EMIT errorOccurred(reply, errorCode, errorString);
}

}

QPlaceManagerEngine *makeQPlaceManagerEngineOhosMapKit(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString)
{
    return new QPlaceManagerEngineOhosMapKit(parameters, error, errorString);
}

QT_END_NAMESPACE
