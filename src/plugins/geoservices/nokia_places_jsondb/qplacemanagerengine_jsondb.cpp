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
** Public License version 3.0 as published by tOhe Free Software Foundation
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
****************************************************************************/

#include "qplacemanagerengine_jsondb.h"

#include <QDebug>
#include <qgeoboundingcircle.h>
#include <jsondb-error.h>

#include "detailsreply.h"
#include "reply.h"
#include "idreply.h"
#include "searchreply.h"
#include "unsupportedreplies.h"

QT_USE_NAMESPACE

Q_DECLARE_METATYPE(QPlaceReply::Error)
Q_DECLARE_METATYPE(QPlaceReply *)

QPlaceManagerEngineJsonDb::QPlaceManagerEngineJsonDb(const QMap<QString, QVariant> &parameters,
                                                     QGeoServiceProvider::Error *error,
                                                     QString *errorString)
:   QPlaceManagerEngine(parameters),
    m_jsonDbHandler(this)

{
    Q_UNUSED(error)
    Q_UNUSED(errorString)

    qRegisterMetaType<QPlaceReply::Error>();
    qRegisterMetaType<QPlaceReply *>();

    connect(&m_jsonDbHandler, SIGNAL(jsonDbResponse(int,QVariant)),
            this, SLOT(processJsonDbResponse(int,QVariant)));
    connect(&m_jsonDbHandler, SIGNAL(jsonDbError(int,int,QString)),
            this, SLOT(processJsonDbError(int,int,QString)));

}

QPlaceManagerEngineJsonDb::~QPlaceManagerEngineJsonDb()
{
}

QPlaceDetailsReply *QPlaceManagerEngineJsonDb::getPlaceDetails(const QString &placeId)
{
    DetailsReply *detailsReply = new DetailsReply(this);
    int reqId = m_jsonDbHandler.queryPlaceDetails(placeId);
    m_idReplyMap.insert(reqId, detailsReply);
    return detailsReply;
}

QPlaceContentReply *QPlaceManagerEngineJsonDb::getContent(const QGeoPlace &place, const QPlaceContentRequest &request)
{
    ContentReply *contentReply = new ContentReply(this);
    contentReply->triggerDone(QPlaceReply::UnsupportedError, tr("Fetching content is unsupported"));
    return contentReply;
}

QPlaceReply *QPlaceManagerEngineJsonDb::postRating(const QString &placeId, qreal value)
{
    Reply *reply = new Reply(this);
    reply->triggerDone(QPlaceReply::UnsupportedError, tr("Posting of ratings is unsupported"));
    return reply;
}

QPlaceSearchReply *QPlaceManagerEngineJsonDb::searchForPlaces(const QPlaceSearchRequest &request)
{
    SearchReply *searchReply = new SearchReply(this);
    searchReply->setRequest(request);

    if (!m_jsonDbHandler.isConnected()) {
        searchReply->triggerDone(QPlaceReply::CommunicationError, "No connection to jsondb database");
        return searchReply;
    }

    if (request.searchArea() != 0) {
        QGeoBoundingArea *searchArea = request.searchArea();
        if (searchArea->type() == QGeoBoundingArea::BoxType) {
            QGeoBoundingBox *box = static_cast<QGeoBoundingBox *>(searchArea);
            if (!box->isValid()) {
                searchReply->triggerDone(QPlaceReply::BadArgumentError,
                                tr("Bounding box search area is invalid"));
                return searchReply;
            }
        } else if (searchArea->type() == QGeoBoundingArea::CircleType) {
            QGeoBoundingCircle *circle = static_cast<QGeoBoundingCircle *>(searchArea);
            if (!circle->center().isValid()) {
                searchReply->triggerDone(QPlaceReply::BadArgumentError,
                                         tr("The center of the search area is an invalid coordinate"));
                return searchReply;
            }
            if (circle->contains(QGeoCoordinate(90,0)) || circle->contains(QGeoCoordinate(-90,0))) {
                searchReply->triggerDone(QPlaceReply::BadArgumentError,
                                         tr("The search area contains the north or south pole"));
                return searchReply;
            }
        }
    }

    QVariant searchJson = JsonDbHandler::convertToJsonVariant(request);
    int reqId = m_jsonDbHandler.query(searchJson);
    m_idReplyMap.insert(reqId, searchReply);
    return searchReply;
}

