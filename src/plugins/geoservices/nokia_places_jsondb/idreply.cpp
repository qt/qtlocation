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
****************************************************************************/

#include "idreply.h"
#include "qplacemanagerengine_jsondb.h"
#include "jsonconverter.h"

#include <QtAddOnJsonDb/jsondb-client.h>
#include <QtAddOnJsonDb/jsondb-error.h>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QUuid>


IdReply::IdReply(QPlaceIdReply::OperationType operationType,
                 QPlaceManagerEngineJsonDb *engine)
    : QPlaceIdReply(operationType, engine),m_engine(engine), m_isUpdate(false)
{
}

IdReply::~IdReply()
{
}

void IdReply::setId(const QString &id)
{
    QPlaceIdReply::setId(id);
}

bool IdReply::isUpdate() const
{
    return m_isUpdate;
}

void IdReply::setIsUpdate()
{
    m_isUpdate = true;
}

QString IdReply::parentCategoryId() const
{
    return m_parentCategoryId;
}

void IdReply::setParentCategoryId(const QString &parentId)
{
    m_parentCategoryId = parentId;
}

void IdReply::start()
{
}

SavePlaceReply::SavePlaceReply(QPlaceManagerEngineJsonDb *engine)
    : IdReply(QPlaceIdReply::SavePlace, engine), currIconIndex(0)
{
}

SavePlaceReply::~SavePlaceReply()
{
}

void SavePlaceReply::setPlace(const QPlace &place)
{
    m_place = place;
    setId(m_place.placeId());
}

void SavePlaceReply::start()
{
    connect(db(), SIGNAL(response(int,QVariant)), this, SLOT(processResponse(int,QVariant)));
    connect(db(), SIGNAL(error(int,int,QString)), this, SLOT(processError(int,int,QString)));

    if (m_place.visibility() & QtLocation::PublicVisibility) {
        triggerDone(QPlaceReply::UnsupportedError, QString::fromLatin1("Saving to public scope in unsupported"));
        return;
    } else {
        if (m_place.placeId().isEmpty()) {
            enterGetCategoriesState();
        } else {
            m_state = SavePlaceReply::CheckIfExists;
            m_reqId = db()->query(QString::fromLatin1("[?%1=\"%2\"][?%3 = \"%4\"]").arg(JsonConverter::Type).arg(JsonConverter::PlaceType)
                                  .arg(JsonConverter::Uuid).arg(m_place.placeId()));
        }
    }
}

void SavePlaceReply::enterGetCategoriesState()
{
    QVariantList categoryUuids;
    foreach (const QPlaceCategory &category, m_place.categories())
        categoryUuids.append(category.categoryId());

    QString queryString = QString::fromLatin1("[?%1=\"%2\"]").arg(JsonConverter::Type).arg(JsonConverter::CategoryType)
            + QString::fromLatin1("[?%1 in %categoryUuids]").arg(JsonConverter::Uuid);

    QVariantMap bindingsMap;
    bindingsMap.insert(QLatin1String("categoryUuids"), categoryUuids);

    QVariantMap queryObj;
    queryObj.insert(JsonConverter::Query, queryString);
    queryObj.insert(JsonConverter::Bindings, bindingsMap);

    m_state = SavePlaceReply::GetCategories;
    m_reqId = db()->find(queryObj);
}

void SavePlaceReply::enterGetIconsState()
{
    m_state = SavePlaceReply::GetIcons;
    QStringList prefixes;
    prefixes << "small" << "medium" << "large" << "fullscreen";

    QList<QUrl> uniqueInputUrls; //unique source urls that have been supplied without associated destinations
    foreach (const QString &prefix, prefixes) {
        bool ok;
        QUrl sourceUrl = convertToUrl(m_place.icon().parameters().value(prefix + QLatin1String("SourceUrl"), QUrl()), &ok);
        if (!ok) {
            triggerDone(QPlaceReply::BadArgumentError, QString::fromLatin1("icon parameter for key: ") + prefix + QLatin1String("SourceUrl")
                        + QLatin1String(" was not a QUrl object"));
            return;
        }
        QUrl destinationUrl = convertToUrl(m_place.icon().parameters().value(prefix + QLatin1String("Url"), QUrl()), &ok);
        if (!ok) {
            triggerDone(QPlaceReply::BadArgumentError, QString::fromLatin1("icon parameter for key: ") + prefix + QLatin1String("Url")
                        + QLatin1String(" was not a QUrl object"));
            return;
        }

        if (destinationUrl.isEmpty()) {
            if (sourceUrl.isEmpty() || uniqueInputUrls.contains(sourceUrl))
                continue;
            else
                uniqueInputUrls.append(sourceUrl);
        }

        QString destination;
        if (!destinationUrl.isEmpty())
            destination = prefix + QLatin1String("Url");

        Icon *icon = new Icon(sourceUrl, destinationUrl, destination);
        if (m_place.icon().parameters().contains(prefix + QLatin1String("Size")))
            icon->setSpecifiedSize(m_place.icon().parameters().value(prefix + QLatin1String("Size")).toSize());

        m_icons.append(icon);
    }

    processIcons();
}

