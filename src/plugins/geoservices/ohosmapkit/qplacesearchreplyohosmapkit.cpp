// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/qjsonarray.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qmetaobject.h>
#include <QtLocation/qplace.h>
#include <QtLocation/qplaceattribute.h>
#include <QtLocation/qplacecategory.h>
#include <QtLocation/qplacecontactdetail.h>
#include <QtLocation/qplaceicon.h>
#include <QtLocation/qplaceratings.h>
#include <QtLocation/qplaceresult.h>
#include <QtLocation/qplacesearchrequest.h>
#include <QtLocation/qplacesearchresult.h>
#include <QtPositioning/qgeoaddress.h>
#include <QtPositioning/qgeolocation.h>
#include <qohosmapkitcommon.h>
#include <qplacesearchreplyohosmapkit.h>
#include <qplacemanagerengineohosmapkit.h>

QT_BEGIN_NAMESPACE

namespace {

class QPlaceSearchReplyOhosMapKit : public QPlaceSearchReply
{
public:
    QPlaceSearchReplyOhosMapKit(
        const QPlaceSearchRequest &request, QNetworkReply *reply, QPlaceManagerEngine *parent);

private:
    void onReplyFinished();
    void onNetworkError(QNetworkReply::NetworkError error);
};

void populatePlaceExtendedAttributes(const QJsonObject &poiJsonObj, QPlace &place)
{
    const QJsonValue openingHours = poiJsonObj.value(QStringLiteral("openingHours"));
    if (!openingHours.isUndefined() && !openingHours.isNull()) {
        QPlaceAttribute attr;
        attr.setLabel(QPlaceSearchReplyOhosMapKit::tr("Opening hours"));
        attr.setText(openingHours.toVariant().toString());
        place.setExtendedAttribute(QStringLiteral("openingHours"), attr);
    }

    const QJsonArray photos = poiJsonObj.value(QStringLiteral("photoUrls")).toArray();
    if (!photos.isEmpty()) {
        QPlaceAttribute attr;
        attr.setLabel(QPlaceSearchReplyOhosMapKit::tr("Photo URLs"));
        attr.setText(QString::fromUtf8(QJsonDocument(photos).toJson(QJsonDocument::Compact)));
        place.setExtendedAttribute(QStringLiteral("photoUrls"), attr);
    }
}

void populatePlaceFromPoiJson(const QJsonObject &poiJsonObj, QPlace &place)
{
    auto phoneData = poiJsonObj.value(QStringLiteral("phone")).toVariant().toString();
    if (!phoneData.isEmpty()) {
        QPlaceContactDetail phoneDetail;
        phoneDetail.setLabel(QPlaceSearchReplyOhosMapKit::tr("Phone"));
        phoneDetail.setValue(phoneData);
        place.appendContactDetail(QPlaceContactDetail::Phone, phoneDetail);
    }

    auto internationalPhoneData =
        poiJsonObj.value(QStringLiteral("internationalPhone")).toVariant().toString();
    if (!internationalPhoneData.isEmpty()) {
        QPlaceContactDetail intlPhoneDetail;
        intlPhoneDetail.setLabel(QPlaceSearchReplyOhosMapKit::tr("International phone"));
        intlPhoneDetail.setValue(internationalPhoneData);
        place.appendContactDetail(QPlaceContactDetail::Phone, intlPhoneDetail);
    }

    auto websiteUrl = poiJsonObj.value(QStringLiteral("websiteUrl")).toVariant().toString();
    if (!websiteUrl.isEmpty()) {
        QPlaceContactDetail websiteDetail;
        websiteDetail.setLabel(QPlaceSearchReplyOhosMapKit::tr("Website"));
        websiteDetail.setValue(internationalPhoneData);
        place.appendContactDetail(QPlaceContactDetail::Website, websiteDetail);
    }

    auto rating = poiJsonObj.value(QStringLiteral("rating")).toDouble(0.0);
    if (rating > 0.0) {
        QPlaceRatings ratings;
        ratings.setAverage(rating);
        place.setRatings(ratings);
    }

    auto iconUrl = poiJsonObj.value(QStringLiteral("icon")).toVariant().toUrl();
    if (!iconUrl.isEmpty()) {
        QPlaceIcon icon;
        icon.setParameters({{QPlaceIcon::SingleUrl, iconUrl}});
        place.setIcon(icon);
    }

    auto hwPoi = poiJsonObj.value(QStringLiteral("hwPoiTypes")).toArray();
    if (!hwPoi.isEmpty()) {
        auto id = hwPoi.first().toVariant().toString();
        if (!id.isEmpty()) {
            QPlaceCategory c;
            c.setCategoryId(id);
            place.setCategory(c);
        }
    }

    populatePlaceExtendedAttributes(poiJsonObj, place);
}

QPlaceResult siteJsonObjToPlaceResult(const QJsonObject &siteJsonObj)
{
    QPlace place;

    place.setName(siteJsonObj.value(QStringLiteral("name")).toVariant().toString());
    place.setLocation(OhosMapKit::SiteJson::tryConvertToQGeoLocation(siteJsonObj));

    const QJsonObject poiObj = siteJsonObj.value(QStringLiteral("poi")).toObject();
    if (!poiObj.isEmpty())
        populatePlaceFromPoiJson(poiObj, place);

    QPlaceResult result;
    result.setPlace(place);
    result.setDistance(siteJsonObj.value(QStringLiteral("distance")).toVariant().toInt());
    return result;
}

QList<QPlaceSearchResult> parsePlaceSearchResponse(const QJsonDocument &responseBodyJsonDoc)
{
    QList<QPlaceSearchResult> results;

    const QJsonObject responseBodyJsonObj = responseBodyJsonDoc.object();
    if (responseBodyJsonObj.contains(QStringLiteral("sites"))) {
        const QJsonArray sites = responseBodyJsonObj.value(QStringLiteral("sites")).toArray();
        for (const QJsonValue &siteValue : sites) {
            if (siteValue.isObject())
                results.append(siteJsonObjToPlaceResult(siteValue.toObject()));
        }
    }

    return results;
}

QPlaceSearchReplyOhosMapKit::QPlaceSearchReplyOhosMapKit(
    const QPlaceSearchRequest &request, QNetworkReply *reply, QPlaceManagerEngine *parent)
    : QPlaceSearchReply(parent)
{
    Q_ASSERT(parent);
    if (!reply) {
        setError(QPlaceReply::UnknownError, QStringLiteral("Null reply"));
        return;
    }
    setRequest(request);

    connect(reply, &QNetworkReply::finished, this, &QPlaceSearchReplyOhosMapKit::onReplyFinished);
    connect(reply, &QNetworkReply::errorOccurred, this, &QPlaceSearchReplyOhosMapKit::onNetworkError);

    connect(this, &QPlaceReply::aborted, reply, &QNetworkReply::abort);
    connect(this, &QObject::destroyed, reply, &QObject::deleteLater);
}

void QPlaceSearchReplyOhosMapKit::onReplyFinished()
{
    auto *reply = static_cast<QNetworkReply *>(sender());
    reply->deleteLater();

    auto setErrorEmitAndFinish = [&](QPlaceReply::Error errorCode, const QString &errorString) {
        setError(errorCode, errorString);
        Q_EMIT errorOccurred(errorCode, errorString);

        setFinished(true);
        Q_EMIT finished();
    };

    if (reply->error() != QNetworkReply::NoError) {
        setErrorEmitAndFinish(
            QPlaceReply::CommunicationError,
            OhosMapKit::msgReplyError(reply->errorString()));
        return;
    }

    const auto responseBodyJsonDoc = QJsonDocument::fromJson(reply->readAll());
    if (!responseBodyJsonDoc.isObject()) {
        setErrorEmitAndFinish(QPlaceReply::ParseError, tr("Response parse error"));
        return;
    }

    auto results = parsePlaceSearchResponse(responseBodyJsonDoc);

    if (request().relevanceHint() == QPlaceSearchRequest::DistanceHint) {
        std::sort(
            results.begin(), results.end(),
            [](const auto &a, const auto &b) {
                return
                    a.type() == QPlaceSearchResult::PlaceResult
                    && b.type() == QPlaceSearchResult::PlaceResult
                    && QPlaceResult(a).distance() < QPlaceResult(b).distance();
            });
    } else if (request().relevanceHint() == QPlaceSearchRequest::LexicalPlaceNameHint) {
        std::sort(
            results.begin(), results.end(),
            [](const auto &a, const auto &b) {
                return
                    a.type() == QPlaceSearchResult::PlaceResult
                    && b.type() == QPlaceSearchResult::PlaceResult
                    && QPlaceResult(a).place().name() < QPlaceResult(b).place().name();
            });
    }

    setResults(results);

    setFinished(true);
    Q_EMIT finished();
}

void QPlaceSearchReplyOhosMapKit::onNetworkError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error)
    auto *reply = static_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    setError(QPlaceReply::CommunicationError, reply->errorString());
}

}

QPlaceSearchReply *makeQPlaceSearchReplyOhosMapKit(
    const QPlaceSearchRequest &request, QNetworkReply *reply, QPlaceManagerEngine *parent)
{
    return new QPlaceSearchReplyOhosMapKit(request, reply, parent);
}

QT_END_NAMESPACE