QPlaceSearchReply *QPlaceManagerEngineJsonDb::recommendations(const QGeoPlace &place, const QPlaceSearchRequest &request)
{
    SearchReply *searchReply = new SearchReply(this);
    searchReply->triggerDone(QPlaceReply::UnsupportedError, tr("Recommendations is unsupported"));
    return searchReply;
}

QPlaceTextPredictionReply *QPlaceManagerEngineJsonDb::textPredictions(const QPlaceSearchRequest &request)
{
    TextPredictionReply *textPredictionReply = new TextPredictionReply(this);
    textPredictionReply->triggerDone(QPlaceReply::UnsupportedError, tr("Text prediction is unsupported"));
    return textPredictionReply;
}

QPlaceManager::ConnectivityModes QPlaceManagerEngineJsonDb::connectivityMode() const
{
    return QPlaceManager::OfflineMode;
}

void QPlaceManagerEngineJsonDb::setConnectivityMode(QPlaceManager::ConnectivityModes mode)
{
    Q_UNUSED(mode)
}

QPlaceManager::ConnectivityModes QPlaceManagerEngineJsonDb::supportedConnectivityModes() const
{
    return QPlaceManager::OfflineMode;
}

QPlaceIdReply *QPlaceManagerEngineJsonDb::savePlace(const QGeoPlace &place, QPlaceManager::VisibilityScope scope)
{
    IdReply *saveReply = new IdReply(QPlaceIdReply::SavePlace, this);
    if (!m_jsonDbHandler.isConnected()) {
        saveReply->triggerDone(QPlaceReply::CommunicationError, "No connection to jsondb database");
        return saveReply;
    }

    if (scope == QPlaceManager::PublicScope) {
        saveReply->triggerDone(QPlaceReply::UnsupportedError, tr("Saving to public scope in unsupported"));
    } else {
        QVariant jsonPlace = JsonDbHandler::convertToJsonVariant(place);
        int reqId;
        if (place.placeId().isEmpty())
            reqId = m_jsonDbHandler.write(jsonPlace);
        else
            reqId = m_jsonDbHandler.update(jsonPlace);

        m_idReplyMap.insert(reqId, saveReply);
    }
    return saveReply;
}

QPlaceManager::VisibilityScopes QPlaceManagerEngineJsonDb::supportedSaveVisibilityScopes() const
{
    return QPlaceManager::NoScope | QPlaceManager::PrivateScope;
}

QPlaceReply *QPlaceManagerEngineJsonDb::removePlace(const QGeoPlace &place)
{
    Reply *reply = new Reply(this);

    if (!m_jsonDbHandler.isConnected()) {
        reply->triggerDone(QPlaceReply::CommunicationError, "No connection to jsondb database");
        return reply;
    }

    int reqId = m_jsonDbHandler.remove(place.placeId());
    m_idReplyMap.insert(reqId, reply);
    return reply;
}

QPlaceReply *QPlaceManagerEngineJsonDb::initializeCategories()
{
    Reply *reply = new Reply(this);
    reply->triggerDone(QPlaceReply::UnsupportedError, tr("Categories are unsupported"));
    return reply;
}

QList<QPlaceCategory> QPlaceManagerEngineJsonDb::categories(const QPlaceCategory &parent) const
{
    Q_UNUSED(parent);
    return QList<QPlaceCategory>();
}

QLocale QPlaceManagerEngineJsonDb::locale() const
{
    return QLocale();
}

void QPlaceManagerEngineJsonDb::setLocale(const QLocale &locale)
{
    Q_UNUSED(locale);
}

