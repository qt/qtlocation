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

#include "qplacemanagerengine_nokiav1.h"

#include <QtLocation/QPlaceContentRequest>

#include "placesv1/qplacecategoriesrepository.h"
#include "placesv1/qplacecontentreplyimplv1.h"
#include "placesv1/qplacetextpredictionreplyimpl.h"
#include "placesv1/qplacesearchreplyimplv1.h"
#include "placesv1/qplacerecommendationreplyimplv1.h"
#include "placesv1/qplacedetailsreplyimplv1.h"
#include "placesv1/qplaceratingreplyimpl.h"
#include "placesv1/qplacerestmanager.h"
#include "placesv1/qplacerestreply.h"
#include "placesv1/unsupportedreplies.h"

QT_USE_NAMESPACE

QPlaceManagerEngineNokiaV1::QPlaceManagerEngineNokiaV1(const QMap<QString, QVariant> &parameters,
                                                 QGeoServiceProvider::Error *error,
                                                 QString *errorString)
:   QPlaceManagerEngine(parameters)
{
    qRegisterMetaType<QPlaceReply::Error>();

    if (parameters.contains(QLatin1String("places.proxy"))) {
        QString proxy = parameters.value("places.proxy").toString();
        if (!proxy.isEmpty()) {
            QUrl proxyUrl(proxy);
            if (proxyUrl.isValid()) {
                QPlaceRestManager::instance()->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy,
                                                                      proxyUrl.host(),
                                                                      proxyUrl.port(8080),
                                                                      proxyUrl.userName(),
                                                                      proxyUrl.password()));
            }
        }
    }

    if (error)
        *error = QGeoServiceProvider::NoError;

    if (errorString)
        errorString->clear();
}

QPlaceManagerEngineNokiaV1::~QPlaceManagerEngineNokiaV1()
{
}

QPlaceDetailsReply *QPlaceManagerEngineNokiaV1::getPlaceDetails(const QString &placeId)
{
    QPlaceDetailsReplyImplV1 *reply = 0;
    QPlaceRestReply *restReply = QPlaceRestManager::instance()->sendPlaceRequest(placeId);
    if (restReply) {
        reply = new QPlaceDetailsReplyImplV1(restReply, manager(), this);
        connect(reply, SIGNAL(processingError(QPlaceReply*,QPlaceReply::Error,QString)),
                this, SLOT(processingError(QPlaceReply*,QPlaceReply::Error,QString)));
        connect(reply, SIGNAL(processingFinished(QPlaceReply*)),
                this, SLOT(processingFinished(QPlaceReply*)));
    }
    return reply;
}

QPlaceContentReply *QPlaceManagerEngineNokiaV1::getPlaceContent(const QString &placeId, const QPlaceContentRequest &request)
{
    QPlaceContentReplyImplV1 *reply;
    switch (request.contentType()) {
    case QPlaceContent::ImageType: {
            QPlaceRestReply *restReply = QPlaceRestManager::instance()->sendPlaceImagesRequest(placeId,
                                                                                               request);
            reply = new QPlaceContentReplyImplV1(request, restReply, manager(), this);

            if (!restReply)
                QMetaObject::invokeMethod(reply, "restError", Qt::QueuedConnection,
                                          Q_ARG(QPlaceReply::Error, QPlaceReply::UnknownError),
                                          Q_ARG(QString, QString("Could not create rest reply for image content request")));
            break;
        }
    case QPlaceContent::ReviewType: {
            QPlaceRestReply *restReply = QPlaceRestManager::instance()->sendPlaceReviewRequest(placeId,
                                                                                               request);
            reply = new QPlaceContentReplyImplV1(request, restReply, manager(), this);

            if (!restReply)
                QMetaObject::invokeMethod(reply, "restError", Qt::QueuedConnection,
                                          Q_ARG(QPlaceReply::Error, QPlaceReply::UnknownError),
                                          Q_ARG(QString, QString("Could not create rest reply for review content request")));
            break;
        }
    case QPlaceContent::EditorialType: {
        QPlaceRestReply *restReply =
            QPlaceRestManager::instance()->sendPlaceRequest(placeId);

        reply = new QPlaceContentReplyImplV1(request, restReply, manager(), this);

        if (!restReply) {
            QMetaObject::invokeMethod(reply, "restError", Qt::QueuedConnection,
                                      Q_ARG(QPlaceReply::Error, QPlaceReply::UnknownError),
                                      Q_ARG(QString, QString("Could not create rest reply for editorial content request")));
        }

        break;
    }
    default: {
            reply = new QPlaceContentReplyImplV1(request, 0, manager(), this);
            QMetaObject::invokeMethod(reply, "restError", Qt::QueuedConnection,
                                      Q_ARG(QPlaceReply::Error, QPlaceReply::UnsupportedError),
                                      Q_ARG(QString, QString("Retrieval of given content type not supported")));
        }
    }

    reply->setStartNumber(request.offset());
    connect(reply, SIGNAL(processingError(QPlaceReply*,QPlaceReply::Error,QString)),
            this, SLOT(processingError(QPlaceReply*,QPlaceReply::Error,QString)));
    connect(reply, SIGNAL(processingFinished(QPlaceReply*)),
            this, SLOT(processingFinished(QPlaceReply*)));

    return reply;
}

