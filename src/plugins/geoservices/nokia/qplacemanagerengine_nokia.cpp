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
** This file is part of the Ovi services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file OVI_SERVICES_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Ovi services
** plugin source code.
**
****************************************************************************/

#include "qplacemanagerengine_nokia.h"

#include <QtLocation/QPlaceContentRequest>

#include "places/qplacecategoriesrepository.h"
#include "places/qplacecontentreplyimpl.h"
#include "places/qplacetextpredictionreplyimpl.h"
#include "places/qplacesearchreplyimpl.h"
#include "places/qplacerecommendationreplyimpl.h"
#include "places/qplacedetailsreplyimpl.h"
#include "places/qplaceratingreplyimpl.h"
#include "places/qplacerestmanager.h"
#include "places/qplacerestreply.h"

QT_USE_NAMESPACE
Q_DECLARE_METATYPE(QPlaceReply::Error)

QPlaceManagerEngineNokia::QPlaceManagerEngineNokia(const QMap<QString, QVariant> &parameters,
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

QPlaceManagerEngineNokia::~QPlaceManagerEngineNokia()
{
}

QPlaceDetailsReply *QPlaceManagerEngineNokia::getPlaceDetails(const QString &placeId)
{
    QPlaceDetailsReplyImpl *reply = NULL;
    QPlaceRestReply *restReply = QPlaceRestManager::instance()->sendPlaceRequest(placeId);
    if (restReply) {
        reply = new QPlaceDetailsReplyImpl(restReply, manager(), this);
        connect(reply, SIGNAL(processingError(QPlaceReply*,QPlaceReply::Error,QString)),
                this, SLOT(processingError(QPlaceReply*,QPlaceReply::Error,QString)));
        connect(reply, SIGNAL(processingFinished(QPlaceReply*)),
                this, SLOT(processingFinished(QPlaceReply*)));
    }
    return reply;
}

QPlaceContentReply *QPlaceManagerEngineNokia::getContent(const QPlace &place, const QPlaceContentRequest &request)
{
    QPlaceContentReplyImpl *reply;
    switch (request.contentType()) {
    case QPlaceContent::ImageType: {
            QPlaceRestReply *restReply = QPlaceRestManager::instance()->sendPlaceImagesRequest(place.placeId(),
                                                                                               request);
            reply = new QPlaceContentReplyImpl(request.contentType(), restReply, manager(), this);

            if (!restReply)
                QMetaObject::invokeMethod(reply, "restError", Qt::QueuedConnection,
                                          Q_ARG(QPlaceReply::Error, QPlaceReply::UnknownError),
                                          Q_ARG(QString, QString("Could not create rest reply for image content request")));
            break;
        }
    case QPlaceContent::ReviewType: {
            QPlaceRestReply *restReply = QPlaceRestManager::instance()->sendPlaceReviewRequest(place.placeId(),
                                                                                               request);
            reply = new QPlaceContentReplyImpl(request.contentType(), restReply, manager(), this);

            if (!restReply)
                QMetaObject::invokeMethod(reply, "restError", Qt::QueuedConnection,
                                          Q_ARG(QPlaceReply::Error, QPlaceReply::UnknownError),
                                          Q_ARG(QString, QString("Could not create rest reply for review content request")));
            break;
        }
    case QPlaceContent::EditorialType: {
        QPlaceRestReply *restReply =
            QPlaceRestManager::instance()->sendPlaceRequest(place.placeId());

        reply = new QPlaceContentReplyImpl(request.contentType(), restReply, manager(), this);

        if (!restReply) {
            QMetaObject::invokeMethod(reply, "restError", Qt::QueuedConnection,
                                      Q_ARG(QPlaceReply::Error, QPlaceReply::UnknownError),
                                      Q_ARG(QString, QString("Could not create rest reply for editorial content request")));
        }

        break;
    }
    default: {
            reply = new QPlaceContentReplyImpl(request.contentType(), 0, manager(), this);
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

QPlaceSearchReply *QPlaceManagerEngineNokia::search(const QPlaceSearchRequest &query)
{
    //TODO: handling of scope
    QPlaceSearchReplyImpl *reply = NULL;
    QPlaceSearchRequest newQuery = query;
    // search by category
    if (newQuery.categories().count()) {
        newQuery.setSearchTerm(query.categories().at(0).name());
    }

    if (query.visibilityScope() == QtLocation::UnspecifiedVisibility ||
        query.visibilityScope() == QtLocation::PublicVisibility) {

        QPlaceRestReply *restReply = QPlaceRestManager::instance()->sendSearchRequest(newQuery);

        if (restReply) {
            reply = new QPlaceSearchReplyImpl(restReply, this);
            connect(reply, SIGNAL(processingError(QPlaceReply*,QPlaceReply::Error,QString)),
                    this, SLOT(processingError(QPlaceReply*,QPlaceReply::Error,QString)));
            connect(reply, SIGNAL(processingFinished(QPlaceReply*)),
                    this, SLOT(processingFinished(QPlaceReply*)));
        }
    } else {
        reply = new QPlaceSearchReplyImpl(0,this);
        QMetaObject::invokeMethod(reply,
                                  "setError",
                                  Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply::Error, QPlaceReply::UnsupportedError),
                                  Q_ARG(QString, "Searching for places with unsupported visibility scope"));

    }
    return reply;
}

QPlaceSearchReply *QPlaceManagerEngineNokia::recommendations(const QPlace &place, const QPlaceSearchRequest &query)
{
    QPlaceRecommendationReplyImpl *reply = NULL;
    QPlaceSearchRequest newQuery = query;
    newQuery.setSearchTerm(place.placeId());
    QPlaceRestReply *restReply = QPlaceRestManager::instance()->sendRecommendationRequest(newQuery, QString());
    if (restReply) {
        reply = new QPlaceRecommendationReplyImpl(restReply, manager(), this);
        connect(reply, SIGNAL(processingError(QPlaceReply*,QPlaceReply::Error,QString)),
                this, SLOT(processingError(QPlaceReply*,QPlaceReply::Error,QString)));
        connect(reply, SIGNAL(processingFinished(QPlaceReply*)),
                this, SLOT(processingFinished(QPlaceReply*)));
    }
    return reply;
}

QPlaceTextPredictionReply *QPlaceManagerEngineNokia::textPredictions(const QPlaceSearchRequest &query)
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

QPlaceIdReply *QPlaceManagerEngineNokia::savePlace(const QPlace &place)
{
    Q_UNUSED(place)

    //TODO: implementation
    return 0;
}

QPlaceIdReply *QPlaceManagerEngineNokia::removePlace(const QPlace &place)
{
    Q_UNUSED(place)

    //TODO: implementation
    return 0;
}

QPlaceIdReply *QPlaceManagerEngineNokia::saveCategory(const QPlaceCategory &category, const QString &parentId)
{
    Q_UNUSED(category)
    Q_UNUSED(parentId)

    return 0;
}

QPlaceIdReply *QPlaceManagerEngineNokia::removeCategory(const QString &categoryId)
{
    Q_UNUSED(categoryId)

    return 0;
}

QPlaceReply *QPlaceManagerEngineNokia::initializeCategories()
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

QList<QPlaceCategory> QPlaceManagerEngineNokia::childCategories(const QString &parentId) const
{
    QPlaceCategoryTree tree = QPlaceCategoriesRepository::instance()->categories();
    tree = tree.findCategoryTreeById(parentId);

    QList<QPlaceCategory> results;

    QHashIterator<QString, QPlaceCategoryTree> it(tree.subCategories);
    while (it.hasNext()) {
        it.next();

        results.append(it.value().category);
    }

    return results;
}

QLocale QPlaceManagerEngineNokia::locale() const
{
    return QPlaceRestManager::instance()->locale();
}

void QPlaceManagerEngineNokia::setLocale(const QLocale &locale)
{
    QPlaceRestManager::instance()->setLocale(locale);
}

QUrl QPlaceManagerEngineNokia::constructIconUrl(const QPlaceIcon &icon, const QSize &size, QPlaceIcon::IconFlags flags)
{
    Q_UNUSED(icon)
    Q_UNUSED(size)
    Q_UNUSED(flags)
    return QUrl();
}

QPlaceManager::ManagerFeatures QPlaceManagerEngineNokia::supportedFeatures() const
{
    return QPlaceManager::RecommendationsFeature
            | QPlaceManager::TextPredictionsFeature
            | QPlaceManager::CorrectionsFeature
            | QPlaceManager::LocaleFeature;
}

void QPlaceManagerEngineNokia::processingError(QPlaceReply *reply,
                                              const QPlaceReply::Error &errorId,
                                              const QString &errorMessage)
{
    emit error(reply, errorId, errorMessage);
}

void QPlaceManagerEngineNokia::processingFinished(QPlaceReply *reply)
{
    emit finished(reply);
}