void SavePlaceReply::processIcons()
{
    if (qobject_cast<Icon *>(sender())) {
        Icon *senderIcon = qobject_cast<Icon *>(sender());
        if (senderIcon->error() != QPlaceReply::NoError) {
            triggerDone(senderIcon->error(), senderIcon->errorString());
            return;
        }
    }

    if (currIconIndex < m_icons.count()) {
        Icon *icon = m_icons.at(currIconIndex);
        connect(icon, SIGNAL(initializationFinished()), this, SLOT(processIcons()), Qt::QueuedConnection);
        icon->initialize();
        currIconIndex++;
        return;
    } else {
        QVariantMap thumbnailsMap;
        QVariantMap thumbnailMap;

        bool error = false;

        //try to set destinations for icons which were not already set
        QStringList specifiedDestinations;
        foreach (Icon *icon, m_icons) {
            if (!icon->destination().isEmpty())
                specifiedDestinations.append(icon->destination());
        }

        //try to set small,medium and large destinations if they haven't already been explicitly specified
        //and there are icons with unspecified destinations. (essentially we are creating data urls if necessary)
        if (!specifiedDestinations.contains(Icon::SmallDestination))
            trySetDestination(Icon::SmallDestination);

        if (!specifiedDestinations.contains(Icon::MediumDestination))
            trySetDestination(Icon::MediumDestination);

        if (!specifiedDestinations.contains(Icon::LargeDestination))
            trySetDestination(Icon::LargeDestination);

        //Note that we don't try and set the destination for full screen thumbnails
        //since data urls are meant to be just for small images

        foreach (Icon *icon, m_icons) {
            thumbnailMap.clear();
            if (icon->error() != QPlaceReply::NoError) {
                triggerDone(icon->error(), icon->errorString());
                error = true;
                break;
            }

            if (!icon->sourceUrl().isEmpty()
                    && icon->destinationUrl().scheme().compare(QLatin1String("file")) == 0) {
                if (!icon->copy()) {
                    triggerDone(icon->error(), icon->errorString());
                    error = true;
                    break;
                }
            }

            thumbnailMap.insert(JsonConverter::Url, icon->destinationUrl().toString());
            if (icon->size().isValid()) {
                thumbnailMap.insert(JsonConverter::Height, icon->size().height());
                thumbnailMap.insert(JsonConverter::Width, icon->size().width());
            } else {
                //size of icon could not be calculated, therefore rely on manually specified size
                if (!icon->specifiedSize().isValid()) {
                    triggerDone(QPlaceReply::BadArgumentError, QLatin1String("Size of icon could not be generated nor was it validly specified"));
                    error = true;
                    break;
                }
                thumbnailMap.insert(JsonConverter::Height, icon->specifiedSize().height());
                thumbnailMap.insert(JsonConverter::Width, icon->specifiedSize().width());
            }

            if (icon->destination() == Icon::SmallDestination)
                thumbnailsMap.insert(JsonConverter::Small, thumbnailMap);
            else if (icon->destination() == Icon::MediumDestination)
                thumbnailsMap.insert(JsonConverter::Medium, thumbnailMap);
            else if (icon->destination() == Icon::LargeDestination)
                thumbnailsMap.insert(JsonConverter::Large, thumbnailMap);
            else
                thumbnailsMap.insert(JsonConverter::Fullscreen, thumbnailMap);
        }

        if (!error) {
            m_placeMap.insert(JsonConverter::Thumbnails, thumbnailsMap);

            //proceed to save
            m_state = SavePlaceReply::Saving;
            if (m_place.placeId().isEmpty())
                m_reqId = db()->create(m_placeMap);
            else
                m_reqId = db()->update(m_placeMap);
        }

        qDeleteAll(m_icons);
        m_icons.clear();
    }
}

