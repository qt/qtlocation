/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#include "places/qplacecategoriesrepository.h"
#include "places/qplacetextpredictionreplyimpl.h"
#include "places/qplacesearchreplyimpl.h"
#include "places/qplacereviewreplyimpl.h"
#include "places/qplacemediareplyimpl.h"
#include "places/qplacerecommendationreplyimpl.h"
#include "places/qplacedetailsreplyimpl.h"
#include "places/qplaceratingreplyimpl.h"
#include "places/qplacerestmanager.h"
#include "places/qplacerestreply.h"

QT_USE_NAMESPACE

QPlaceManagerEngineNokia::QPlaceManagerEngineNokia(const QMap<QString, QVariant> &parameters,
                                                 QGeoServiceProvider::Error *error,
                                                 QString *errorString)
:   QPlaceManagerEngine(parameters)
{
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
        reply = new QPlaceDetailsReplyImpl(restReply, this);
        connect(reply, SIGNAL(processingError(QPlaceReply*,QPlaceReply::Error,QString)),
                this, SLOT(processingError(QPlaceReply*,QPlaceReply::Error,QString)));
        connect(reply, SIGNAL(processingFinished(QPlaceReply*)),
                this, SLOT(processingFinished(QPlaceReply*)));
    }
    return reply;
}

QPlaceMediaReply *QPlaceManagerEngineNokia::getMedia(const QGeoPlace &place, const QPlaceQuery &query)
{
    QPlaceMediaReplyImpl *reply = NULL;
    QPlaceRestReply *restReply = QPlaceRestManager::instance()->sendPlaceImagesRequest(place.placeId(),
                                                                                       query);
    if (restReply) {
        reply = new QPlaceMediaReplyImpl(restReply, this);
        reply->setStartNumber(query.offset());
        connect(reply, SIGNAL(processingError(QPlaceReply*,QPlaceReply::Error,QString)),
                this, SLOT(processingError(QPlaceReply*,QPlaceReply::Error,QString)));
        connect(reply, SIGNAL(processingFinished(QPlaceReply*)),
                this, SLOT(processingFinished(QPlaceReply*)));
    }
    return reply;
}

QPlaceReply *QPlaceManagerEngineNokia::postRating(const QString &placeId, qreal value)
{
    QPlaceRatingReplyImpl *reply = NULL;
    //TODO: need to post user name
    QPlaceRestReply *restReply = QPlaceRestManager::instance()->postRatingRequest(placeId,
                                                                                  QString(),
                                                                                  value);
    if (restReply) {
        reply = new QPlaceRatingReplyImpl(restReply, this);
        connect(reply, SIGNAL(processingError(QPlaceReply*,QPlaceReply::Error,QString)),
                this, SLOT(processingError(QPlaceReply*,QPlaceReply::Error,QString)));
        connect(reply, SIGNAL(processingFinished(QPlaceReply*)),
                this, SLOT(processingFinished(QPlaceReply*)));
    }
    return reply;
}

QPlaceReviewReply *QPlaceManagerEngineNokia::getReviews(const QGeoPlace &place, const QPlaceQuery &query)
{
    QPlaceReviewReplyImpl *reply = NULL;
    QPlaceRestReply *restReply = QPlaceRestManager::instance()->sendPlaceReviewRequest(place.placeId(),
                                                                                       query);
    if (restReply) {
        reply = new QPlaceReviewReplyImpl(restReply, this);
        reply->setStartNumber(query.offset());
        connect(reply, SIGNAL(processingError(QPlaceReply*,QPlaceReply::Error,QString)),
                this, SLOT(processingError(QPlaceReply*,QPlaceReply::Error,QString)));
        connect(reply, SIGNAL(processingFinished(QPlaceReply*)),
                this, SLOT(processingFinished(QPlaceReply*)));
    }
    return reply;
}

QPlaceSearchReply *QPlaceManagerEngineNokia::searchForPlaces(const QPlaceSearchQuery &query, QPlaceManager::VisibilityScope scope)
{
    //TODO: handling of scope
    QPlaceSearchReplyImpl *reply = NULL;
    QPlaceSearchQuery newQuery = query;
    // search by category
    if (newQuery.categories().count()) {
        newQuery.setSearchTerm(query.categories().at(0).name());
    }
    QPlaceRestReply *restReply = QPlaceRestManager::instance()->sendSearchRequest(newQuery);

    if (restReply) {
        reply = new QPlaceSearchReplyImpl(restReply, this);
        connect(reply, SIGNAL(processingError(QPlaceReply*,QPlaceReply::Error,QString)),
                this, SLOT(processingError(QPlaceReply*,QPlaceReply::Error,QString)));
        connect(reply, SIGNAL(processingFinished(QPlaceReply*)),
                this, SLOT(processingFinished(QPlaceReply*)));
    }
    return reply;
}

QPlaceManager::VisibilityScopes QPlaceManagerEngineNokia::supportedSearchVisibilityScopes() const
{
    return QPlaceManager::PublicScope;
}

QPlaceSearchReply *QPlaceManagerEngineNokia::recommendations(const QGeoPlace &place, const QPlaceSearchQuery &query)
{
    QPlaceRecommendationReplyImpl *reply = NULL;
    QPlaceSearchQuery newQuery = query;
    newQuery.setSearchTerm(place.placeId());
    QPlaceRestReply *restReply = QPlaceRestManager::instance()->sendRecommendationRequest(newQuery, QString());
    if (restReply) {
        reply = new QPlaceRecommendationReplyImpl(restReply, this);
        connect(reply, SIGNAL(processingError(QPlaceReply*,QPlaceReply::Error,QString)),
                this, SLOT(processingError(QPlaceReply*,QPlaceReply::Error,QString)));
        connect(reply, SIGNAL(processingFinished(QPlaceReply*)),
                this, SLOT(processingFinished(QPlaceReply*)));
    }
    return reply;
}

QPlaceTextPredictionReply *QPlaceManagerEngineNokia::textPredictions(const QPlaceSearchQuery &query)
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

QPlaceManager::ConnectivityModes QPlaceManagerEngineNokia::connectivityMode() const
{
    return QPlaceManager::OnlineMode;
}

void QPlaceManagerEngineNokia::setConnectivityMode(QPlaceManager::ConnectivityModes mode)
{
    Q_UNUSED(mode)
    // only QPlaceManager::OnlineMode is suppoerted
}

QPlaceManager::ConnectivityModes QPlaceManagerEngineNokia::supportedConnectivityModes() const
{
    return QPlaceManager::OnlineMode;
}

QPlaceReply *QPlaceManagerEngineNokia::savePlace(QGeoPlace *place, QPlaceManager::VisibilityScope scope) {
    //TODO: implementation
    return NULL;
}

QPlaceManager::VisibilityScopes QPlaceManagerEngineNokia::supportedSaveVisibilityScopes() const {
    return QPlaceManager::NoScope;
}

QPlaceReply *QPlaceManagerEngineNokia::removePlace(const QGeoPlace &place) {
    //TODO: implementation
    return NULL;
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

QList<QPlaceCategory> QPlaceManagerEngineNokia::categories() const
{
    return QPlaceCategoriesRepository::instance()->categories();
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
