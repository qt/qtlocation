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
** This file is part of the Ovi services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file OVI_SERVICES_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Ovi services
** plugin source code.
**
****************************************************************************/

#ifndef QPLACEMANAGERENGINE_NOKIAV2_H
#define QPLACEMANAGERENGINE_NOKIAV2_H

#include <QtCore/QWeakPointer>
#include <QtNetwork/QNetworkReply>
#include <QtLocation/QPlaceManagerEngine>
#include <QtLocation/QGeoServiceProvider>

QT_BEGIN_NAMESPACE

class QPlaceContentReply;
class QNetworkReply;
class QNetworkAccessManager;
class QPlaceCategoriesReplyImpl;
class QNetworkInfo;
class QGeoNetworkAccessManager;

struct PlaceCategoryNode
{
    QString parentId;
    QStringList childIds;
    QPlaceCategory category;
};

typedef QMap<QString, PlaceCategoryNode> QPlaceCategoryTree;

class QPlaceManagerEngineNokiaV2 : public QPlaceManagerEngine
{
    Q_OBJECT

public:
    QPlaceManagerEngineNokiaV2(QGeoNetworkAccessManager* networkManager,
                               const QMap<QString, QVariant> &parameters,
                               QGeoServiceProvider::Error *error,
                               QString *errorString);
    ~QPlaceManagerEngineNokiaV2();

    QPlaceDetailsReply *getPlaceDetails(const QString &placeId);

    QPlaceContentReply *getPlaceContent(const QString &placeId, const QPlaceContentRequest &request);

    QPlaceSearchReply *search(const QPlaceSearchRequest &query);

    QPlaceSearchReply *recommendations(const QString &placeId, const QPlaceSearchRequest &query);
    QPlaceSearchSuggestionReply *searchSuggestions(const QPlaceSearchRequest &query);

    QPlaceIdReply *savePlace(const QPlace &place);
    QPlaceIdReply *removePlace(const QString &placeId);

    QPlaceIdReply *saveCategory(const QPlaceCategory &category, const QString &parentId);
    QPlaceIdReply *removeCategory(const QString &categoryId);

    QPlaceReply *initializeCategories();
    QString parentCategoryId(const QString &categoryId) const;
    QStringList childCategoryIds(const QString &categoryId) const;
    QPlaceCategory category(const QString &categoryId) const;
    QList<QPlaceCategory> childCategories(const QString &parentId) const;

    QList<QLocale> locales() const;
    void setLocales(const QList<QLocale> &locales);

    QString iconPath(const QString &remotePath) const;

private:
    QNetworkReply *sendRequest(const QUrl &url);
    QByteArray createLanguageString() const;

private slots:
    void replyFinished();
    void replyError(QPlaceReply::Error error_, const QString &errorString);
    void categoryReplyFinished();
    void categoryReplyError();
#ifdef USE_CHINA_NETWORK_REGISTRATION
    void currentMobileCountryCodeChanged(int interface, const QString &mcc);
#endif

private:
    QGeoNetworkAccessManager *m_manager;
#ifdef USE_CHINA_NETWORK_REGISTRATION
    QNetworkInfo *m_networkInfo;
#endif

    QList<QLocale> m_locales;

    QPlaceCategoryTree m_categoryTree;

    QWeakPointer<QPlaceCategoriesReplyImpl> m_categoryReply;
    QHash<QString, QNetworkReply *> m_categoryRequests;

    QString m_appId;
    QString m_appCode;
    QString m_host;

    QString m_placesServer;
    QString m_localDataPath;
    QString m_theme;
};

QT_END_NAMESPACE

#endif // QPLACEMANAGERENGINE_NOKIAV2_H
