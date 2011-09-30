/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QPLACEMANAGERENGINE_H
#define QPLACEMANAGERENGINE_H

#include "qplacemanager.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Location)

class QPlaceManagerEnginePrivate;

class Q_LOCATION_EXPORT QPlaceManagerEngine : public QObject
{
    Q_OBJECT
public:

    QPlaceManagerEngine(const QMap<QString, QVariant> &parameters, QObject *parent = 0);
    virtual ~QPlaceManagerEngine();

    QString managerName() const;
    int managerVersion() const;

    virtual QPlaceDetailsReply *getPlaceDetails(const QString &placeId) = 0;

    virtual QPlaceContentReply *getContent(const QPlace &place, const QPlaceContentRequest &request) = 0;

    virtual QPlaceReply *postRating(const QString &placeId, qreal value) = 0;

    virtual QPlaceSearchReply *searchForPlaces(const QPlaceSearchRequest &request) = 0;

    virtual QPlaceSearchReply *recommendations(const QPlace &place, const QPlaceSearchRequest &request) = 0;
    virtual QPlaceTextPredictionReply *textPredictions(const QPlaceSearchRequest &request) = 0;

    virtual QPlaceIdReply *savePlace(const QPlace &place, QPlaceManager::VisibilityScope scope) = 0;
    virtual QPlaceIdReply *removePlace(const QPlace &place) = 0;
    virtual QPlaceManager::VisibilityScopes supportedSaveVisibilityScopes() const = 0;

    virtual QPlaceIdReply *saveCategory(const QPlaceCategory &category, const QString &parentId) = 0;
    virtual QPlaceIdReply *removeCategory(const QString &categoryId) = 0;

    virtual QPlaceReply *initializeCategories() = 0;
    virtual QString parentCategoryId(const QString &categoryId) const = 0;
    virtual QStringList childrenCategoryIds(const QString &categoryId) const = 0;
    virtual QPlaceCategory category(const QString &categoryId) const = 0;

    virtual QList<QPlaceCategory> childCategories(const QString &parentId) const = 0;

    virtual QLocale locale() const = 0;
    virtual void setLocale(const QLocale &locale) = 0;

Q_SIGNALS:
    void finished(QPlaceReply *reply);
    void error(QPlaceReply *, QPlaceReply::Error error, QString errorString = QString());
    void authenticationRequired(QAuthenticator *authenticator);

    void placeAdded(const QString &placeId);
    void placeUpdated(const QString &placeId);
    void placeRemoved(const QString &placeId);

    void categoryAdded(const QPlaceCategory &category, const QString &parentCategoryId);
    void categoryUpdated(const QPlaceCategory &category, const QString &parentCategoryId);
    void categoryRemoved(const QString &categoryId, const QString &parentCategoryId);

private:
    void setManagerName(const QString &managerName);
    void setManagerVersion(int managerVersion);

    QPlaceManagerEnginePrivate *d_ptr;
    Q_DISABLE_COPY(QPlaceManagerEngine)

    friend class QGeoServiceProvider;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
