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

#ifndef QPLACEMANAGERENGINE_H
#define QPLACEMANAGERENGINE_H

#include "qplacemanager.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QPlaceManagerEnginePrivate;

class Q_LOCATION_EXPORT QPlaceManagerEngine : public QObject
{
    Q_OBJECT

public:
    QPlaceManagerEngine(const QMap<QString, QVariant> &parameters, QObject *parent = 0);
    virtual ~QPlaceManagerEngine();

    QString managerName() const;
    int managerVersion() const;

    virtual QPlaceDetailsReply *getPlaceDetails(const QString &placeId);

    virtual QPlaceContentReply *getPlaceContent(const QString &placeId,
                                                const QPlaceContentRequest &request);

    virtual QPlaceSearchReply *search(const QPlaceSearchRequest &request);

    virtual QPlaceSearchReply *recommendations(const QString &placeId,
                                               const QPlaceSearchRequest &request);
    virtual QPlaceSearchSuggestionReply *searchSuggestions(const QPlaceSearchRequest &request);

    virtual QPlaceIdReply *savePlace(const QPlace &place);
    virtual QPlaceIdReply *removePlace(const QString &placeId);

    virtual QPlaceIdReply *saveCategory(const QPlaceCategory &category, const QString &parentId);
    virtual QPlaceIdReply *removeCategory(const QString &categoryId);

    virtual QPlaceReply *initializeCategories();
    virtual QString parentCategoryId(const QString &categoryId) const;
    virtual QStringList childCategoryIds(const QString &categoryId) const;
    virtual QPlaceCategory category(const QString &categoryId) const;

    virtual QList<QPlaceCategory> childCategories(const QString &parentId) const;

    virtual QList<QLocale> locales() const;
    virtual void setLocales(const QList<QLocale> &locales);

    virtual QUrl constructIconUrl(const QPlaceIcon &icon, const QSize &size) const;

    virtual QPlace compatiblePlace(const QPlace &original) const;

    virtual QPlaceMatchReply *matchingPlaces(const QPlaceMatchRequest &request);

Q_SIGNALS:
    void finished(QPlaceReply *reply);
    void error(QPlaceReply *, QPlaceReply::Error error, const QString &errorString = QString());

    void placeAdded(const QString &placeId);
    void placeUpdated(const QString &placeId);
    void placeRemoved(const QString &placeId);

    void categoryAdded(const QPlaceCategory &category, const QString &parentCategoryId);
    void categoryUpdated(const QPlaceCategory &category, const QString &parentCategoryId);
    void categoryRemoved(const QString &categoryId, const QString &parentCategoryId);
    void dataChanged();

protected:
    QPlaceManager *manager() const;

private:
    void setManagerName(const QString &managerName);
    void setManagerVersion(int managerVersion);

    QPlaceManagerEnginePrivate *d_ptr;
    Q_DISABLE_COPY(QPlaceManagerEngine)

    friend class QGeoServiceProviderPrivate;
    friend class QPlaceManager;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