QPlaceSearchReply *QPlaceManagerEngineNokiaV1::search(const QPlaceSearchRequest &query)
{
    //TODO: handling of scope
    QPlaceSearchReplyImplV1 *reply = 0;
    QPlaceSearchRequest newQuery = query;
    // search by category
    if (newQuery.categories().count()) {
        newQuery.setSearchTerm(query.categories().at(0).name());
    }

    if (query.visibilityScope() == QtLocation::UnspecifiedVisibility ||
        query.visibilityScope() == QtLocation::PublicVisibility) {

        QPlaceRestReply *restReply = QPlaceRestManager::instance()->sendSearchRequest(newQuery);

        if (restReply) {
            reply = new QPlaceSearchReplyImplV1(restReply, this);
            connect(reply, SIGNAL(processingError(QPlaceReply*,QPlaceReply::Error,QString)),
                    this, SLOT(processingError(QPlaceReply*,QPlaceReply::Error,QString)));
            connect(reply, SIGNAL(processingFinished(QPlaceReply*)),
                    this, SLOT(processingFinished(QPlaceReply*)));
        }
    } else {
        reply = new QPlaceSearchReplyImplV1(0, this);
        QMetaObject::invokeMethod(reply,
                                  "setError",
                                  Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply::Error, QPlaceReply::UnsupportedError),
                                  Q_ARG(QString, "Searching for places with unsupported visibility scope"));

    }
    return reply;
}

QPlaceSearchReply *QPlaceManagerEngineNokiaV1::recommendations(const QString &placeId, const QPlaceSearchRequest &query)
{
    QPlaceRecommendationReplyImplV1 *reply = 0;
    QPlaceSearchRequest newQuery = query;
    newQuery.setSearchTerm(placeId);
    QPlaceRestReply *restReply = QPlaceRestManager::instance()->sendRecommendationRequest(newQuery, QString());
    if (restReply) {
        reply = new QPlaceRecommendationReplyImplV1(restReply, manager(), this);
        connect(reply, SIGNAL(processingError(QPlaceReply*,QPlaceReply::Error,QString)),
                this, SLOT(processingError(QPlaceReply*,QPlaceReply::Error,QString)));
        connect(reply, SIGNAL(processingFinished(QPlaceReply*)),
                this, SLOT(processingFinished(QPlaceReply*)));
    }
    return reply;
}

QPlaceSearchSuggestionReply *QPlaceManagerEngineNokiaV1::searchSuggestions(const QPlaceSearchRequest &query)
{
    QPlaceTextPredictionReplyImpl *reply = NULL;
    QPlaceRestReply *restReply = QPlaceRestManager::instance()->sendSuggestionRequest(query);
    if (restReply) {
        reply = new QPlaceTextPredictionReplyImpl(restReply, this);
        connect(reply, SIGNAL(processingError(QPlaceReply*,QPlaceReply::Error,QString)),
                this, SLOT(processingError(QPlaceReply*,QPlaceReply::Error,QString)));
        connect(reply, SIGNAL(processingFinished(QPlaceReply*)),
                this, SLOT(processingFinished(QPlaceReply*)));
    }
    return reply;
}

