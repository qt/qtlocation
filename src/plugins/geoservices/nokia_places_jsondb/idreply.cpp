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
#include "jsondb.h"

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
    if (m_place.visibility() & ~QtLocation::DeviceVisibility) {
        triggerDone(QPlaceReply::UnsupportedError, QString::fromLatin1("Only saving to device (or unspecified) scope is supported"));
        return;
    } else {
        if (m_place.placeId().isEmpty())
            db()->getCategories(m_place.categories(), this, SLOT(getCategoriesForPlaceFinished()));
        else
            db()->getPlace(m_place.placeId(), this, SLOT(checkIfExistsFinished()));
    }
}

void SavePlaceReply::getIcons()
{
    QStringList prefixes;
    prefixes << QStringLiteral("small") << QStringLiteral("medium") << QStringLiteral("large") << QStringLiteral("fullscreen");

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

        //if we have an existing place, we try to preserve existing properties
        //of the thumbnail objects, but we completely remove/replace the fields
        //that are relevant for the QtLocation API.
        QJsonObject thumbnailsJson = m_placeJson.value(JsonDb::Thumbnails).toObject();
        thumbnailsJson.remove(JsonDb::Small);
        thumbnailsJson.remove(JsonDb::Medium);
        thumbnailsJson.remove(JsonDb::Large);
        thumbnailsJson.remove(JsonDb::Fullscreen);

        foreach (Icon *icon, m_icons) {
            QJsonObject thumbnailJson;
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

            thumbnailJson.insert(JsonDb::Url, icon->destinationUrl().toString());
            if (icon->size().isValid()) {
                thumbnailJson.insert(JsonDb::Height, icon->size().height());
                thumbnailJson.insert(JsonDb::Width, icon->size().width());
            } else {
                //size of icon could not be calculated, therefore rely on manually specified size
                if (!icon->specifiedSize().isValid()) {
                    triggerDone(QPlaceReply::BadArgumentError, QLatin1String("Size of icon could not be generated nor was it validly specified"));
                    error = true;
                    break;
                }
                thumbnailJson.insert(JsonDb::Height, icon->specifiedSize().height());
                thumbnailJson.insert(JsonDb::Width, icon->specifiedSize().width());
            }

            if (icon->destination() == Icon::SmallDestination)
                thumbnailsJson.insert(JsonDb::Small, thumbnailJson);
            else if (icon->destination() == Icon::MediumDestination)
                thumbnailsJson.insert(JsonDb::Medium, thumbnailJson);
            else if (icon->destination() == Icon::LargeDestination)
                thumbnailsJson.insert(JsonDb::Large, thumbnailJson);
            else
                thumbnailsJson.insert(JsonDb::Fullscreen, thumbnailJson);
        }

        if (!error) {
            m_placeJson.insert(JsonDb::Thumbnails, thumbnailsJson);

            //proceed to save
            db()->write(m_placeJson, this, SLOT(savingFinished()));
        }

        qDeleteAll(m_icons);
        m_icons.clear();
    }
}

void SavePlaceReply::checkIfExistsFinished()
{
    QJsonDbRequest *request = qobject_cast<QJsonDbRequest *>(sender());
    Q_ASSERT(request);
    QList<QJsonObject> results = request->takeResults();

    if (results.isEmpty()) {
        triggerDone(QPlaceReply::PlaceDoesNotExistError, QStringLiteral("Specified place does not exist"));
        return;
    }

    m_placeJson = results.takeFirst();
    db()->getCategories(m_place.categories(), this, SLOT(getCategoriesForPlaceFinished()));
}

void SavePlaceReply::getCategoriesForPlaceFinished()
{
    QJsonDbRequest *request = qobject_cast<QJsonDbRequest *>(sender());
    Q_ASSERT(request);
    QList<QJsonObject> results = request->takeResults();

    if (results.count() < m_place.categories().count()) {
        triggerDone(QPlaceReply::CategoryDoesNotExistError,
                        QStringLiteral("Specified categories does not exist for place"));
        return;
    } else if (results.count() > m_place.categories().count()) {
        triggerDone(QPlaceReply::UnknownError,
                    QStringLiteral("Too many categories found for a place")); //should never happen
        return;
    }

    QStringList allCategoryUuids;
    foreach (const QJsonObject &categoryJson, results)
        allCategoryUuids.append(categoryJson.value(JsonDb::Lineage).toVariant().toStringList());
    allCategoryUuids.removeDuplicates();

    m_placeJson.insert(JsonDb::AllCategoryUuids, QJsonArray::fromStringList(allCategoryUuids));
    JsonDb::addToJson(&m_placeJson, m_place);

    getIcons();
    return;
}

void SavePlaceReply::savingFinished()
{
    QJsonDbRequest *request = qobject_cast<QJsonDbRequest *>(sender());
    Q_ASSERT(request);
    QList<QJsonObject> results = request->takeResults();

    if (results.count() != 1) {
        triggerDone(QPlaceReply::UnknownError, QStringLiteral("Saving a place resulted in ") + results.count() + QStringLiteral("places being saved"));
    } else {
        setId(results.first().value(JsonDb::Uuid).toString());
        triggerDone();
    }
}

