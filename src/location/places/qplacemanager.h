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

#ifndef QPLACEMANAGER_H
#define QPLACEMANAGER_H

#include "qplacecontentreply.h"
#include "qplacecontentrequest.h"
#include "qplaceidreply.h"
#include "qplacereply.h"
#include "qplacedetailsreply.h"
#include "qplacematchreply.h"
#include "qplacematchrequest.h"
#include "qplacesearchsuggestionreply.h"
#include "qplacesearchrequest.h"
#include "qplacesearchresult.h"

#include <QLocale>
#include <QVector>
#include <QString>
#include <QObject>
#include <QAuthenticator>
#include <QtLocation/QPlaceIcon>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QPlaceManagerEngine;
class QPlaceSearchRequest;
class QPlaceSearchReply;

class Q_LOCATION_EXPORT QPlaceManager : public QObject
{
    Q_OBJECT
public:
    enum ManagerFeature {
        NoFeatures              = 0x0,
        SavePlaceFeature        = 0x1,
        RemovePlaceFeature      = 0x2,
        SaveCategoryFeature     = 0x4,
        RemoveCategoryFeature   = 0x8,
        RecommendationsFeature  = 0x10,
        SearchSuggestionsFeature   = 0x20,
        CorrectionsFeature      = 0x40,
        LocaleFeature           = 0x80,
        NotificationsFeature    = 0x100,
        MatchingFeature         = 0x200
    };

    Q_DECLARE_FLAGS(ManagerFeatures, ManagerFeature)
    Q_FLAGS(ManagerFeatures)

    ~QPlaceManager();

    QString managerName() const;
    int managerVersion() const;

    QPlaceDetailsReply *getPlaceDetails(const QString &placeId) const;

    QPlaceContentReply *getPlaceContent(const QString &placeId, const QPlaceContentRequest &request) const;

    QPlaceSearchReply *search(const QPlaceSearchRequest &query) const;

    QPlaceSearchReply *recommendations(const QString &placeId, const QPlaceSearchRequest &request = QPlaceSearchRequest()) const;

    QPlaceSearchSuggestionReply *searchSuggestions(const QPlaceSearchRequest &request) const;

    QPlaceIdReply *savePlace(const QPlace &place);
    QPlaceIdReply *removePlace(const QString &placeId);

    QPlaceIdReply *saveCategory(const QPlaceCategory &category, const QString &parentId = QString());
    QPlaceIdReply *removeCategory(const QString &categoryId);

    QPlaceReply *initializeCategories();
    QString parentCategoryId(const QString &categoryId) const;
    QStringList childCategoryIds(const QString &parentId = QString()) const;

    QPlaceCategory category(const QString &categoryId) const;
    QList<QPlaceCategory> childCategories(const QString &parentId = QString()) const;

    QList<QLocale> locales() const;
    void setLocale(const QLocale &locale);
    void setLocales(const QList<QLocale> &locale);

    QPlace compatiblePlace(const QPlace &place);

    ManagerFeatures supportedFeatures() const;

    QPlaceMatchReply *matchingPlaces(const QPlaceMatchRequest &request) const;

Q_SIGNALS:
    void finished(QPlaceReply *reply);
    void error(QPlaceReply *, QPlaceReply::Error error, const QString &errorString = QString());

    void placeAdded(const QString &placeId);
    void placeUpdated(const QString &placeId);
    void placeRemoved(const QString &placeId);

    void categoryAdded(const QPlaceCategory &category, const QString &parentId);
    void categoryUpdated(const QPlaceCategory &category, const QString &parentId);
    void categoryRemoved(const QString &categoryId, const QString &parentId);

private:
    QPlaceManager(QPlaceManagerEngine *engine, QObject *parent = 0);
    Q_DISABLE_COPY(QPlaceManager)

    QPlaceManagerEngine *d;

    friend class QGeoServiceProvider;
    friend class QPlaceIcon;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QPlaceManager::ManagerFeatures)

QT_END_NAMESPACE

QT_END_HEADER

#endif // QPLACEMANAGER_H
