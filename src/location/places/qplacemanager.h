/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QPLACEMANAGER_H
#define QPLACEMANAGER_H

#include <QtLocation/QPlaceReply>

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

class QLocale;
class QPlace;
class QPlaceCategory;
class QPlaceContentReply;
class QPlaceContentRequest;
class QPlaceDetailsReply;
class QPlaceIcon;
class QPlaceIdReply;
class QPlaceManagerEngine;
class QPlaceMatchReply;
class QPlaceMatchRequest;
class QPlaceSearchRequest;
class QPlaceSearchReply;
class QPlaceSearchResult;
class QPlaceSearchSuggestionReply;

class Q_LOCATION_EXPORT QPlaceManager : public QObject
{
    Q_OBJECT
public:
    ~QPlaceManager();

    QString managerName() const;
    int managerVersion() const;

    QPlaceDetailsReply *getPlaceDetails(const QString &placeId) const;

    QPlaceContentReply *getPlaceContent(const QPlaceContentRequest &request) const;

    QPlaceSearchReply *search(const QPlaceSearchRequest &query) const;

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

    QPlace compatiblePlace(const QPlace &place) const;

    QPlaceMatchReply *matchingPlaces(const QPlaceMatchRequest &request) const;

Q_SIGNALS:
    void finished(QPlaceReply *reply);
    void errorOccurred(QPlaceReply *, QPlaceReply::Error error,
                       const QString &errorString = QString());

    void placeAdded(const QString &placeId);
    void placeUpdated(const QString &placeId);
    void placeRemoved(const QString &placeId);

    void categoryAdded(const QPlaceCategory &category, const QString &parentId);
    void categoryUpdated(const QPlaceCategory &category, const QString &parentId);
    void categoryRemoved(const QString &categoryId, const QString &parentId);
    void dataChanged();

private:
    explicit QPlaceManager(QPlaceManagerEngine *engine, QObject *parent = nullptr);
    Q_DISABLE_COPY(QPlaceManager)

    QPlaceManagerEngine *d;

    friend class QGeoServiceProvider;
    friend class QGeoServiceProviderPrivate;
    friend class QPlaceIcon;
};

QT_END_NAMESPACE

#endif // QPLACEMANAGER_H
