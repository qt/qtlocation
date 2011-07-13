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

#include "qplacereviewreply.h"
#include "qplacereply_p.h"

#include "qgeoplace.h"

QT_BEGIN_NAMESPACE
class QPlaceReviewReplyPrivate : public QPlaceReplyPrivate
{
public:
    QPlaceReviewReplyPrivate() {}
    QPlacePaginationList<QPlaceReview> reviews;
    int totalCount;
};

QT_END_NAMESPACE

QT_USE_NAMESPACE

/*!
    \class QPlaceReviewReply

    \brief The QPlaceReviewReply class manages a review retrieval operation started by an
    instance of QPlaceManager.

    \inmodule QtLocation

    \ingroup places-main
*/

/*!
    Constructs a review reply with a given \a parent.
*/
QPlaceReviewReply::QPlaceReviewReply(QObject *parent)
    : QPlaceReply(new QPlaceReviewReplyPrivate, parent)
{
}

/*!
    Destroys the reply.
*/
QPlaceReviewReply::~QPlaceReviewReply()
{
}

/*!
    Returns the type of reply.
*/
QPlaceReply::Type QPlaceReviewReply::type() const
{
    return QPlaceReply::ReviewReply;
}

 /*!
    Returns the reviews.
*/
QPlacePaginationList<QPlaceReview> QPlaceReviewReply::reviews() const
{
    Q_D(const QPlaceReviewReply);
    return d->reviews;
}

/*!
    Sets the \a reviews.
*/
void QPlaceReviewReply::setReviews(const QList<QPlaceReview> &reviews)
{
    Q_D(QPlaceReviewReply);
    d->reviews.setData(reviews);
}

/*!
    Sets the \a offset number of the first media objects on the current page.
*/
void QPlaceReviewReply::setOffset(int offset)
{
    Q_D(QPlaceReviewReply);
    d->reviews.setStart(offset);
}

/*!
    Returns the total number of reviews for a place.  If the total number of
    reviews cannot be counted a value of -1 is returned.
*/
int QPlaceReviewReply::totalCount() const
{
    Q_D(const QPlaceReviewReply);
    return d->totalCount;
}

/*!
    Sets the \a total number of media objects for a place.
*/
void QPlaceReviewReply::setTotalCount(int total)
{
    Q_D(QPlaceReviewReply);
    d->totalCount = total;
}
