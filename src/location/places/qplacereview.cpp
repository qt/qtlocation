/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#include "qplacereview.h"
#include "qplacereview_p.h"

QT_USE_NAMESPACE

QPlaceReviewPrivate::QPlaceReviewPrivate()
    : QSharedData(),
      helpfulVotings(0),
      unhelpfulVotings(0),
      rating(0)
{
}

QPlaceReviewPrivate::QPlaceReviewPrivate(const QPlaceReviewPrivate &other)
    : QSharedData()
{
    this->date = other.date;
    this->description = other.description;
    this->language = other.language;
    this->helpfulVotings = other.helpfulVotings;
    this->mediaIds = other.mediaIds;
    this->unhelpfulVotings = other.unhelpfulVotings;
    this->rating = other.rating;
    this->reviewId = other.reviewId;
    this->supplier = other.supplier;
    this->title = other.title;
    this->userId = other.userId;
    this->userName = other.userName;
    this->originatorUrl = other.originatorUrl;
}

QPlaceReviewPrivate::~QPlaceReviewPrivate()
{
}

bool QPlaceReviewPrivate::operator==(const QPlaceReviewPrivate &other) const
{
    return (
            this->date == other.date
            && this->description == other.description
            && this->language == other.language
            && this->helpfulVotings == other.helpfulVotings
            && this->mediaIds == other.mediaIds
            && this->unhelpfulVotings == other.unhelpfulVotings
            && this->rating == other.rating
            && this->reviewId == other.reviewId
            && this->supplier == other.supplier
            && this->title == other.title
            && this->userId == other.userId
            && this->userName == other.userName
            && this->originatorUrl == other.originatorUrl
    );
}

/*!
    \class QPlaceReview

    \inmodule QPlaces

    \brief The QPlaceReview class represents a review object.


    Each QPlaceReview represents a review object with a number of attributes
    such as rating, review id, connected media etc. Each QPlaceReview is associated
    with place.

    Review objects are read-only, e.g. user of API might get list of review objects
    associated to specific place but can not edit its content. User might also create new review.

    QPlaceReview is an in memory representation of a review object.
*/


/*!
    Constructs an new review object.
*/
QPlaceReview::QPlaceReview()
    : d(new QPlaceReviewPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceReview::QPlaceReview(const QPlaceReview &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceReview::~QPlaceReview()
{
}

QPlaceReview &QPlaceReview::operator =(const QPlaceReview &other) {
    d = other.d;
    return *this;
}

bool QPlaceReview::operator==(const QPlaceReview &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns review date.
*/
QString QPlaceReview::date() const
{
    return d->date;
}

/*!
    Sets review date.
*/
void QPlaceReview::setDate(const QString &data)
{
    d->date = data;
}

/*!
    Returns description.
*/
QString QPlaceReview::description() const
{
    return d->description;
}

/*!
    Sets description.
*/
void QPlaceReview::setDescription(const QString &data)
{
    d->description = data;
}

/*!
    Returns language.
*/
QString QPlaceReview::language() const
{
    return d->language;
}

/*!
    Sets language.
*/
void QPlaceReview::setLanguage(const QString &data)
{
    d->language = data;
}

/*!
    Returns the number of helpful votings or "thumbs up"
*/
int QPlaceReview::helpfulVotings() const
{
    return d->helpfulVotings;
}

/*!
    Sets the number of helpful \a votes or "thumbs up".
*/
void QPlaceReview::setHelpfulVotings(const int &votes)
{
    d->helpfulVotings = votes;
}

/*!
    Returns media ids associated with review.
*/
QStringList QPlaceReview::mediaIds() const
{
    return d->mediaIds;
}

/*!
    Sets media ids.
*/
void QPlaceReview::setMediaIds(const QStringList &data)
{
    d->mediaIds = data;
}

/*!
    Returns the number of uhelpful votings or "thumbs down".
*/
int QPlaceReview::unhelpfulVotings() const
{
    return d->unhelpfulVotings;
}

/*!
    Sets the number of unhelpful votings or "thumbs down"
*/
void QPlaceReview::setUnhelpfulVotings(const int &votes)
{
    d->unhelpfulVotings = votes;
}

/*!
    Returns rating.
*/
qreal QPlaceReview::rating() const
{
    return d->rating;
}

/*!
    Sets rating.
*/
void QPlaceReview::setRating(const qreal &data)
{
    d->rating = data;
}

/*!
    Returns review id.
*/
QString QPlaceReview::reviewId() const
{
    return d->reviewId;
}

/*!
    Sets review id.
*/
void QPlaceReview::setReviewId(const QString &data)
{
    d->reviewId = data;
}

/*!
    Returns supplier. Do not remove it.
*/
QPlaceSupplier QPlaceReview::supplier() const
{
    return d->supplier;
}

/*!
    Sets supplier.
*/
void QPlaceReview::setSupplier(const QPlaceSupplier &data)
{
    d->supplier = data;
}

/*!
    Returns review title.
*/
QString QPlaceReview::title() const
{
    return d->title;
}

/*!
    Sets title.
*/
void QPlaceReview::setTitle(const QString &data)
{
    d->title = data;
}

/*!
    Returns user id.
*/
QString QPlaceReview::userId() const
{
    return d->userId;
}

/*!
    Sets user id.
*/
void QPlaceReview::setUserId(const QString &data)
{
    d->userId = data;
}

/*!
    Returns user name.
*/
QString QPlaceReview::userName() const
{
    return d->userName;
}

/*!
    Sets user name.
*/
void QPlaceReview::setUserName(const QString &data)
{
    d->userName = data;
}

/*!
    Returns originator URL.
*/
QString QPlaceReview::originatorUrl() const
{
    return d->originatorUrl;
}

/*!
    Sets originator \a url of the review.
*/
void QPlaceReview::setOriginatorUrl(const QString &url)
{
    d->originatorUrl = url;
}
