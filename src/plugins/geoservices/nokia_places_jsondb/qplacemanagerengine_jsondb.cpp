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
****************************************************************************/

#include "qplacemanagerengine_jsondb.h"

#include <QDebug>
#include <QtCore/qnumeric.h>
#include <qgeoboundingcircle.h>
#include <jsondb-error.h>

#include "detailsreply.h"
#include "reply.h"
#include "idreply.h"
#include "searchreply.h"
#include "unsupportedreplies.h"


//Json db response key constants
#define LENGTH QLatin1String("length")
#define DATA QLatin1String("data")

QT_USE_NAMESPACE

Q_DECLARE_METATYPE(QPlaceReply::Error)
Q_DECLARE_METATYPE(QPlaceReply *)

QPlaceManagerEngineJsonDb::QPlaceManagerEngineJsonDb(const QMap<QString, QVariant> &parameters,
                                                     QGeoServiceProvider::Error *error,
                                                     QString *errorString)
:   QPlaceManagerEngine(parameters),
    m_jsonDbHandler(this)

{
    qRegisterMetaType<QPlaceReply::Error>();
    qRegisterMetaType<QPlaceReply *>();
    qRegisterMetaType<QPlaceCategory>();

    connect(&m_jsonDbHandler, SIGNAL(jsonDbResponse(int,QVariant)),
            this, SLOT(processJsonDbResponse(int,QVariant)));
    connect(&m_jsonDbHandler, SIGNAL(jsonDbError(int,int,QString)),
            this, SLOT(processJsonDbError(int,int,QString)));
    *error = QGeoServiceProvider::NoError;
    errorString->clear();

}

QPlaceManagerEngineJsonDb::~QPlaceManagerEngineJsonDb()
{
}

QPlaceDetailsReply *QPlaceManagerEngineJsonDb::getPlaceDetails(const QString &placeId)
{
    DetailsReply *detailsReply = new DetailsReply(this);
    int reqId = m_jsonDbHandler.queryByUuid(placeId);
    m_idReplyMap.insert(reqId, detailsReply);
    return detailsReply;
}

QPlaceContentReply *QPlaceManagerEngineJsonDb::getContent(const QPlace &place, const QPlaceContentRequest &request)
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
            if (!circle->center().isValid() || qIsNaN(circle->center().latitude()) || qIsNaN(circle->center().longitude())) {
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

    QString searchQuery = JsonDbHandler::convertToQueryString(request);
    int reqId = m_jsonDbHandler.query(searchQuery);
    m_idReplyMap.insert(reqId, searchReply);
    return searchReply;
}

QPlaceSearchReply *QPlaceManagerEngineJsonDb::recommendations(const QPlace &place, const QPlaceSearchRequest &request)
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

