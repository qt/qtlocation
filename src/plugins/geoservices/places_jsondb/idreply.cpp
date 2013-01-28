/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "idreply.h"
#include "qplacemanagerengine_jsondb.h"
#include "jsondb.h"
#include "traverser.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QUuid>
#include <QtCore/QDateTime>

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
    : IdReply(QPlaceIdReply::SavePlace, engine), m_iconHandler(0)
{
}

SavePlaceReply::~SavePlaceReply()
{
    delete m_iconHandler;
}

void SavePlaceReply::setPlace(const QPlace &place)
{
    m_place = place;
    setId(m_place.placeId());
}

void SavePlaceReply::start()
{
    if (m_place.visibility() & ~QLocation::DeviceVisibility) {
        triggerDone(QPlaceReply::UnsupportedError, QString::fromLatin1("Only saving to device (or unspecified) scope is supported"));
        return;
    } else {
        if (m_place.placeId().isEmpty())
            db()->getCategories(m_place.categories(), this, SLOT(getCategoriesForPlaceFinished()));
        else
            db()->getPlace(m_place.placeId(), this, SLOT(checkIfExistsFinished()));
    }
}

void SavePlaceReply::processIconsFinished(const QJsonObject &thumbnailsJson)
{
    if (m_iconHandler->error() != QPlaceReply::NoError) {
        triggerDone(m_iconHandler->error(), m_iconHandler->errorString());
        return;
    }

    if (!thumbnailsJson.isEmpty())
        m_placeJson.insert(JsonDb::Thumbnails, thumbnailsJson);

    delete m_iconHandler;
    m_iconHandler = 0;

    QString currentDateTime = QDateTime::currentDateTime().toString(Qt::ISODate);
    if (m_place.placeId().isEmpty()) {
        m_placeJson.insert(JsonDb::CreatedDateTime, currentDateTime);
        m_placeJson.insert(JsonDb::ModifiedDateTime, currentDateTime);
    } else {
        m_placeJson.insert(JsonDb::ModifiedDateTime, currentDateTime);
    }

    db()->write(m_placeJson, this, SLOT(savingFinished()));
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

    JsonDb::addToJson(&m_placeJson, m_place);

    m_iconHandler = new IconHandler(m_place.icon(), m_placeJson.value(JsonDb::Thumbnails).toObject(),
                                    const_cast<SavePlaceReply *>(this));
    connect(m_iconHandler, SIGNAL(finished(QJsonObject)), this, SLOT(processIconsFinished(QJsonObject)));
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
    : IdReply(QPlaceIdReply::SaveCategory, engine), m_iconHandler(0)
{
}

SaveCategoryReply::~SaveCategoryReply()
{
    delete m_iconHandler;
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
   else {

       if (!m_category.categoryId().isEmpty()) {
           db()->getCategory(m_category.categoryId(), this, SLOT(getCurrentCategoryFinished()));
       } else {
           m_categoryJson = prepareCategoryJson();
           processIcons();
       }
   }
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
        if (!m_category.categoryId().isEmpty()) {
            db()->getCategory(m_category.categoryId(), this, SLOT(getCurrentCategoryFinished()));
            return;
        }

        m_categoryJson = prepareCategoryJson();
        processIcons();
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
        m_categoryJson = results.first();
        JsonDb::addToJson(&m_categoryJson, m_category);
        m_categoryJson.insert(JsonDb::CategoryParentId, m_parentId);
        processIcons();
    }
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

void SaveCategoryReply::processIconsFinished(const QJsonObject &thumbnailsJson)
{
    if (m_iconHandler->error() != QPlaceReply::NoError) {
        triggerDone(m_iconHandler->error(), m_iconHandler->errorString());
        return;
    }

    if (!thumbnailsJson.isEmpty())
        m_categoryJson.insert(JsonDb::Thumbnails, thumbnailsJson);

    delete m_iconHandler;
    m_iconHandler = 0;

    QList<QJsonObject> categories;
    categories << m_categoryJson << m_descendantsJson;
    db()->write(categories, this, SLOT(savingFinished()));
}

QJsonObject SaveCategoryReply::prepareCategoryJson()
{
    //if it is a new category we need to predefine the category id so we can
    //save it in the lineage
    if (m_category.categoryId().isEmpty())
        m_category.setCategoryId(QUuid::createUuid().toString());

    QJsonObject categoryJson;
    JsonDb::addToJson(&categoryJson, m_category);

    categoryJson.insert(JsonDb::CategoryParentId,  m_parentId);

    return categoryJson;
}

void SaveCategoryReply::processIcons()
{
    m_iconHandler = new IconHandler(m_category.icon(), m_categoryJson.value(JsonDb::Thumbnails).toObject(),
                                    this);
    connect(m_iconHandler, SIGNAL(finished(QJsonObject)), this, SLOT(processIconsFinished(QJsonObject)));
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
            CategoryTraverser * traverser = new CategoryTraverser(engine()->db(), this);
            connect(traverser, SIGNAL(finished()),
                    this, SLOT(getCategoriesToBeRemovedFinished()));
            traverser->start(id());
        }
    }
}

void RemoveCategoryReply::getCategoriesToBeRemovedFinished()
{
    CategoryTraverser * traverser = qobject_cast<CategoryTraverser *>(sender());

    if (!traverser->errorString().isEmpty()) {
        QString errorString =
                QString::fromLatin1("Unknown error occurred during remove place operation: %1")
                .arg(traverser->errorString());
        traverser->deleteLater();

        triggerDone(QPlaceReply::UnknownError, errorString);
        return;
    }

    QList<QJsonObject> catObjects = traverser->results();
    traverser->deleteLater();

    if (!catObjects.isEmpty())
        db()->remove(catObjects, this, SLOT(removeFinished()));
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