void QPlaceManagerEngineJsonDb::processJsonDbResponse(int id, const QVariant &data)
{
    QPlaceReply *reply = m_idReplyMap.value(id,0);
    if (reply) {
        switch (reply->type()) {
        case QPlaceReply::IdReply: {

                /*
                Expected data format
                {
                "uuid":<uuid>,
                "_version": <version>
                }
            */
                IdReply *idReply = qobject_cast<IdReply *>(reply);
                idReply->setId(data.toMap().value(UUID).toString());
                idReply->triggerDone();
                break;
            }
        case QPlaceReply::PlaceSearchReply: {
                SearchReply *searchReply = qobject_cast<SearchReply *>(reply);
                QList<QGeoPlace> places = JsonDbHandler::convertJsonResponseToPlaces(data);
                if (searchReply->request().searchArea() != 0) {
                    QGeoBoundingArea *searchArea = searchReply->request().searchArea();
                    if (searchArea->type() == QGeoBoundingArea::CircleType) {
                        QGeoBoundingCircle *circle = static_cast<QGeoBoundingCircle*>(searchArea);
                        foreach (const QGeoPlace &place, places) {
                            double dist = circle->center().distanceTo(place.location().coordinate());
                            if (dist > circle->radius())
                                places.removeAll(place);
                        }
                        //TODO: we can optimize this using a bounding box to cull candidates
                        //      and then use distance comparisons for the rest.

                    } else if (searchArea->type() == QGeoBoundingArea::BoxType) {
                        //There seem to be some issues with using the comparison operators
                        //so for now we filter in the plugin code
                        QGeoBoundingBox *box = static_cast<QGeoBoundingBox *>(searchArea);
                        double tly = box->topLeft().latitude();
                        double bry = box->bottomRight().latitude();
                        double tlx = box->topLeft().longitude();
                        double brx = box->bottomRight().longitude();

                        QGeoCoordinate coord;
                        foreach (const QGeoPlace &place, places) {
                            coord = place.location().coordinate();
                            if (coord.latitude() > tly)
                                places.removeAll(place);
                            if (coord.latitude() < bry)
                                places.removeAll(place);

                            bool lonWrap = (tlx > brx); //box wraps over the dateline

                            if (!lonWrap) {
                                if (coord.longitude() < tlx || coord.longitude() > brx) {
                                    places.removeAll(place);
                                }
                            } else {
                                if (coord.longitude() < tlx && coord.longitude() > brx) {
                                    places.removeAll(place);
                                }
                            }
                        }
                    }
                }

                QList<QPlaceSearchResult> results;
                QPlaceSearchResult result;
                result.setType(QPlaceSearchResult::Place);
                foreach (const QGeoPlace &place, places) {
                    result.setPlace(place);
                    results.append(result);
                }

                searchReply->setResults(results);
                searchReply->triggerDone();
                break;
            }
        case QPlaceReply::PlaceDetailsReply: {
                QVariantMap jsonMap = data.toMap();
                DetailsReply *detailsReply = qobject_cast<DetailsReply *>(reply);
                if (jsonMap.value("length").toInt() <= 0) {
                    detailsReply->triggerDone(QPlaceReply::PlaceDoesNotExistError,
                                              tr("Specified place does not exist"));
                } else {
                    QList<QGeoPlace> places = JsonDbHandler::convertJsonResponseToPlaces(data);
                    Q_ASSERT(!places.isEmpty());
                    detailsReply->setResult(places.first());
                    detailsReply->triggerDone();
                }
                break;
            }
        case QPlaceReply::Reply: {
                Reply *reply_ = qobject_cast<Reply *>(reply);
                reply_->triggerDone();
                break;
            }
        }
    }
    m_idReplyMap.remove(id);
}

void QPlaceManagerEngineJsonDb::processJsonDbError(int id, int code, const QString &jsonDbErrorString)
{
    QPlaceReply::Error error = QPlaceReply::UnknownError;
    QString errorString = tr("Unknown error occurred operation: jsondb error code =%1, erroString=%2").
                  arg(code).arg(jsonDbErrorString);

    QPlaceReply *placeReply = m_idReplyMap.value(id,0);
    if (placeReply) {
        switch (placeReply->type()) {
        case QPlaceReply::IdReply: {
            IdReply *idReply = qobject_cast<IdReply *>(placeReply);
            switch (idReply->operationType()) {
            case (QPlaceIdReply::SavePlace):
                switch (code) {
                case JsonDbError::MissingObject:
                    error = QPlaceReply::PlaceDoesNotExistError;
                    errorString = tr("Trying to update place which does not exist");
                    break;
                }
                idReply->triggerDone(error, errorString);
                break;
            }
            break;
        }
        case QPlaceReply::PlaceSearchReply: {
                SearchReply *searchReply = qobject_cast<SearchReply*>(placeReply);
                searchReply->triggerDone(error, errorString);
                break;
            }
        case QPlaceReply::Reply: {
                Reply *reply = qobject_cast<Reply*>(placeReply);
                reply->triggerDone(error, errorString);
                break;
            }
        }
        m_idReplyMap.remove(id);
    }
}
