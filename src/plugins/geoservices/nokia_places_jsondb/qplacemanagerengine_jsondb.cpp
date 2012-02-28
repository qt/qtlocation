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

#include "qplacemanagerengine_jsondb.h"

#include "detailsreply.h"
#include "idreply.h"
#include "initreply.h"
#include "matchreply.h"
#include "searchreply.h"
#include "unsupportedreplies.h"

#include <QtCore/QDebug>
#include <QtCore/qnumeric.h>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>

QT_USE_NAMESPACE

QPlaceManagerEngineJsonDb::QPlaceManagerEngineJsonDb(const QMap<QString, QVariant> &parameters,
                                                     QGeoServiceProvider::Error *error,
                                                     QString *errorString)
    :   QPlaceManagerEngine(parameters), m_jsonDb(new JsonDb())
{
    connect(m_jsonDb, SIGNAL(placeNotifications(QList<QJsonDbNotification>)),
            this, SLOT(processPlaceNotifications(QList<QJsonDbNotification>)));
    connect(m_jsonDb, SIGNAL(categoryNotifications(QList<QJsonDbNotification>)),
            this, SLOT(processCategoryNotifications(QList<QJsonDbNotification>)));

    *error = QGeoServiceProvider::NoError;
    errorString->clear();
}

QPlaceManagerEngineJsonDb::~QPlaceManagerEngineJsonDb()
{
    delete m_jsonDb;
}

QPlaceDetailsReply *QPlaceManagerEngineJsonDb::getPlaceDetails(const QString &placeId)
{
    DetailsReply *detailsReply = new DetailsReply(this);
    detailsReply->setPlaceId(placeId);
    detailsReply->start();
    return detailsReply;
}

QPlaceContentReply *QPlaceManagerEngineJsonDb::getPlaceContent(const QString &placeId, const QPlaceContentRequest &request)
{
    Q_UNUSED(placeId)
    Q_UNUSED(request)

    ContentReply *contentReply = new ContentReply(this);
    contentReply->triggerDone(QPlaceReply::UnsupportedError, QString::fromLatin1("Fetching content is unsupported"));
    return contentReply;
}

QPlaceSearchReply *QPlaceManagerEngineJsonDb::search(const QPlaceSearchRequest &request)
{
    SearchReply *searchReply = new SearchReply(this);
    searchReply->setRequest(request);
    searchReply->start();
    return searchReply;
}

QPlaceSearchReply *QPlaceManagerEngineJsonDb::recommendations(const QString &placeId, const QPlaceSearchRequest &request)
{
    Q_UNUSED(placeId);
    SearchReply *searchReply = new SearchReply(this);
    searchReply->setRequest(request);
    searchReply->triggerDone(QPlaceReply::UnsupportedError, QString::fromLatin1("Recommendations are unsupported"));
    return searchReply;
}

QPlaceSearchSuggestionReply *QPlaceManagerEngineJsonDb::searchSuggestions(const QPlaceSearchRequest &request)
{
    Q_UNUSED(request)

    SearchSuggestionReply *searchSuggestionReply = new SearchSuggestionReply(this);
    searchSuggestionReply->triggerDone(QPlaceReply::UnsupportedError, QString::fromLatin1("Search suggestions are unsupported"));
    return searchSuggestionReply;
}

QPlaceIdReply *QPlaceManagerEngineJsonDb::savePlace(const QPlace &place)
{
    SavePlaceReply *saveReply = new SavePlaceReply(this);
    saveReply->setPlace(place);
    saveReply->start();
    return saveReply;
}

QPlaceIdReply *QPlaceManagerEngineJsonDb::saveCategory(const QPlaceCategory &category, const QString &parentId)
{
    SaveCategoryReply *reply = new SaveCategoryReply(this);
    reply->setCategory(category);
    reply->setParentId(parentId);
    reply->start();
    return reply;
}

QPlaceIdReply *QPlaceManagerEngineJsonDb::removePlace(const QString &placeId)
{
    RemovePlaceReply *removeReply = new RemovePlaceReply(this);
    removeReply->setId(placeId);
    removeReply->start();
    return removeReply;
}

QPlaceIdReply *QPlaceManagerEngineJsonDb::removeCategory(const QString &categoryId)
{
    RemoveCategoryReply *reply = new RemoveCategoryReply(this);
    reply->setId(categoryId);
    reply->start();
    return reply;
}