void SavePlaceReply::processResponse(int id, const QVariant &data)
{
    if (id != m_reqId)
        return;

    if (m_state == SavePlaceReply::CheckIfExists) {
        if (data.toMap().value(JsonConverter::Length).toInt() == 0) {
            triggerDone(QPlaceReply::PlaceDoesNotExistError, QString::fromLatin1("Specified place does not exist"));
            return;
        }

        m_placeMap = data.toMap().value(JsonConverter::Data).toList().first().toMap();

        enterGetCategoriesState();
    } else if (m_state == SavePlaceReply::GetCategories) {
        if (data.toMap().value(JsonConverter::Length).toInt() < m_place.categories().count()) {
            triggerDone(QPlaceReply::CategoryDoesNotExistError,
                            QLatin1String("Specified categories does not exist for place"));
            return;
        } else if (data.toMap().value(JsonConverter::Length).toInt() > m_place.categories().count()) {
            triggerDone(QPlaceReply::UnknownError,
                        QLatin1String("Too many categories found for a place")); //should never happen
            return;
        }

        QList<QVariantMap> categoriesJson = JsonConverter::convertToListOfMaps(data);
        QStringList allCategoryUuids;
        foreach (const QVariantMap categoryMap, categoriesJson)
            allCategoryUuids.append(categoryMap.value(JsonConverter::Ancestors).toStringList());
        allCategoryUuids.removeDuplicates();

        m_placeMap.insert(JsonConverter::AllCategoryUuids, allCategoryUuids);
        m_placeMap = JsonConverter::addToJsonMap(m_placeMap, m_place);

        enterGetIconsState();
        return;
    } else if (m_state == SavePlaceReply::Saving) {
        /*
        Expected data format
        {
        "uuid":<uuid>,
        "_version": <version>
        }*/
        setId(data.toMap().value(JsonConverter::Uuid).toString());
        triggerDone();
    }
}

void SavePlaceReply::processError(int id, int code, const QString &jsonDbErrorString)
{
    if (id != m_reqId)
        return;

    if (m_state == SavePlaceReply::CheckIfExists) {
        triggerDone(QPlaceReply::UnknownError, QString::fromLatin1("Error saving place from jsondb, could not "
                                                              "determine if place exists"));
        return;
    }

    if (code == JsonDbError::MissingObject) {
        triggerDone(QPlaceReply::PlaceDoesNotExistError, QLatin1String("Place does not exist"));
        return;
    } else {
        QPlaceReply::Error error = QPlaceReply::UnknownError;
        QString errorString = QString::fromLatin1("Unknown error occurred during save place operation: jsondb error code =%1, erroString=%2")
                      .arg(code).arg(jsonDbErrorString);
        triggerDone(error, errorString);
        return;
    }
}

void SavePlaceReply::trySetDestination(const QString &destination)
{
    static int threshold;
    int height;

    //assumption is that icons are squarish
    //so we can rely on height as a means to detect which size bucket
    //the icon belongs to
    if (destination == Icon::SmallDestination) {
        threshold = (Icon::SmallSize.height() + Icon::MediumSize.height()) / 2;
        height = Icon::SmallSize.height();
    } else if (destination == Icon::MediumDestination) {
        threshold = (Icon::MediumSize.height() + Icon::LargeSize.height()) / 2;
        height = Icon::MediumSize.height();
    } else if (destination == Icon::LargeDestination) {
        threshold = Icon::LargeSize.height() * 2;
        height = Icon::LargeSize.height();
    } //note fullscreen thumbnails should not be set as data urls.

    Icon *currIcon = 0;
    foreach (Icon *icon, m_icons) {
        if (icon->destination().isEmpty()
                && icon->size().height() < threshold
                && (currIcon == 0 ||  qAbs(icon->size().height() - height) < qAbs(currIcon->size().height() - height))) {
            if (currIcon)
                currIcon->setDestination(QString());
            currIcon = icon;
            currIcon->setDestination(destination);
        }
    }
    if (currIcon)
        currIcon->setDestinationDataUrl();
}

