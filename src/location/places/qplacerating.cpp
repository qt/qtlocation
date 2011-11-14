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

#include "qplacerating.h"
#include "qplacerating_p.h"

QT_USE_NAMESPACE

QPlaceRatingPrivate::QPlaceRatingPrivate()
:   QSharedData(), value(0), maximum(0), count(0)
{
}

QPlaceRatingPrivate::QPlaceRatingPrivate(const QPlaceRatingPrivate &other)
:   QSharedData(), value(other.value), maximum(other.maximum), count(other.count)
{
}

QPlaceRatingPrivate::~QPlaceRatingPrivate()
{
}

bool QPlaceRatingPrivate::operator==(const QPlaceRatingPrivate &other) const
{
    return value == other.value && maximum == other.maximum && count == other.count;
}

/*!
    \class QPlaceRating
    \inmodule QtLocation
    \ingroup QtLocation-places
    \ingroup QtLocation-places-data
    \since QtLocation 5.0

    \brief The QPlaceRating class contains an aggregated rating for a place.
*/

/*!
    Constructs a new rating object.
*/
QPlaceRating::QPlaceRating()
    : d(new QPlaceRatingPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceRating::QPlaceRating(const QPlaceRating &other)
    :d(other.d)
{
}

/*!
    Destroys the rating object.
*/
QPlaceRating::~QPlaceRating()
{
}

/*!
    Assigns \a other to this rating and returns a reference
    to this rating.
*/
QPlaceRating &QPlaceRating::operator=(const QPlaceRating &other) {
    d = other.d;
    return *this;
}

/*!
    Returns true if \a other is equal to this rating,
    otherwise returns false.
*/
bool QPlaceRating::operator==(const QPlaceRating &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    \fn bool QPlaceRating::operator!=(const QPlaceRating &other) const

    Returns true if \a other is not equal to this rating,
    otherwise returns false
*/

/*!
    Returns the value of the rating which is an aggregation
    of individual ratings.
*/
qreal QPlaceRating::value() const
{
    return d->value;
}

/*!
    Sets the rating's aggregated \a value.
*/
void QPlaceRating::setValue(qreal value)
{
    d->value = value;
}

/*!
    Returns the maximum possible rating value.
*/
qreal QPlaceRating::maximum() const
{
    return d->maximum;
}

/*!
    Sets the maximum possible rating value to \a max.
*/
void QPlaceRating::setMaximum(qreal max)
{
    d->maximum = max;
}

/*!
    Returns the total number of individual ratings.
*/
int QPlaceRating::count() const
{
    return d->count;
}

/*!
    Sets the total number of individual ratings to \a count.
*/
void QPlaceRating::setCount(int count)
{
    d->count = count;
}