QPlaceReply *QPlaceManagerEngineJsonDb::initializeCategories()
{
    CategoryInitReply *reply = new CategoryInitReply(this);
    reply->start();
    return reply;
}

QString QPlaceManagerEngineJsonDb::parentCategoryId(const QString &categoryId) const
{
    QMutexLocker locker(&m_treeMutex);
    return m_tree.value(categoryId).parentId;
}

QStringList QPlaceManagerEngineJsonDb::childCategoryIds(const QString &categoryId) const
{
    QMutexLocker locker(&m_treeMutex);
    QStringList childrenIds = m_tree.value(categoryId).childIds;
    return childrenIds;
}

QPlaceCategory QPlaceManagerEngineJsonDb::category(const QString &categoryId) const
{
    QMutexLocker locker(&m_treeMutex);

    if (m_tree.contains(categoryId))
        return m_tree.value(categoryId).category;
    else
        return QPlaceCategory();
}

QList<QPlaceCategory> QPlaceManagerEngineJsonDb::childCategories(const QString &parentId) const
{
    QMutexLocker locker(&m_treeMutex);

    QStringList childrenIds = m_tree.value(parentId).childIds;
    QList<QPlaceCategory> categories;
    foreach (const QString &childId, childrenIds) {
        if (m_tree.contains(childId))
            categories.append(m_tree.value(childId).category);
    }

    return categories;
}

QList<QLocale> QPlaceManagerEngineJsonDb::locales() const
{
    return QList<QLocale>();
}

void QPlaceManagerEngineJsonDb::setLocales(const QList<QLocale> &locales)
{
    Q_UNUSED(locales);
}

QPlace QPlaceManagerEngineJsonDb::compatiblePlace(const QPlace &original) const
{
    QPlace place;
    place.setName(original.name());

    QGeoLocation location = original.location();
    QGeoCoordinate coord = original.location().coordinate();
    coord.setAltitude(qQNaN());
    location.setCoordinate(coord);
    location.setBoundingBox(QGeoBoundingBox());
    place.setLocation(location);

    QList<QPlaceContactDetail> details;
    foreach (const QString &contactType, original.contactTypes())
        place.setContactDetails(contactType, original.contactDetails(contactType));

    place.setVisibility(QtLocation::UnspecifiedVisibility);

    QStringList attributeTypes = original.extendedAttributeTypes();
    foreach (const QString &attributeType, attributeTypes)
        place.setExtendedAttribute(attributeType, original.extendedAttribute(attributeType));

    if (attributeTypes.contains(QLatin1String("x_provider")) && !original.extendedAttribute(QLatin1String("x_provider")).text().isEmpty()) {
        QPlaceAttribute alternativeId;
        alternativeId.setText(original.placeId());
        place.setExtendedAttribute(QString::fromLatin1("x_id_") + original.extendedAttribute(QLatin1String("x_provider")).text(),
                                   alternativeId);
    }

    QVariantMap parameters;
    QPlaceIcon originalIcon = original.icon();
    if (!originalIcon.isEmpty()) {
        if (originalIcon.parameters().contains(QPlaceIcon::SingleUrl)) {
            if (isSupportedScheme(originalIcon.url(Icon::MediumSize).scheme()))
                parameters.insert(Icon::MediumSource, originalIcon.url(Icon::MediumSize));
        } else if (originalIcon.manager()) {
            if (!originalIcon.url(Icon::SmallSize).isEmpty() && isSupportedScheme(originalIcon.url(Icon::SmallSize).scheme()))
                parameters.insert(Icon::SmallSource, originalIcon.url(Icon::SmallSize));
            if (!originalIcon.url(Icon::MediumSize).isEmpty() && isSupportedScheme(originalIcon.url(Icon::MediumSize).scheme()))
                parameters.insert(Icon::MediumSource, originalIcon.url(Icon::MediumSize));
            if (!originalIcon.url(Icon::LargeSize).isEmpty() && isSupportedScheme(originalIcon.url(Icon::LargeSize).scheme()))
                parameters.insert(Icon::LargeSource, originalIcon.url(Icon::LargeSize));
        }
    }

    if (!parameters.isEmpty()) {
        QPlaceIcon icon;
        icon.setParameters(parameters);
        icon.setManager(manager());
        place.setIcon(icon);
    }

    return place;
}

