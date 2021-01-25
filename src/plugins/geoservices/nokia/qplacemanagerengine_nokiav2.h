/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef QPLACEMANAGERENGINE_NOKIAV2_H
#define QPLACEMANAGERENGINE_NOKIAV2_H

#include <QtCore/QPointer>
#include <QtNetwork/QNetworkReply>
#include <QtLocation/QPlaceManagerEngine>
#include <QtLocation/QGeoServiceProvider>

QT_BEGIN_NAMESPACE

class QPlaceContentReply;
class QNetworkReply;
class QNetworkAccessManager;
class QPlaceCategoriesReplyHere;
class QGeoNetworkAccessManager;
class QGeoUriProvider;

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
    QPlaceManagerEngineNokiaV2(QGeoNetworkAccessManager *networkManager,
                               const QVariantMap &parameters,
                               QGeoServiceProvider::Error *error,
                               QString *errorString);
    ~QPlaceManagerEngineNokiaV2();

    QPlaceDetailsReply *getPlaceDetails(const QString &placeId) override;

    QPlaceContentReply *getPlaceContent(const QPlaceContentRequest &request) override;

    QPlaceSearchReply *search(const QPlaceSearchRequest &query) override;

    QPlaceSearchSuggestionReply *searchSuggestions(const QPlaceSearchRequest &query) override;

    QPlaceIdReply *savePlace(const QPlace &place) override;
    QPlaceIdReply *removePlace(const QString &placeId) override;

    QPlaceIdReply *saveCategory(const QPlaceCategory &category, const QString &parentId) override;
    QPlaceIdReply *removeCategory(const QString &categoryId) override;

    QPlaceReply *initializeCategories() override;
    QString parentCategoryId(const QString &categoryId) const override;
    QStringList childCategoryIds(const QString &categoryId) const override;
    QPlaceCategory category(const QString &categoryId) const override;
    QList<QPlaceCategory> childCategories(const QString &parentId) const override;

    QList<QLocale> locales() const override;
    void setLocales(const QList<QLocale> &locales) override;

    QPlaceIcon icon(const QString &remotePath,
                    const QList<QPlaceCategory> &categories = QList<QPlaceCategory>()) const;

    QUrl constructIconUrl(const QPlaceIcon &icon, const QSize &size) const override;

private:
    QNetworkReply *sendRequest(const QUrl &url);
    QByteArray createLanguageString() const;

private Q_SLOTS:
    void replyFinished();
    void replyError(QPlaceReply::Error error_, const QString &errorString);
    void categoryReplyFinished();
    void categoryReplyError();

private:
    QGeoNetworkAccessManager *m_manager;
    QGeoUriProvider *m_uriProvider;

    QList<QLocale> m_locales;

    QPlaceCategoryTree m_categoryTree;
    QPlaceCategoryTree m_tempTree;
    QHash<QString, QString> m_restIdToIconHash;

    QPointer<QPlaceCategoriesReplyHere> m_categoryReply;
    QHash<QString, QNetworkReply *> m_categoryRequests;

    QString m_appId;
    QString m_appCode;

    QString m_localDataPath;
    QString m_theme;
};

QT_END_NAMESPACE

#endif // QPLACEMANAGERENGINE_NOKIAV2_H
