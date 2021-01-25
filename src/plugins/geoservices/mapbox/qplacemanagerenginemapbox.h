/****************************************************************************
**
** Copyright (C) 2017 Mapbox, Inc.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtFoo module of the Qt Toolkit.
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
**
**
**
****************************************************************************/

#ifndef QPLACEMANAGERENGINEMAPBOX_H
#define QPLACEMANAGERENGINEMAPBOX_H

#include <QtLocation/QPlaceManagerEngine>
#include <QtLocation/QGeoServiceProvider>

QT_BEGIN_NAMESPACE

class QNetworkAccessManager;

class QPlaceManagerEngineMapbox : public QPlaceManagerEngine
{
    Q_OBJECT

public:
    QPlaceManagerEngineMapbox(const QVariantMap &parameters, QGeoServiceProvider::Error *,
                           QString *errorString);
    ~QPlaceManagerEngineMapbox();

    QPlaceSearchReply *search(const QPlaceSearchRequest &) override;

    QPlaceSearchSuggestionReply *searchSuggestions(const QPlaceSearchRequest &) override;

    QPlaceReply *initializeCategories() override;
    QString parentCategoryId(const QString &categoryId) const override;
    QStringList childCategoryIds(const QString &categoryId) const override;
    QPlaceCategory category(const QString &categoryId) const override;
    QList<QPlaceCategory> childCategories(const QString &parentId) const override;

    QList<QLocale> locales() const override;
    void setLocales(const QList<QLocale> &locales) override;

    // TODO: icon
    //QPlaceIcon icon(const QString &remotePath,
    //                const QList<QPlaceCategory> &categories = QList<QPlaceCategory>()) const;

    //QUrl constructIconUrl(const QPlaceIcon &icon, const QSize &size) const override;

private slots:
    void onReplyFinished();
    void onReplyError(QPlaceReply::Error, const QString &errorString);

private:
    enum PlaceSearchType {
        CompleteSearch = 0,
        SuggestionSearch
    };

    QPlaceReply *doSearch(const QPlaceSearchRequest&, PlaceSearchType);

    QNetworkAccessManager *m_networkManager;
    QByteArray m_userAgent;
    QString m_accessToken;
    QString m_urlPrefix;
    bool m_isEnterprise;

    QList<QLocale> m_locales;
    QHash<QString, QPlaceCategory> m_categories;
};

QT_END_NAMESPACE

#endif // QPLACEMANAGERENGINEMAPBOX_H