QUrl QPlaceManagerEngineJsonDb::constructIconUrl(const QPlaceIcon &icon, const QSize &size) const
{
    QList<QPair<int, QUrl> > candidates;
    //TODO: possible optimizations
    QMap<QString, QSize> sizeDictionary;
    sizeDictionary.insert(Icon::SmallDestination, Icon::SmallSize);

    sizeDictionary.insert(Icon::MediumDestination, Icon::MediumSize);
    sizeDictionary.insert(Icon::LargeDestination, Icon::LargeSize);
    sizeDictionary.insert(Icon::FullscreenDestination, Icon::FullscreenSize);

    QStringList sizeKeys;
    sizeKeys << "small" <<  "medium" << "large"
             << "fullscreen";

    foreach (const QString &sizeKey, sizeKeys) {
        if (icon.parameters().contains(sizeKey + QLatin1String("Url"))) {
            QSize destSize = icon.parameters().value(sizeKey + QLatin1String("Size")).toSize();
            if (destSize.isEmpty()) {
                candidates.append(QPair<int, QUrl>(sizeDictionary.value(sizeKey + QLatin1String("Url")).height(),
                                                   icon.parameters().value(sizeKey + QLatin1String("Url")).toUrl()));
            } else {
                candidates.append(QPair<int, QUrl>(destSize.height(),
                                                   icon.parameters().value(sizeKey + QLatin1String("Url")).toUrl()));
            }
        }
    }

    if (candidates.isEmpty())
        return QUrl();
    else if (candidates.count() == 1) {
        return candidates.first().second;
    } else {
        //we assume icons are squarish so we can use height to
        //determine which particular icon to return
        int requestedHeight = size.height();

        for (int i = 0; i < candidates.count() - 1; ++i) {
            int thresholdHeight = (candidates.at(i).first + candidates.at(i+1).first) / 2;
            if (requestedHeight < thresholdHeight)
                return candidates.at(i).second;
        }
        return candidates.last().second;
    }
    return QUrl();
}

QPlaceMatchReply * QPlaceManagerEngineJsonDb::matchingPlaces(const QPlaceMatchRequest &request)
{
    MatchReply *reply = new MatchReply(this);
    reply->setRequest(request);
    reply->start();
    return reply;
}

void QPlaceManagerEngineJsonDb::setCategoryTree(const CategoryTree &tree)
{
    QMutexLocker locker(&m_treeMutex);
    m_tree = tree;
}

void QPlaceManagerEngineJsonDb::processPlaceNotifications(const QList<QJsonDbNotification> &notifications)
{
    foreach (const QJsonDbNotification &notification, notifications) {
        if (notification.action() == QJsonDbWatcher::Created)
            emit placeAdded(notification.object().value(JsonDb::Uuid).toString());
        else if (notification.action() == QJsonDbWatcher::Updated)
            emit placeUpdated(notification.object().value(JsonDb::Uuid).toString());
        else if (notification.action() == QJsonDbWatcher::Removed)
            emit placeRemoved(notification.object().value(JsonDb::Uuid).toString());
    }
}

void QPlaceManagerEngineJsonDb::processCategoryNotifications(const QList<QJsonDbNotification> &notifications)
{
    foreach (const QJsonDbNotification &notification, notifications) {
        QPlaceCategory category = JsonDb::convertJsonObjectToCategory(notification.object(), this);
        QString parentId = notification.object().value(JsonDb::CategoryParentId).toString();
        if (notification.action() == QJsonDbWatcher::Created)
            emit categoryAdded(category, parentId);
        else if (notification.action() == QJsonDbWatcher::Updated)
            emit categoryUpdated(category, parentId);
        else if (notification.action() == QJsonDbWatcher::Removed)
            emit categoryRemoved(category.categoryId(), parentId);
    }
}

void QPlaceManagerEngineJsonDb::notificationsError(QJsonDbWatcher::ErrorCode code, const QString &errorString)
{
    qWarning() << Q_FUNC_INFO << " Error code: " << code << " Error String: " << errorString;
}

//this only for schemes for source icon urls.
bool QPlaceManagerEngineJsonDb::isSupportedScheme(const QString &scheme) const
{
    if (scheme.isEmpty() || (scheme.compare(QStringLiteral("data"), Qt::CaseInsensitive) == 0) || (scheme.compare(QStringLiteral("file"), Qt::CaseInsensitive) == 0))
        return true;
    else
        return false;
}
