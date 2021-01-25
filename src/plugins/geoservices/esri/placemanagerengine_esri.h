/****************************************************************************
**
** Copyright (C) 2013-2018 Esri <contracts@esri.com>
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
**
**
**
****************************************************************************/

#ifndef PLACEMANAGERENGINEESRI_H
#define PLACEMANAGERENGINEESRI_H

#include <QtLocation/QPlaceManagerEngine>
#include <QtLocation/QGeoServiceProvider>

QT_BEGIN_NAMESPACE

class PlaceCategoriesReplyEsri;
class QNetworkAccessManager;
class QNetworkReply;

class PlaceManagerEngineEsri : public QPlaceManagerEngine
{
    Q_OBJECT

public:
    PlaceManagerEngineEsri(const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString);
    ~PlaceManagerEngineEsri();

    QPlaceSearchReply *search(const QPlaceSearchRequest &request) override;

    QPlaceReply *initializeCategories() override;
    QString parentCategoryId(const QString &categoryId) const override;
    QStringList childCategoryIds(const QString &categoryId) const override;
    QPlaceCategory category(const QString &categoryId) const override;

    QList<QPlaceCategory> childCategories(const QString &parentId) const override;

    QList<QLocale> locales() const override;
    void setLocales(const QList<QLocale> &locales) override;

private slots:
    void geocodeServerReplyFinished();
    void geocodeServerReplyError();
    void replyFinished();
    void replyError(QPlaceReply::Error errorCode, const QString &errorString);

private:
    QNetworkAccessManager *m_networkManager = Q_NULLPTR;

    // geocode serveur
    void initializeGeocodeServer();

    QNetworkReply *m_geocodeServerReply = Q_NULLPTR;

    // categories
    void finishCategories();
    void errorCaterogies(const QString &error);
    void parseCategories(const QJsonArray &jsonArray, const QString &parentCategoryId);

    QList<PlaceCategoriesReplyEsri *> m_pendingCategoriesReply;
    QHash<QString, QPlaceCategory> m_categories;
    QHash<QString, QStringList> m_subcategories;
    QHash<QString, QString> m_parentCategory;

    // localized names
    QString localizedName(const QJsonObject &jsonObject);
    void parseCandidateFields(const QJsonArray &jsonArray);
    void parseCountries(const QJsonArray &jsonArray);

    QList<QLocale> m_locales;
    QHash<QString, QString> m_candidateFieldsLocale;
    QHash<QString, QString> m_countriesLocale;
    void localizedName();
};

QT_END_NAMESPACE

#endif // PLACEMANAGERENGINEESRI_H
