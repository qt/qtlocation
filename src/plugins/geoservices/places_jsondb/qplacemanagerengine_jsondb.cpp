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

#include "qplacemanagerengine_jsondb.h"

#include "traverser.h"
#include "detailsreply.h"
#include "idreply.h"
#include "initreply.h"
#include "matchreply.h"
#include "searchreply.h"
#include "unsupportedreplies.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/qnumeric.h>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QStandardPaths>
#include <QtNetwork/QNetworkAccessManager>

QT_USE_NAMESPACE

Q_DECLARE_METATYPE(QJsonObject);

const QLatin1String QPlaceManagerEngineJsonDb::PartitionKey("places.partition");
const QLatin1String QPlaceManagerEngineJsonDb::LocalDataPathKey("places.local_data_path");

const QLatin1String QPlaceManagerEngineJsonDb::IconThemeKey("places.icons.theme");
const QLatin1String QPlaceManagerEngineJsonDb::CustomIconsKey("places.icons.custom");
const QLatin1String QPlaceManagerEngineJsonDb::FavoriteBadgesKey("places.icons.favorite_badges");


QPlaceManagerEngineJsonDb::QPlaceManagerEngineJsonDb(const QMap<QString, QVariant> &parameters,
                                                     QGeoServiceProvider::Error *error,
                                                     QString *errorString)
    :   QPlaceManagerEngine(parameters), m_netManager(0)
{
    qRegisterMetaType<QJsonObject>();

    QString partition;
    if (parameters.contains(PartitionKey))
        partition = parameters.value(PartitionKey).toString();

    m_jsonDb = new JsonDb(partition);

    connect(m_jsonDb, SIGNAL(placeNotifications(QList<QJsonDbNotification>)),
            this, SLOT(processPlaceNotifications(QList<QJsonDbNotification>)));
    connect(m_jsonDb, SIGNAL(categoryNotifications(QList<QJsonDbNotification>)),
            this, SLOT(processCategoryNotifications(QList<QJsonDbNotification>)));

    m_localDataPath = parameters.value(LocalDataPathKey, QString()).toString();
    if (m_localDataPath.isEmpty()) {
        QStringList dataLocations = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);

        if (!dataLocations.isEmpty() && !dataLocations.first().isEmpty()) {
            m_localDataPath = dataLocations.first()
                                + QLatin1String("/nokia/qtlocation/data");
        }
    }

    m_theme = parameters.value(IconThemeKey, QString()).toString();
    if (m_theme == QLatin1String("default"))
        m_theme.clear();

    m_useFavoriteBadges = parameters.value(FavoriteBadgesKey).toBool();

    if (parameters.contains(CustomIconsKey))
        m_useCustomIcons = parameters.value(CustomIconsKey).toBool();
    else
        m_useCustomIcons = true;

    *error = QGeoServiceProvider::NoError;
    errorString->clear();
}

