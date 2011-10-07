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

#include "qplacereview.h"
#include "qplacereview_p.h"

QT_BEGIN_NAMESPACE

QPlaceReviewPrivate::QPlaceReviewPrivate()
:   QPlaceContentPrivate(), rating(0)
{
}

QPlaceReviewPrivate::QPlaceReviewPrivate(const QPlaceReviewPrivate &other)
    : QPlaceContentPrivate(other)
{
    dateTime = other.dateTime;
    content = other.content;
    language = other.language;
    rating = other.rating;
    reviewId = other.reviewId;
    title = other.title;
}

QPlaceReviewPrivate::~QPlaceReviewPrivate()
{
}

bool QPlaceReviewPrivate::compare(const QPlaceContentPrivate *other) const
{
    const QPlaceReviewPrivate *od = static_cast<const QPlaceReviewPrivate *>(other);
    return dateTime == od->dateTime &&
           content == od->content &&
           language == od->language &&
           rating == od->rating &&
           reviewId == od->reviewId &&
           title == od->title;
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
    Returns the date and time that the review was written.
*/
QDateTime QPlaceReview::dateTime() const
{
    Q_D(const QPlaceReview);
    return d->dateTime;
}

/*!
    Sets the date and time that the review was written to \a dt.
*/
void QPlaceReview::setDateTime(const QDateTime &dt)
{
    Q_D(QPlaceReview);
    d->dateTime = dt;
}

/*!
    Returns description.
*/
QString QPlaceReview::content() const
{
    Q_D(const QPlaceReview);
    return d->content;
}

/*!
    Sets description.
*/
void QPlaceReview::setContent(const QString &data)
{
    Q_D(QPlaceReview);
    d->content = data;
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
void QPlaceReview::setRating(qreal data)
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

QT_END_NAMESPACE
