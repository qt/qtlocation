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

#include "qdeclarativereviewmodel_p.h"
#include "qdeclarativesupplier_p.h"

#include <QtLocation/QPlaceReview>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass ReviewModel QDeclarativeReviewModel

    \brief The ReviewModel element provides access to reviews of a Place.
    \ingroup qml-places
    \since 5.0

    The ReviewModel is a read-only model used to fetch reviews about a Place.  The model
    incrementally fetches reviews.  The number of reviews which are fetched at a time is specified
    by the batchSize property.  The total number of reviews available can be accessed via the
    totalCount property and the number of fetched reviews via the count property.

    The model returns data for the following roles:
    \table
        \header
            \o Role
            \o Type
            \o Description
        \row
            \o date
            \o string
            \o The date that the review was posted.
        \row
            \o description
            \o string
            \o The content of the review.
        \row
            \o language
            \o string
            \o The language that the review is written in.
        \row
            \o helpfulVotings
            \o int
            \o The number of votings indicating that the review was helpful.
        \row
            \o unhelpfulVotings
            \o int
            \o The number of votings indicating that the review was not helpful.
        \row
            \o rating
            \o real
            \o The rating that the reviewer gave to the place.
        \row
            \o mediaIds
            \o list
            \o The list of media ids associated with the review.
        \row
            \o reviewId
            \o string
            \o The id of the review.
        \row
            \o supplier
            \o Supplier
            \o The source of the review.
        \row
            \o title
            \o string
            \o The title of the review.
        \row
            \o userId
            \o string
            \o The id of the user who posted the review.
        \row
            \o userName
            \o string
            \o The name of the user who posted the review.
        \row
            \o originatorUrl
            \o url
            \o The URL of the review.
    \endtable
*/

/*!
    \qmlproperty Place ReviewModel::place

    This property holds the Place that the reviews are for.
*/

/*!
    \qmlproperty int ReviewModel::batchSize

    This property holds the batch size to use when fetching more reviews.
*/

/*!
    \qmlproperty int ReviewModel::totalCount

    This property holds the total number of reviews for the place.
*/

QDeclarativeReviewModel::QDeclarativeReviewModel(QObject* parent)
:   QDeclarativePlaceContentModel(QPlaceContent::ReviewType, parent)
{
    QHash<int, QByteArray> roleNames;
    roleNames.insert(DateRole, "date");
    roleNames.insert(DescriptionRole, "description");
    roleNames.insert(LanguageRole, "language");
    roleNames.insert(HelpfulVotingsRole, "helpfulVotings");
    roleNames.insert(UnhelpfulVotingsRole, "unhelpfulVotings");
    roleNames.insert(RatingRole, "rating");
    roleNames.insert(MediaIdsRole, "mediaIds");
    roleNames.insert(ReviewIdRole, "reviewId");
    roleNames.insert(SupplierRole, "supplier");
    roleNames.insert(TitleRole, "title");
    roleNames.insert(UserIdRole, "userId");
    roleNames.insert(UserNameRole, "userName");
    roleNames.insert(OriginatorUrlRole, "originatorUrl");
    setRoleNames(roleNames);
}

QDeclarativeReviewModel::~QDeclarativeReviewModel()
{
    qDeleteAll(m_suppliers);
}

QVariant QDeclarativeReviewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= rowCount(index.parent()) || index.row() < 0)
        return QVariant();

    const QPlaceReview &review = m_content.value(index.row());

    switch (role) {
    case DateRole:
        return review.date();
    case DescriptionRole:
        return review.description();
    case LanguageRole:
        return review.language();
    case HelpfulVotingsRole:
        return review.helpfulVotings();
    case UnhelpfulVotingsRole:
        return review.unhelpfulVotings();
    case RatingRole:
        return review.rating();
    case MediaIdsRole:
        return review.mediaIds();
    case ReviewIdRole:
        return review.reviewId();
    case TitleRole:
        return review.title();
    case UserIdRole:
        return review.userId();
    case UserNameRole:
        return review.userName();
    case OriginatorUrlRole:
        return review.originatorUrl();
    }

    return QDeclarativePlaceContentModel::data(index, role);
}

QT_END_NAMESPACE