QPlaceIdReply *QPlaceManagerEngineJsonDb::savePlace(const QPlace &place)
{
    IdReply *saveReply = new IdReply(QPlaceIdReply::SavePlace, this);
    if (!m_jsonDbHandler.isConnected()) {
        saveReply->triggerDone(QPlaceReply::CommunicationError, "No connection to jsondb database");
        return saveReply;
    }

    if (place.visibility() & QtLocation::PublicVisibility) {
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

QPlaceIdReply *QPlaceManagerEngineJsonDb::saveCategory(const QPlaceCategory &category, const QString &parentId)
{
    IdReply *saveReply = new IdReply(QPlaceIdReply:: SaveCategory ,this);
    if (!m_jsonDbHandler.isConnected()) {
        saveReply->triggerDone(QPlaceReply::CommunicationError, "No connection to jsondb database");
        return saveReply;
    }

    QVariantMap categoryMap;
    QVariantMap parentMap;

    bool isUpdate = false;
    bool isSameParent = false;

    int reqId;

    categoryMap = m_jsonDbHandler.findCategoryJson(category.categoryId());
    if (!categoryMap.isEmpty()) {
        isUpdate = true;
        //check if the existing parent is different from the new one
        //if it is, then resave the old parent without the category as a child
        parentMap = m_jsonDbHandler.findParentCategoryJson(category.categoryId());
        if (!parentMap.isEmpty()) {
            if (parentMap.value(UUID) != parentId) {
                QList<QVariant> siblingUuids = parentMap.value(CHILDREN_UUIDS).toList();
                siblingUuids.removeAll(QVariant(category.categoryId()));
                parentMap.insert(CHILDREN_UUIDS, siblingUuids);
                reqId = m_jsonDbHandler.update(parentMap);
                m_jsonDbHandler.waitForRequest(reqId);
            } else {
                isSameParent = true;
            }
        }
    }

    //check if the parent category exists
    bool isTopLevel = parentId.isEmpty();
    if (!isTopLevel) {
        if (!isSameParent) {
            parentMap = m_jsonDbHandler.findCategoryJson(parentId);
            if (parentMap.isEmpty()) {
                saveReply->triggerDone(QPlaceReply::CategoryDoesNotExistError, tr("Parent category does not exist"));
                return saveReply;
            }
        }
    } else {
        parentMap.clear();
    }

    if (!isUpdate) {
        reqId = m_jsonDbHandler.write(JsonDbHandler::convertToJsonVariant(category, isTopLevel));
    } else {
        categoryMap.insert(DISPLAY_NAME, category.name());
        reqId = m_jsonDbHandler.update(categoryMap);
        saveReply->setIsUpdate();
    }

    QVariantMap responseMap = m_jsonDbHandler.waitForRequest(reqId);
    if (!responseMap.contains(UUID)) {
        saveReply->triggerDone(QPlaceReply::UnknownError, tr("Could not save category"));
        return saveReply;
    }

    QString categoryUuid = responseMap.value(UUID).toString();
    saveReply->setId(categoryUuid);

    if (!isTopLevel) {
        //if the specified parent is a new parent, save the specified parent with it's new child
        if (!isSameParent) {
            QList<QVariant> childrenUuids = parentMap.value(CHILDREN_UUIDS).toList();
            childrenUuids.append(categoryUuid);
            parentMap.insert(CHILDREN_UUIDS, childrenUuids);
            reqId = m_jsonDbHandler.update(parentMap);
            m_idReplyMap.insert(reqId, saveReply);
            return saveReply;
        }
    }

    //TODO: this variable is only needed for for the workaround for emitting signals
    // when jsondb notifications are introduced this should be removed.
    QPlaceCategory category_ = m_jsonDbHandler.findCategory(saveReply->id());

    saveReply->setId(categoryUuid);
    saveReply->triggerDone();

    if (!saveReply->isUpdate())
        emit categoryAdded(category_, parentMap.value(UUID).toString());
    else
        emit categoryUpdated(category_, parentMap.value(UUID).toString());

    return saveReply;
}

QPlaceIdReply *QPlaceManagerEngineJsonDb::removePlace(const QPlace &place)
{
    IdReply *removeReply = new IdReply(QPlaceIdReply::RemovePlace, this);

    if (!m_jsonDbHandler.isConnected()) {
        removeReply->triggerDone(QPlaceReply::CommunicationError, "No connection to jsondb database");
        return removeReply;
    }

    int reqId = m_jsonDbHandler.remove(place.placeId());
    removeReply->setId(place.placeId());
    m_idReplyMap.insert(reqId, removeReply);
    return removeReply;
}

QPlaceIdReply *QPlaceManagerEngineJsonDb::removeCategory(const QString &categoryId)
{
    IdReply *removeReply = new IdReply(QPlaceIdReply::RemoveCategory, this);
    removeReply->setId(categoryId);
    if (categoryId.isEmpty()) {
        removeReply->triggerDone(QPlaceReply::CategoryDoesNotExistError, tr("Trying to remove category with empty category id"));
        return removeReply;
    }

    //check if category exists
    QVariantMap categoryMap = m_jsonDbHandler.findCategoryJson(categoryId);
    if (categoryMap.isEmpty()) {
        removeReply->triggerDone(QPlaceReply::CategoryDoesNotExistError, tr("Trying to remove non-existent category"));
        return removeReply;
    }

    QVariantMap parentCategoryMap = m_jsonDbHandler.findParentCategoryJson(categoryId);
    recursiveRemoveHelper(categoryId, parentCategoryMap.value(UUID).toString());

    removeReply->setParentCategoryId(parentCategoryMap.value(UUID).toString());
    removeReply->setId(categoryId);

    //resave the parent without the given category as a child
    int reqId;
    if (!parentCategoryMap.isEmpty()) {
        QStringList childrenUuids;
        childrenUuids = parentCategoryMap.value(CHILDREN_UUIDS).toStringList();
        childrenUuids.removeAll(categoryId);
        parentCategoryMap.insert(CHILDREN_UUIDS, childrenUuids);
        reqId = m_jsonDbHandler.update(parentCategoryMap);
        m_idReplyMap.insert(reqId, removeReply);
    } else {
        //trigger the reply as done
        removeReply->triggerDone();
        emit categoryRemoved(removeReply->id(), removeReply->parentCategoryId());
    }

    return removeReply;
}

QPlaceReply *QPlaceManagerEngineJsonDb::initializeCategories()
{
    Reply *reply = new Reply(this);
    reply->triggerDone();
    return reply;
}

QList<QPlaceCategory> QPlaceManagerEngineJsonDb::childCategories(const QString &parentId) const
{
    int reqId;

    if (parentId.isEmpty()) {
        reqId= m_jsonDbHandler.query(QString("[?") + TYPE + QLatin1String(" = \"") + PLACE_CATEGORY_TYPE + QLatin1String("\"]")
                                               + QLatin1String("[?") + TOP_LEVEL_CATEGORY + QLatin1String(" = true]"));
    } else {
        reqId= m_jsonDbHandler.query(QString("[?") + TYPE + QLatin1String(" = \"") + PLACE_CATEGORY_TYPE + QLatin1String("\"]")
                                               + QLatin1String("[?") + UUID + QLatin1String(" = \"") + parentId + QLatin1String("\"]"));

        QVariantMap responseMap = m_jsonDbHandler.waitForRequest(reqId);

        if (responseMap.value(LENGTH).toInt() <= 0)
            return QList<QPlaceCategory>();

        QVariantMap categoryMap = responseMap.value("data").toList().at(0).toMap();
        QStringList childUuids = categoryMap.value(CHILDREN_UUIDS).toStringList();
        for (int i=0; i < childUuids.count(); ++i)
            childUuids[i].prepend("\"").append("\"");
        QString queryString = QString("[?") + TYPE + QLatin1String(" = \"") + PLACE_CATEGORY_TYPE + QLatin1String("\"]")
                        + QLatin1String("[?") + UUID + QLatin1String(" in [") + childUuids.join(",") + QLatin1String("]]");
        reqId= m_jsonDbHandler.query(queryString);
    }

    QVariantMap responseMap = m_jsonDbHandler.waitForRequest(reqId);
    return JsonDbHandler::convertJsonResponseToCategories(responseMap);
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
                IdReply *idReply = qobject_cast<IdReply *>(reply);

                switch (idReply->operationType()) {
                case QPlaceIdReply::SavePlace:
                        /*
                        Expected data format
                        {
                        "uuid":<uuid>,
                        "_version": <version>
                        }*/
                    idReply->setId(data.toMap().value(UUID).toString());
                    break;
                case QPlaceIdReply::RemovePlace:{
                    /*
                    Expected data format example
                    {
                    "count":1,
                    "data":[
                        {
                        "_uuid":"8c196304-509c-5c45-0a07-0ea25a280f10523d"
                        }],
                    "error":[]
                    }*/
                    QVariantMap jsonResponse = data.toMap();
                    if (jsonResponse.value(QLatin1String("count")).toInt() >= 0) {
                        QVariantMap jsonResponse = data.toMap();
                        QString uuid = jsonResponse.value(QLatin1String("data"))
                                .toList().at(0).toMap().value(UUID).toString();
                        if (uuid != idReply->id()) {
                            idReply->triggerDone(QPlaceReply::UnknownError,
                                                 tr("JsonDb Response UUID does not match that in request"
                                                    "for a removal operation\n"
                                                    "JsonDb UUID: %1"
                                                    "Request UUID: %2").arg(uuid).arg(idReply->id()));
                        }
                    } else {
                        idReply->triggerDone(QPlaceReply::UnknownError,
                                             tr("JsonDb response does not contain a uuid"
                                                "for a removal request"));
                    }

                    if (!jsonResponse.value(QLatin1String("error")).toStringList().isEmpty()
                            && idReply->error() == QPlaceReply::NoError) {
                        idReply->triggerDone(QPlaceReply::UnknownError,
                                             tr("JsonDb response had unexpected errors: %1")
                                             .arg(jsonResponse.value(QLatin1String("error"))
                                                    .toStringList().join(QLatin1String(","))));
                    }

                    break;
                }
                case QPlaceIdReply::RemoveCategory:
                case QPlaceIdReply::SaveCategory:
                    //id should've already been set, note the ids from the response
                    //won't match those from the reply since the response might be
                    //be changes to the parent category
                    break;
                default:
                    //Other types should not be possible
                    break;
                }

                if (idReply->error() == QPlaceReply::NoError)
                    idReply->triggerDone();

                //TODO: This is workaround code, it is intended that we use
                //       jsondb notifications to emit the added, updated
                //       and removed signals.
                switch (idReply->operationType()) {
                    case QPlaceIdReply::RemovePlace:
                        emit placeRemoved(idReply->id());
                        break;
                    case QPlaceIdReply::SavePlace:
                        emit placeAdded(idReply->id());
                        break;
                    case QPlaceIdReply::RemoveCategory:
                        emit categoryRemoved(idReply->id(), idReply->parentCategoryId());
                        break;
                    case QPlaceIdReply::SaveCategory:
                        //the uuid in the response here is expected to be that of the parent
                        //the child uuid is already set in the reply.
                        if (idReply->isUpdate())
                            emit categoryUpdated(m_jsonDbHandler.findCategory(idReply->id()), data.toMap().value(UUID).toString());
                        else
                            emit categoryAdded(m_jsonDbHandler.findCategory(idReply->id()), data.toMap().value(UUID).toString());
                        break;
                default:
                    //no other cases should exist
                    break;
                }
                break;
            }
        case QPlaceReply::PlaceSearchReply: {
                SearchReply *searchReply = qobject_cast<SearchReply *>(reply);
                QList<QPlace> places = JsonDbHandler::convertJsonResponseToPlaces(data);
                QList<QPlaceSearchResult> results;
                QPlaceSearchResult result;
                result.setType(QPlaceSearchResult::Place);
                bool resultsAlreadySet = false;

                if (searchReply->request().searchArea() != 0) {
                    QGeoBoundingArea *searchArea = searchReply->request().searchArea();
                    if (searchArea->type() == QGeoBoundingArea::CircleType) {
                        QGeoBoundingCircle *circle = static_cast<QGeoBoundingCircle*>(searchArea);
                        foreach (const QPlace &place, places) {
                            qreal dist = circle->center().distanceTo(place.location().coordinate());
                            if (dist < circle->radius() || qFuzzyCompare(dist, circle->radius()) || circle->radius() < 0.0) {
                                result.setDistance(dist);
                                result.setPlace(place);

                                if (searchReply->request().relevanceHint() == QPlaceSearchRequest::DistanceHint) {
                                    //TODO: we can optimize this insertion sort
                                    bool added = false;
                                    for (int i=0; i < results.count(); ++i) {
                                        if (result.distance() < results.at(i).distance() || qFuzzyCompare(result.distance(),results.at(i).distance())) {
                                            results.insert(i, result);
                                            added = true;
                                            break;
                                        }
                                    }
                                    if (!added)
                                        results.append(result);
                                } else {
                                    results.append(result);
                                }
                            }
                        }
                        resultsAlreadySet = true;

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
                        foreach (const QPlace &place, places) {
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

                //if we haven't already filled in the results, fill them in now
                if (!resultsAlreadySet) {
                    foreach (const QPlace &place, places) {
                        result.setPlace(place);
                        results.append(result);
                    }
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
                    QList<QPlace> places = JsonDbHandler::convertJsonResponseToPlaces(data);
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
        m_idReplyMap.remove(id);
    }
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
            switch (code) {
            case JsonDbError::MissingObject:
                switch (idReply->operationType()) {
                case QPlaceIdReply::SavePlace:
                case QPlaceIdReply::RemovePlace:
                    error = QPlaceReply::PlaceDoesNotExistError;
                    errorString = tr("Place does not exist");
                    break;
                case QPlaceIdReply::SaveCategory:
                case QPlaceIdReply::RemoveCategory:
                    error = QPlaceReply::CategoryDoesNotExistError;
                    errorString = tr("Category does not exist");
                }
            }
            idReply->triggerDone(error, errorString);
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

void QPlaceManagerEngineJsonDb::recursiveRemoveHelper(const QString &categoryId, const QString &parentId)
{

    QVariantMap categoryMap = m_jsonDbHandler.findCategoryJson(categoryId);
    if (categoryMap.isEmpty())
        return;

    QStringList childUuids = categoryMap.value(CHILDREN_UUIDS).toStringList();
    foreach (const QString &uuid, childUuids)
        recursiveRemoveHelper(uuid, categoryId);

    int reqId = m_jsonDbHandler.remove(categoryId);
    m_jsonDbHandler.waitForRequest(reqId);
    emit categoryRemoved(categoryId, parentId);
}
