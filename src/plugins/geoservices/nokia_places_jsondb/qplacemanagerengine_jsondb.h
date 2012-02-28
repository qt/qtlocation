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

#ifndef QPLACEMANAGERENGINE_NOKIA_JSONDB_H
#define QPLACEMANAGERENGINE_NOKIA_JSONDB_H

#include "jsondb.h"

#include <qplacemanagerengine.h>
#include <qgeoserviceprovider.h>

#include <QtCore/QEventLoop>
#include <QtCore/QMutex>

#include <QtJsonDb/QJsonDbConnection>
#include <QtJsonDb/QJsonDbWatcher>

QT_BEGIN_NAMESPACE

QT_USE_NAMESPACE_JSONDB

struct CategoryNode {
    QString parentId;
    QStringList childIds;
    QPlaceCategory category;
};

typedef QMap<QString, CategoryNode> CategoryTree;

class QPlaceManagerEngineJsonDb : public QPlaceManagerEngine
{
    Q_OBJECT
public:

    QPlaceManagerEngineJsonDb(const QMap<QString, QVariant> &parameters, QGeoServiceProvider::Error *error,
                                                                        QString *errorString);
    ~QPlaceManagerEngineJsonDb();

    QPlaceDetailsReply *getPlaceDetails(const QString &placeId);

    QPlaceContentReply *getPlaceContent(const QString &placeId, const QPlaceContentRequest &request);

    QPlaceSearchReply *search(const QPlaceSearchRequest &request);

    QPlaceSearchReply *recommendations(const QString &placeId, const QPlaceSearchRequest &request);
    QPlaceSearchSuggestionReply *searchSuggestions(const QPlaceSearchRequest &request);

    QPlaceIdReply *savePlace(const QPlace &place);
    QPlaceIdReply *removePlace(const QString &placeId);

    QPlaceIdReply *saveCategory(const QPlaceCategory &category, const QString &parentId);
    QPlaceIdReply *removeCategory(const QString &categoryId);

    QPlaceMatchReply * matchingPlaces(const QPlaceMatchRequest &request);

    QPlaceReply *initializeCategories();
    QString parentCategoryId(const QString &categoryId) const;
    QStringList childCategoryIds(const QString &categoryId) const;
    QPlaceCategory category(const QString &categoryId) const;

    QList<QPlaceCategory> childCategories(const QString &parentId) const;

    QList<QLocale> locales() const;
    void setLocales(const QList<QLocale> &locales);

    QPlace compatiblePlace(const QPlace &original) const;
    QUrl constructIconUrl(const QPlaceIcon &icon, const QSize &size) const;

    JsonDb *db() { return m_jsonDb;}
    QPlaceManager *manager() const { return QPlaceManagerEngine::manager(); }
    void setCategoryTree(const CategoryTree &tree);

private slots:
    void processPlaceNotifications(const QList<QJsonDbNotification> &notifications);
    void processCategoryNotifications(const QList<QJsonDbNotification> &notifications);
    void notificationsError(QtJsonDb::QJsonDbWatcher::ErrorCode code, const QString &errorString);

private:
    bool isSupportedScheme(const QString &scheme) const;

    JsonDb *m_jsonDb;

    mutable QMutex m_treeMutex;
    CategoryTree m_tree;

    friend class SaveReply;
    friend class MediaReply;
    friend class RemoveReply;
    friend class SearchSuggestionReply;
    friend class ReviewReply;
    friend class Reply;
    friend class SearchReply;
    friend class CategoryInitReply;
};

QT_END_NAMESPACE

#endif