QUrl SavePlaceReply::convertToUrl(const QVariant &var, bool *ok)
{
    if (ok)
        *ok = false;

    switch (var.type()) {
    case (QVariant::Url): {
        if (ok)
            *ok = true;
        QUrl url = var.toUrl();
        if (url.scheme().isEmpty())
            return QUrl::fromUserInput(url.toString());
        else
            return url;
        break;
    }
    default:
        break;
    }
    return QUrl();
}

//-------RemovePlaceReply

RemovePlaceReply::RemovePlaceReply(QPlaceManagerEngineJsonDb *engine)
    : IdReply(QPlaceIdReply::RemovePlace, engine)
{
}

RemovePlaceReply::~RemovePlaceReply()
{
}

void RemovePlaceReply::setId(const QString &placeId)
{
    IdReply::setId(placeId);
}

void RemovePlaceReply::start()
{
    connect(db(), SIGNAL(response(int,QVariant)), this, SLOT(processResponse(int,QVariant)));
    connect(db(), SIGNAL(error(int,int,QString)), this, SLOT(processError(int,int,QString)));

    QVariantMap jsonMap;
    jsonMap.insert(JsonConverter::Uuid, id());
    jsonMap.insert(JsonConverter::Type, JsonConverter::PlaceType);
    m_reqId = db()->remove(jsonMap);
}

void RemovePlaceReply::processResponse(int id, const QVariant &data)
{
    if (id != m_reqId)
        return;

    /*
        Expected data format example
        {
        "count":1,
        "data":[
            {
            "_uuid":"8c196304-509c-5c45-0a07-0ea25a280f10523d"
            }],
        "error":[]
        }
    */
    QVariantMap jsonResponse = data.toMap();
    if (jsonResponse.value(QLatin1String("count")).toInt() <= 0) {
        triggerDone(QPlaceReply::UnknownError,
                             QString::fromLatin1("JsonDb response does not contain a uuid"
                                "for a removal request"));
        return;
    } else {
        QVariantMap jsonResponse = data.toMap();
        QString uuid = jsonResponse.value(JsonConverter::Data)
                .toList().at(0).toMap().value(JsonConverter::Uuid).toString();
        if (uuid != IdReply::id()) {
            triggerDone(QPlaceReply::UnknownError,
                                 QString::fromLatin1("JsonDb Response UUID does not match that in request"
                                    "for a removal operation\n"
                                    "JsonDb UUID: %1"
                                    "Request UUID: %2").arg(uuid).arg(IdReply::id()));
            return;
        } else {
            triggerDone();
            return;
        }
    }
}

void RemovePlaceReply::processError(int id, int code, const QString &jsonDbErrorString)
{
    if (id != m_reqId)
        return;

    if (code == JsonDbError::MissingObject) {
        triggerDone(QPlaceReply::PlaceDoesNotExistError, QLatin1String("Place does not exist"));
        return;
    } else {
        QPlaceReply::Error error = QPlaceReply::UnknownError;
        QString errorString = QString::fromLatin1("Unknown error occurred during remove place operation: jsondb error code =%1, erroString=%2").
                      arg(code).arg(jsonDbErrorString);
        triggerDone(error, errorString);
        return;
    }
}

//-------SaveCategoryReply

SaveCategoryReply::SaveCategoryReply(QPlaceManagerEngineJsonDb *engine)
    : IdReply(QPlaceIdReply::SaveCategory, engine)
{
}

SaveCategoryReply::~SaveCategoryReply()
{
}

void SaveCategoryReply::setCategory(const QPlaceCategory &category)
{
    m_category = category;
}

void SaveCategoryReply::setParentId(const QString &parentId)
{
    m_parentId = parentId;
}

void SaveCategoryReply::start()
{
   connect(db(), SIGNAL(response(int,QVariant)), this, SLOT(processResponse(int,QVariant)));
   connect(db(), SIGNAL(error(int,int,QString)), this, SLOT(processError(int,int,QString)));


    //check if parentExists
   if (!m_parentId.isEmpty()) {
       m_state = SaveCategoryReply::CheckParentExists;
       m_reqId = db()->query(m_engine->queryCategoryString(m_parentId));
   } else {
       doSave();
   }
}