QPlaceIdReply *QPlaceManagerEngineNokiaV1::savePlace(const QPlace &place)
{
    IdReply *reply = new IdReply(QPlaceIdReply::SavePlace, this);
    reply->setId(place.placeId());
    reply->triggerDone(QPlaceReply::UnsupportedError, tr("Saving places is not supported"));
    return reply;
}

QPlaceIdReply *QPlaceManagerEngineNokiaV1::removePlace(const QString &placeId)
{
    IdReply *reply = new IdReply(QPlaceIdReply::RemovePlace, this);
    reply->setId(placeId);
    reply->triggerDone(QPlaceReply::UnsupportedError, tr("Removing places is not supported"));
    return reply;
}

QPlaceIdReply *QPlaceManagerEngineNokiaV1::saveCategory(const QPlaceCategory &category, const QString &parentId)
{
    Q_UNUSED(parentId)

    IdReply *reply = new IdReply(QPlaceIdReply::SaveCategory, this);
    reply->setId(category.categoryId());
    reply->triggerDone(QPlaceReply::UnsupportedError, tr("Saving categories is not supported"));
    return reply;
}

QPlaceIdReply *QPlaceManagerEngineNokiaV1::removeCategory(const QString &categoryId)
{
    IdReply *reply = new IdReply(QPlaceIdReply::RemoveCategory, this);
    reply->setId(categoryId);
    reply->triggerDone(QPlaceReply::UnsupportedError, tr("Removing categories is not supported"));
    return reply;
}

QPlaceReply *QPlaceManagerEngineNokiaV1::initializeCategories()
{
    QPlaceReply *reply = QPlaceCategoriesRepository::instance()->initializeCategories();
    if (reply) {
        reply->setParent(this);
        connect(reply, SIGNAL(processingError(QPlaceReply*,QPlaceReply::Error,QString)),
                this, SLOT(processingError(QPlaceReply*,QPlaceReply::Error,QString)));
        connect(reply, SIGNAL(processingFinished(QPlaceReply*)),
                this, SLOT(processingFinished(QPlaceReply*)));
    }
    return reply;
}

QString QPlaceManagerEngineNokiaV1::parentCategoryId(const QString &categoryId) const
{
    QPlaceCategoryTree tree = QPlaceCategoriesRepository::instance()->categories();
    return tree.value(categoryId).parentId;
}

QStringList QPlaceManagerEngineNokiaV1::childCategoryIds(const QString &categoryId) const
{
    QPlaceCategoryTree tree = QPlaceCategoriesRepository::instance()->categories();
    return tree.value(categoryId).childIds;
}

QPlaceCategory QPlaceManagerEngineNokiaV1::category(const QString &categoryId) const
{
    QPlaceCategoryTree tree = QPlaceCategoriesRepository::instance()->categories();
    return tree.value(categoryId).category;
}

QList<QPlaceCategory> QPlaceManagerEngineNokiaV1::childCategories(const QString &parentId) const
{
    QList<QPlaceCategory> results;
    QPlaceCategoryTree tree = QPlaceCategoriesRepository::instance()->categories();
    PlaceCategoryNode node = tree.value(parentId);
    foreach (const QString &childId, node.childIds)
        results.append(tree.value(childId).category);
    return results;
}

QList<QLocale> QPlaceManagerEngineNokiaV1::locales() const
{
    return QPlaceRestManager::instance()->locales();
}

void QPlaceManagerEngineNokiaV1::setLocales(const QList<QLocale> &locales)
{
    QPlaceRestManager::instance()->setLocales(locales);
}


QPlaceManager::ManagerFeatures QPlaceManagerEngineNokiaV1::supportedFeatures() const
{
    return QPlaceManager::RecommendationsFeature
            | QPlaceManager::SearchSuggestionsFeature
            | QPlaceManager::CorrectionsFeature
            | QPlaceManager::LocaleFeature;
}

void QPlaceManagerEngineNokiaV1::processingError(QPlaceReply *reply,
                                              const QPlaceReply::Error &errorId,
                                              const QString &errorMessage)
{
    emit error(reply, errorId, errorMessage);
}

void QPlaceManagerEngineNokiaV1::processingFinished(QPlaceReply *reply)
{
    emit finished(reply);
}