QPlaceManagerEngineJsonDb::~QPlaceManagerEngineJsonDb()
{
    delete m_netManager;
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
    QString mappingFile = m_localDataPath + QLatin1String("/offline/offline-mapping.json");

    m_jsonDb->parseIconMapping(mappingFile);

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
    location.setBoundingBox(QGeoRectangle());
    place.setLocation(location);

    QList<QPlaceContactDetail> details;
    foreach (const QString &contactType, original.contactTypes())
        place.setContactDetails(contactType, original.contactDetails(contactType));

    place.setVisibility(QLocation::UnspecifiedVisibility);

    QStringList attributeTypes = original.extendedAttributeTypes();
    foreach (const QString &attributeType, attributeTypes)
        place.setExtendedAttribute(attributeType, original.extendedAttribute(attributeType));

    QString provider = original.extendedAttribute(QLatin1String("x_provider")).text();
    if (!provider.isEmpty()) {
        QPlaceAttribute alternativeId;
        alternativeId.setText(original.placeId());
        place.setExtendedAttribute(QString::fromLatin1("x_id_") + provider,
                                   alternativeId);

        if (provider == QLatin1String("nokia") || provider == QLatin1String("nokia_mos")) {
            QStringList nokiaCategoryIds;
            foreach (const QPlaceCategory &cat, original.categories()) {
                if (!cat.categoryId().isEmpty())
                    nokiaCategoryIds.append(cat.categoryId());
            }

            if (!nokiaCategoryIds.isEmpty()) {
                QPlaceAttribute nokiaCatIds;
                nokiaCatIds.setText(nokiaCategoryIds.join(QLatin1String(",")));
                place.setExtendedAttribute(QString::fromLatin1("x_nokia_category_ids"), nokiaCatIds);
            }
        }
    }

    QVariantMap parameters;
    QPlaceIcon originalIcon = original.icon();
    if (!originalIcon.isEmpty()) {
        if (originalIcon.parameters().contains(QPlaceIcon::SingleUrl)) {
                parameters.insert(JsonDbIcon::MediumSource, originalIcon.url(JsonDbIcon::MediumSize));
        } else if (originalIcon.manager()) {
            if (originalIcon.manager()->managerName() == QLatin1String("nokia") ||
                 originalIcon.manager()->managerName() == QLatin1String("nokia_mos")) {
                QString nokiaIcon = originalIcon.parameters()
                                    .value(JsonDbIcon::NokiaIcon).toString();
                if (!nokiaIcon.isEmpty())
                    parameters.insert(JsonDbIcon::NokiaIcon, nokiaIcon);

                bool nokiaIconGenerated = originalIcon.parameters()
                        .value(JsonDbIcon::NokiaIconGenerated).toBool() == true;
                if (nokiaIconGenerated)
                     parameters.insert(JsonDbIcon::NokiaIconGenerated, nokiaIconGenerated);
            } else if (originalIcon.manager()->managerName()
                            == QLatin1String("places_jsondb")) {
                parameters = originalIcon.parameters();
            } else {
                if (!originalIcon.url(JsonDbIcon::SmallSize).isEmpty())
                    parameters.insert(JsonDbIcon::SmallSource, originalIcon.url(JsonDbIcon::SmallSize));
                if (!originalIcon.url(JsonDbIcon::MediumSize).isEmpty())
                    parameters.insert(JsonDbIcon::MediumSource, originalIcon.url(JsonDbIcon::MediumSize));
                if (!originalIcon.url(JsonDbIcon::LargeSize).isEmpty())
                    parameters.insert(JsonDbIcon::LargeSource, originalIcon.url(JsonDbIcon::LargeSize));
            }
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
    QUrl iconUrl;
    if (m_useCustomIcons) {
        QList<QPair<int, QUrl> > candidates;
        //TODO: possible optimizations
        QMap<QString, QSize> sizeDictionary;
        sizeDictionary.insert(JsonDbIcon::SmallDestination, JsonDbIcon::SmallSize);

        sizeDictionary.insert(JsonDbIcon::MediumDestination, JsonDbIcon::MediumSize);
        sizeDictionary.insert(JsonDbIcon::LargeDestination, JsonDbIcon::LargeSize);
        sizeDictionary.insert(JsonDbIcon::FullscreenDestination, JsonDbIcon::FullscreenSize);

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

        if (candidates.isEmpty()) {
            iconUrl = QUrl();
        } else if (candidates.count() == 1) {
            iconUrl = candidates.first().second;
        } else {
            //we assume icons are squarish so we can use height to
            //determine which particular icon to return
            int requestedHeight = size.height();

            for (int i = 0; i < candidates.count() - 1; ++i) {
                int thresholdHeight = (candidates.at(i).first + candidates.at(i+1).first) / 2;
                if (requestedHeight < thresholdHeight)
                    return candidates.at(i).second;
            }
            iconUrl = candidates.last().second;
        }
    } else {
        QString nokiaIcon = icon.parameters().value(JsonDbIcon::NokiaIcon).toString();
        if (!nokiaIcon.isEmpty()) {
            QString nokiaIconPath = m_localDataPath + nokiaIcon;
            if (QFile::exists(nokiaIconPath)) {
                if (nokiaIconPath.contains(QLatin1String("/icons/categories"))) {
                    if (m_useFavoriteBadges) {
                        nokiaIconPath.replace(QStringLiteral(".icon"),
                                                 QStringLiteral("_01.icon"));
                    }

                    if (!m_theme.isEmpty())
                        nokiaIconPath += QLatin1Char('.') + m_theme;
                }

                iconUrl = QUrl::fromLocalFile(nokiaIconPath);
            }
        }
    }

    return iconUrl;
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

QNetworkAccessManager *QPlaceManagerEngineJsonDb::networkAccessManager()
{
    if (!m_netManager)
        m_netManager = new QNetworkAccessManager(this);
    return m_netManager;
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
    m_queuedNotifications.append(notifications);

    CategoryTraverser *traverser = new CategoryTraverser(m_jsonDb, this);
    connect(traverser, SIGNAL(finished()), this, SLOT(reinitializeCategories()));
    traverser->start();
}

void QPlaceManagerEngineJsonDb::notificationsError(QJsonDbWatcher::ErrorCode code, const QString &errorString)
{
    qWarning() << Q_FUNC_INFO << " Error code: " << code << " Error String: " << errorString;
}

void QPlaceManagerEngineJsonDb::reinitializeCategories()
{
    CategoryTraverser *traverser = qobject_cast<CategoryTraverser *>(sender());
    if (traverser->errorString().isEmpty()) {
        CategoryTree tree = CategoryTraverser::convertToTree(traverser->results(), this);
        setCategoryTree(tree);
    } else {
        qWarning() << QString::fromLatin1("Error trying to reinitializing categories "
                                        "errorString: %1").arg(traverser->errorString());
    }
    traverser->deleteLater();

    if (!m_queuedNotifications.isEmpty()) {
        QList<QJsonDbNotification> notifications = m_queuedNotifications.takeFirst();
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
}