void SaveCategoryReply::doSave()
{
    m_state = Saving;

    if (m_category.categoryId().isEmpty())
        m_category.setCategoryId(QUuid::createUuid().toString());
    QVariantMap categoryMap = JsonConverter::convertToJsonMap(m_category);

    m_newAncestors.append(m_category.categoryId());
    categoryMap.insert(JsonConverter::Ancestors, m_newAncestors);
    categoryMap.insert(JsonConverter::CategoryParentId,  m_parentId);
    m_reqId = db()->update(categoryMap);
}

void SaveCategoryReply::processResponse(int id, const QVariant &data)
{
    if (id != m_reqId)
        return;

    if (m_state == SaveCategoryReply::CheckParentExists) {
        if (data.toMap().value(JsonConverter::Length).toInt() <= 0) {
            triggerDone(QPlaceReply::CategoryDoesNotExistError, "Parent category does not exist");
            return;
        }

        QVariantMap parentMap = data.toMap().value(JsonConverter::Data).toList().at(0).toMap();
        m_newAncestors = parentMap.value(JsonConverter::Ancestors).toStringList();

        if (!m_category.categoryId().isEmpty()) {
            m_state = SaveCategoryReply::GetCurrentCategory;
            m_reqId = db()->query(m_engine->queryCategoryString(m_category.categoryId()));
            return;
        }

        doSave();
    } else if (m_state == SaveCategoryReply::GetCurrentCategory) {
        if (data.toMap().value(JsonConverter::Length).toInt() == 0) {
            triggerDone(QPlaceReply::CategoryDoesNotExistError, "Category does not exist");
            return;
        }

        QVariantMap categoryMap = data.toMap().value(JsonConverter::Data).toList().first().toMap();
        m_oldAncestors = categoryMap.value(JsonConverter::Ancestors).toStringList();
        m_oldAncestors.removeLast();  //remove the current category from the ancestor list

        //if the new parent is different from the old, if so we need to change the ancestors
        //of the category and it's descendents
        if (m_parentId != categoryMap.value(JsonConverter::CategoryParentId)) {

            QString getDescendantsQuery = QString("[?%1=\"%2\"]").arg(JsonConverter::Type).arg(JsonConverter::CategoryType);
            foreach (const QVariant &ancestor, m_oldAncestors)
                getDescendantsQuery += QString("[?%1 contains \"%2\"]").arg(JsonConverter::Ancestors).arg(ancestor.toString());

            m_reqId = db()->query(getDescendantsQuery);
            m_state = SaveCategoryReply::GetParentDescendants;
        } else {
            m_state = SaveCategoryReply::Saving;
            m_reqId = db()->update(JsonConverter::addToJsonMap(categoryMap, m_category));
        }

    } else if (m_state == SaveCategoryReply::GetParentDescendants) {
        if (data.toMap().value(JsonConverter::Data).toList().length() == 0) {
            triggerDone(QPlaceReply::CategoryDoesNotExistError, QLatin1String("Category could not be found while searching for the parent's "
                                                                "descendants"));
            return;
        }

        QStringList ancestors;
        QStringList newAncestors;
        QVariantList jsonMapCategories;
        QVariantMap categoryMap;
        foreach (QVariant var, data.toMap().value(JsonConverter::Data).toList()) {
            categoryMap = var.toMap();
            ancestors = categoryMap.value(JsonConverter::Ancestors).toStringList();

            for (int i = 0; i < m_oldAncestors.count(); ++i)
               ancestors.removeFirst();

            newAncestors = m_newAncestors;
            newAncestors.append(ancestors);

            categoryMap.insert(JsonConverter::Ancestors, newAncestors);

            if (categoryMap.value(JsonConverter::Uuid).toString() == m_category.categoryId()) {
                categoryMap = JsonConverter::addToJsonMap(categoryMap, m_category);
                categoryMap.insert(JsonConverter::CategoryParentId, m_parentId);
            }

            jsonMapCategories.append(categoryMap);
        }

        m_state = SaveCategoryReply::Saving;
        m_reqId = db()->update(jsonMapCategories);

    } else if (m_state == SaveCategoryReply::Saving) {
        if (data.type() == QVariant::Map) {
            setId(data.toMap().value(JsonConverter::Uuid).toString());
            triggerDone();
        } else if (data.type() == QVariant::List) {
            setId(m_category.categoryId());
            triggerDone();
        }
    }
}