void SavePlaceReply::requestError(QtJsonDb::QJsonDbRequest::ErrorCode dbCode, const QString &dbErrorString)
{
    if (dbCode == QJsonDbRequest::MissingObject) {
        triggerDone(QPlaceReply::PlaceDoesNotExistError, QStringLiteral("Place does not exist"));
        return;
    } else {
        QString errorString = QString::fromLatin1("Unknown error occurred during save place operation: jsondb error code = %1, erroString = %2")
                      .arg(dbCode).arg(dbErrorString);
        triggerDone(QPlaceReply::UnknownError, errorString);
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
    db()->getPlace(id(), this, SLOT(checkIfExistsFinished()));
}

void RemovePlaceReply::checkIfExistsFinished()
{
    QJsonDbRequest *request = qobject_cast<QJsonDbRequest *>(sender());
    Q_ASSERT(request);
    QList<QJsonObject> results = request->takeResults();

    if (results.isEmpty()) {
        triggerDone(QPlaceReply::PlaceDoesNotExistError, QStringLiteral("Specified place does not exist"));
        return;
    }

    db()->remove(results.first(), this, SLOT(removeFinished()));
}

void RemovePlaceReply::removeFinished()
{
    triggerDone();
}

void RemovePlaceReply::requestError(QJsonDbRequest::ErrorCode dbCode, const QString &dbErrorString)
{
    if (dbCode == QJsonDbRequest::MissingObject) {
        triggerDone(QPlaceReply::PlaceDoesNotExistError, QStringLiteral("Place does not exist"));
        return;
    } else {
        QPlaceReply::Error error = QPlaceReply::UnknownError;
        QString errorString = QString::fromLatin1("Unknown error occurred during remove place operation: jsondb error code =%1, erroString=%2").
                      arg(dbCode).arg(dbErrorString);
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
    //check if parentExists
   if (!m_parentId.isEmpty())
       db()->getCategory(m_parentId, this, SLOT(checkParentExistsFinished()));
   else
       db()->write(prepareCategoryJson(), this, SLOT(savingFinished()));
}

void SaveCategoryReply::checkParentExistsFinished()
{
    QJsonDbRequest *request = qobject_cast<QJsonDbRequest *>(sender());
    Q_ASSERT(request);
    QList<QJsonObject> results = request->takeResults();

    if (results.isEmpty()) {
        triggerDone(QPlaceReply::CategoryDoesNotExistError, QStringLiteral("Parent category does not exist"));
        return;
    } else if (results.count() > 1) {
        triggerDone(QPlaceReply::UnknownError,
                    QStringLiteral("More than one category was found corresponding to (parent) id") + m_parentId);
        return;
    } else {
        QJsonObject parentJson = results.first();
        m_newAncestors = parentJson.value(JsonDb::Lineage).toArray();
        if (!m_category.categoryId().isEmpty()) {
            db()->getCategory(m_category.categoryId(), this, SLOT(getCurrentCategoryFinished()));
            return;
        }
        db()->write(prepareCategoryJson(), this, SLOT(savingFinished()));
        return;
    }
}

void SaveCategoryReply::getCurrentCategoryFinished()
{
    QJsonDbRequest *request = qobject_cast<QJsonDbRequest *>(sender());
    Q_ASSERT(request);
    QList<QJsonObject> results = request->takeResults();

    if (results.isEmpty()) {
        triggerDone(QPlaceReply::CategoryDoesNotExistError, "Category does not exist");
        return;
    } else {
        QJsonObject categoryJson = results.first();
        m_oldAncestors = categoryJson.value(JsonDb::Lineage).toArray();
        m_oldAncestors.removeLast();

        //if the new parent is different from the old, if so we need to change the ancestors
        //of it's descendants and the ancestors of itself
        if (m_parentId != categoryJson.value(JsonDb::CategoryParentId).toString()) {
            QString getDescendantsAndSelf = QString::fromLatin1("[?_type=\"%1\"]").arg(JsonDb::CategoryType);
            for (int i = 0; i < m_oldAncestors.count(); ++i)
                getDescendantsAndSelf += QString::fromLatin1("[?%1 contains \"%2\"]").arg(JsonDb::Lineage).arg(m_oldAncestors.at(i).toString());
            getDescendantsAndSelf += QString::fromLatin1("[?%1 contains \"%2\"]").arg(JsonDb::Lineage).arg(m_category.categoryId());

            db()->read(getDescendantsAndSelf, this, SLOT(getDescendantsAndSelfFinished()));
        } else {
            JsonDb::addToJson(&categoryJson, m_category);

            db()->write(categoryJson, this, SLOT(savingFinished()));
        }
    }
}

void SaveCategoryReply::getDescendantsAndSelfFinished()
{
    QJsonDbRequest *request = qobject_cast<QJsonDbRequest *>(sender());
    Q_ASSERT(request);
    QList<QJsonObject> results = request->takeResults();

    if (results.isEmpty()) {
        triggerDone(QPlaceReply::CategoryDoesNotExistError, QStringLiteral("Category and it's descendants couldn ot be found"));
        return;
    }

    QList<QJsonObject> categoriesJson;
    for (int i = 0; i < results.count(); ++i) {
        QJsonObject categoryJson = results.at(i);
        QJsonArray lineage = categoryJson.value(JsonDb::Lineage).toArray();
        for (int i = 0; i < m_oldAncestors.count(); ++i)
           lineage.removeFirst();

        for (int i = m_newAncestors.count() - 1; i >= 0; --i)
            lineage.prepend(m_newAncestors.at(i));

        categoryJson.insert(JsonDb::Lineage, lineage);

        if (categoryJson.value(JsonDb::Uuid).toString() == m_category.categoryId()) {
            JsonDb::addToJson(&categoryJson, m_category);
            categoryJson.insert(JsonDb::CategoryParentId, m_parentId);
        }

        categoriesJson.append(categoryJson);
    }

    db()->write(categoriesJson, this, SLOT(savingFinished()));
}

void SaveCategoryReply::savingFinished()
{
    QJsonDbRequest *request = qobject_cast<QJsonDbRequest *>(sender());
    Q_ASSERT(request);
    QList<QJsonObject> results = request->takeResults();

    if (!m_category.categoryId().isEmpty()) //must've been saving an existing category
        setId(m_category.categoryId());
    else
        setId(results.first().value(JsonDb::Uuid).toString()); //must've been saving a new category

    triggerDone();
}

QJsonObject SaveCategoryReply::prepareCategoryJson()
{
    //if it is a new category we need to predefine the category id so we can
    //save it in the lineage
    if (m_category.categoryId().isEmpty())
        m_category.setCategoryId(QUuid::createUuid().toString());

    QJsonObject categoryJson;
    JsonDb::addToJson(&categoryJson, m_category);

    QJsonArray lineage = m_newAncestors;
    lineage.append(m_category.categoryId());
    categoryJson.insert(JsonDb::Lineage, lineage);
    categoryJson.insert(JsonDb::CategoryParentId,  m_parentId);

    return categoryJson;
}

void SaveCategoryReply::requestError(QJsonDbRequest::ErrorCode dbCode, const QString &dbErrorString)
{
    if (dbCode == QJsonDbRequest::MissingObject) {
        triggerDone(QPlaceReply::CategoryDoesNotExistError, QStringLiteral("Category does not exist"));
        return;
    } else {
        QString errorString = QString::fromLatin1("Unknown error occurred during save category operation: "
                                                  "jsondb error code = %1, erroString = %2")
                      .arg(dbCode).arg(dbErrorString);
        triggerDone(QPlaceReply::UnknownError, errorString);
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
    db()->getCategory(id(), this, SLOT(getCategoryFinished()));
}

void RemoveCategoryReply::getCategoryFinished()
{
    QJsonDbRequest *request = qobject_cast<QJsonDbRequest *>(sender());
    Q_ASSERT(request);
    QList<QJsonObject> results = request->takeResults();

    if (results.isEmpty()) {
        triggerDone(QPlaceReply::CategoryDoesNotExistError,
                    QStringLiteral("Trying to remove category that does not exist"));
        return;
    } else {
        QJsonObject categoryJson = results.first();
        QString uuid = categoryJson.value(JsonDb::Uuid).toString();
        if (uuid != id()) {
            triggerDone(QPlaceReply::UnknownError,
                        QString::fromLatin1(" Response UUID does not match that in the request"
                           "for a category removal"
                           "JsonDb UUID: %1"
                           "Request UUID: %2").arg(uuid).arg(id()));
            return;
        } else {
            //now check if there are any child categories which need to be removed as well
            QString queryString = QString::fromLatin1("[?%1=\"%2\"]").arg(JsonDb::Type).arg(JsonDb::CategoryType);
            QStringList lineage = categoryJson.value(JsonDb::Lineage).toVariant().toStringList();
            foreach (const QString &ancestor, lineage)
                queryString += QString::fromLatin1("[?%1 contains \"%2\"]").arg(JsonDb::Lineage).arg(ancestor);

            db()->read(queryString, this, SLOT(getCategoriesToBeRemovedFinished()));
        }
    }
}

void RemoveCategoryReply::getCategoriesToBeRemovedFinished()
{
    QJsonDbRequest *request = qobject_cast<QJsonDbRequest *>(sender());
    Q_ASSERT(request);
    QList<QJsonObject> results = request->takeResults();

    if (!results.isEmpty())
        db()->remove(results, this, SLOT(removeFinished()));
    else
        removeFinished();
}

void RemoveCategoryReply::removeFinished()
{
    triggerDone();
}

void RemoveCategoryReply::requestError(QJsonDbRequest::ErrorCode dbCode, const QString &dbErrorString)
{
    if (dbCode == QJsonDbRequest::MissingObject) {
        triggerDone(QPlaceReply::CategoryDoesNotExistError, QStringLiteral("Category does not exist"));
        return;
    } else {
        QString errorString = QString::fromLatin1("Unknown error occurred during remove place operation: jsondb error code =%1, erroString=%2").
                      arg(dbCode).arg(dbErrorString);
        triggerDone(QPlaceReply::UnknownError, errorString);
        return;
    }
}
