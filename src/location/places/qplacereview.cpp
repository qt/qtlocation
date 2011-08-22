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
    : QPlaceContentPrivate(),
      helpfulVotings(0),
      unhelpfulVotings(0),
      rating(0)
{
}

QPlaceReviewPrivate::QPlaceReviewPrivate(const QPlaceReviewPrivate &other)
    : QPlaceContentPrivate(other)
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

bool QPlaceReviewPrivate::compare(const QPlaceContentPrivate *other) const
{
    const QPlaceReviewPrivate *od = static_cast<const QPlaceReviewPrivate *>(other);
    return (
            this->date == od->date
            && this->description == od->description
            && this->language == od->language
            && this->helpfulVotings == od->helpfulVotings
            && this->mediaIds == od->mediaIds
            && this->unhelpfulVotings == od->unhelpfulVotings
            && this->rating == od->rating
            && this->reviewId == od->reviewId
            && this->supplier == od->supplier
            && this->title == od->title
            && this->userId == od->userId
            && this->userName == od->userName
            && this->originatorUrl == od->originatorUrl
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
    : QPlaceContent(new QPlaceReviewPrivate)
{
}

Q_IMPLEMENT_CONTENT_COPY_CTOR(QPlaceReview)


/*!
    Destructor.
*/
QPlaceReview::~QPlaceReview()
{
}

Q_IMPLEMENT_CONTENT_D_FUNC(QPlaceReview)

/*!
    Returns review date.
*/
QString QPlaceReview::date() const
{
    Q_D(const QPlaceReview);
    return d->date;
}

/*!
    Sets review date.
*/
void QPlaceReview::setDate(const QString &data)
{
    Q_D(QPlaceReview);
    d->date = data;
}

/*!
    Returns description.
*/
QString QPlaceReview::description() const
{
    Q_D(const QPlaceReview);
    return d->description;
}

/*!
    Sets description.
*/
void QPlaceReview::setDescription(const QString &data)
{
    Q_D(QPlaceReview);
    d->description = data;
}

/*!
    Returns language.
*/
QString QPlaceReview::language() const
{
    Q_D(const QPlaceReview);
    return d->language;
}

/*!
    Sets language.
*/
void QPlaceReview::setLanguage(const QString &data)
{
    Q_D(QPlaceReview);
    d->language = data;
}

/*!
    Returns the number of helpful votings or "thumbs up"
*/
int QPlaceReview::helpfulVotings() const
{
    Q_D(const QPlaceReview);
    return d->helpfulVotings;
}

/*!
    Sets the number of helpful \a votes or "thumbs up".
*/
void QPlaceReview::setHelpfulVotings(const int &votes)
{
    Q_D(QPlaceReview);
    d->helpfulVotings = votes;
}

/*!
    Returns media ids associated with review.
*/
QStringList QPlaceReview::mediaIds() const
{
    Q_D(const QPlaceReview);
    return d->mediaIds;
}

/*!
    Sets media ids.
*/
void QPlaceReview::setMediaIds(const QStringList &data)
{
    Q_D(QPlaceReview);
    d->mediaIds = data;
}

/*!
    Returns the number of uhelpful votings or "thumbs down".
*/
int QPlaceReview::unhelpfulVotings() const
{
    Q_D(const QPlaceReview);
    return d->unhelpfulVotings;
}

/*!
    Sets the number of unhelpful votings or "thumbs down"
*/
void QPlaceReview::setUnhelpfulVotings(const int &votes)
{
    Q_D(QPlaceReview);
    d->unhelpfulVotings = votes;
}

/*!
    Returns rating.
*/
qreal QPlaceReview::rating() const
{
    Q_D(const QPlaceReview);
    return d->rating;
}

/*!
    Sets rating.
*/
void QPlaceReview::setRating(const qreal &data)
{
    Q_D(QPlaceReview);
    d->rating = data;
}

/*!
    Returns review id.
*/
QString QPlaceReview::reviewId() const
{
    Q_D(const QPlaceReview);
    return d->reviewId;
}

/*!
    Sets review id.
*/
void QPlaceReview::setReviewId(const QString &data)
{
    Q_D(QPlaceReview);
    d->reviewId = data;
}

/*!
    Returns supplier. Do not remove it.
*/
QPlaceSupplier QPlaceReview::supplier() const
{
    Q_D(const QPlaceReview);
    return d->supplier;
}

/*!
    Sets supplier.
*/
void QPlaceReview::setSupplier(const QPlaceSupplier &data)
{
    Q_D(QPlaceReview);
    d->supplier = data;
}

/*!
    Returns review title.
*/
QString QPlaceReview::title() const
{
    Q_D(const QPlaceReview);
    return d->title;
}

/*!
    Sets title.
*/
void QPlaceReview::setTitle(const QString &data)
{
    Q_D(QPlaceReview);
    d->title = data;
}

/*!
    Returns user id.
*/
QString QPlaceReview::userId() const
{
    Q_D(const QPlaceReview);
    return d->userId;
}

/*!
    Sets user id.
*/
void QPlaceReview::setUserId(const QString &data)
{
    Q_D(QPlaceReview);
    d->userId = data;
}

/*!
    Returns user name.
*/
QString QPlaceReview::userName() const
{
    Q_D(const QPlaceReview);
    return d->userName;
}

/*!
    Sets user name.
*/
void QPlaceReview::setUserName(const QString &data)
{
    Q_D(QPlaceReview);
    d->userName = data;
}

/*!
    Returns originator URL.
*/
QString QPlaceReview::originatorUrl() const
{
    Q_D(const QPlaceReview);
    return d->originatorUrl;
}

/*!
    Sets originator \a url of the review.
*/
void QPlaceReview::setOriginatorUrl(const QString &url)
{
    Q_D(QPlaceReview);
    d->originatorUrl = url;
}