void SaveCategoryReply::processError(int id, int code, const QString &jsonDbErrorString)
{
    if (id != m_reqId)
        return;

    if (code == JsonDbError::MissingObject) {
        triggerDone(QPlaceReply::CategoryDoesNotExistError, QLatin1String("Category does not exist"));
        return;
    } else {
        QPlaceReply::Error error = QPlaceReply::UnknownError;
        QString errorString = QString::fromLatin1("Unknown error occurred during save category operation: "
                                                  "jsondb error code = %1, erroString = %2")
                      .arg(code).arg(jsonDbErrorString);
        triggerDone(error, errorString);
        return;
    }
}

//-------RemoveCategoryReply

RemoveCategoryReply::RemoveCategoryReply(QPlaceManagerEngineJsonDb *engine)
    : IdReply(QPlaceIdReply::RemovePlace, engine)
{
}

RemoveCategoryReply::~RemoveCategoryReply()
{
}

void RemoveCategoryReply::setId(const QString &categoryId)
{
    IdReply::setId(categoryId);
}

void RemoveCategoryReply::start()
{
    connect(db(), SIGNAL(response(int,QVariant)), this, SLOT(processResponse(int,QVariant)));
    connect(db(), SIGNAL(error(int,int,QString)), this, SLOT(processError(int,int,QString)));

    m_state = RemoveCategoryReply::GetCategory;
    m_reqId = db()->query(m_engine->queryCategoryString(id()));
}

void RemoveCategoryReply::processResponse(int id, const QVariant &data)
{
    if (id != m_reqId)
        return;

    //TODO: need to handle removal of category related information still
    //      in the Place objects
    switch (m_state) {
    case RemoveCategoryReply::Initial :
        break;
    case (RemoveCategoryReply::GetCategory) : {
        /*
            Expected data format example
            {
            "count":1,
            "data":[
                {
                "_uuid":"8c196304-509c-5c45-0a07-0ea25a280f10523d"
                }],
            "error":[]
            }
        */
        if (data.toMap().value(JsonConverter::Length).toInt() <= 0) {
            triggerDone(QPlaceReply::CategoryDoesNotExistError,
                        QLatin1String("Trying to remove category that does not exist"));
            return;
        } else {
             QVariantMap categoryMap = data.toMap().value(JsonConverter::Data)
                                        .toList().at(0).toMap();
            QString uuid = categoryMap.value(JsonConverter::Uuid).toString();
            if (uuid != IdReply::id()) {
                triggerDone(QPlaceReply::UnknownError,
                            QString::fromLatin1(" Response UUID does not match that in the request"
                               "for a category removal"
                               "JsonDb UUID: %1"
                               "Request UUID: %2").arg(uuid).arg(IdReply::id()));
                return;
            } else {
                QString queryString = QString::fromLatin1("[?%1=\"%2\"]").arg(JsonConverter::Type).arg(JsonConverter::CategoryType);
                QStringList ancestors = categoryMap.value(JsonConverter::Ancestors).toStringList();
                foreach (const QString &ancestor, ancestors)
                    queryString += QString::fromLatin1("[?%1 contains \"%2\"]").arg(JsonConverter::Ancestors).arg(ancestor);

                m_state = RemoveCategoryReply::GetCategoriesToBeRemoved;
                m_reqId = db()->query(queryString);
            }
        }
        break;
    }
    case (RemoveCategoryReply::GetCategoriesToBeRemoved) : {
        QVariant categoriesJson = data.toMap().value("data");
        m_state = RemoveCategoryReply::RemoveCategories;
        m_reqId = db()->remove(categoriesJson);
        break;
    }
    case (RemoveCategoryReply::RemoveCategories) : {
        triggerDone();
    }
    }
}

void RemoveCategoryReply::processError(int id, int code, const QString &jsonDbErrorString)
{
    if (id != m_reqId)
        return;

    if (code == JsonDbError::MissingObject) {
        triggerDone(QPlaceReply::CategoryDoesNotExistError, QLatin1String("Category does not exist"));
        return;
    } else {
        QPlaceReply::Error error = QPlaceReply::UnknownError;
        QString errorString = QString::fromLatin1("Unknown error occurred during remove place operation: jsondb error code =%1, erroString=%2").
                      arg(code).arg(jsonDbErrorString);
        triggerDone(error, errorString);
        return;
    }
}


