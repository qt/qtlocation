/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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

#ifndef QPLACEMANAGERENGINE_JSONDB_H
#define QPLACEMANAGERENGINE_JSONDB_H

#include "jsondb.h"

#include <qplacemanagerengine.h>
#include <qgeoserviceprovider.h>

#include <QtCore/QEventLoop>
#include <QtCore/QMutex>

#include <QtJsonDb/QJsonDbConnection>
#include <QtJsonDb/QJsonDbWatcher>

QT_BEGIN_NAMESPACE

QT_USE_NAMESPACE_JSONDB

class QNetworkAccessManager;

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
    const JsonDb *db() const { return m_jsonDb;}
    QPlaceManager *manager() const { return QPlaceManagerEngine::manager(); }
    void setCategoryTree(const CategoryTree &tree);

    QNetworkAccessManager *networkAccessManager();
    QString localDataPath() const { return m_localDataPath; }
    bool useCustomIcons() const { return m_useCustomIcons; }

private slots:
    void processPlaceNotifications(const QList<QJsonDbNotification> &notifications);
    void processCategoryNotifications(const QList<QJsonDbNotification> &notifications);
    void notificationsError(QtJsonDb::QJsonDbWatcher::ErrorCode code, const QString &errorString);
    void reinitializeCategories();

private:
    JsonDb *m_jsonDb;

    mutable QMutex m_treeMutex;
    CategoryTree m_tree;
    QNetworkAccessManager *m_netManager;

    QString m_localDataPath;
    QString m_theme;
    bool m_useFavoriteBadges;
    bool m_useCustomIcons;

    QList<QList<QJsonDbNotification> > m_queuedNotifications;

    static const QLatin1String PartitionKey;
    static const QLatin1String LocalDataPathKey;

    static const QLatin1String IconThemeKey;
    static const QLatin1String CustomIconsKey;
    static const QLatin1String FavoriteBadgesKey;

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
