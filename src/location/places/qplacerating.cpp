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

#include "qplacerating.h"
#include "qplacerating_p.h"

QT_USE_NAMESPACE

QPlaceRatingPrivate::QPlaceRatingPrivate()
    : QSharedData(),
      value(0),
      count(0)
{
}

QPlaceRatingPrivate::QPlaceRatingPrivate(const QPlaceRatingPrivate &other)
    : QSharedData()
{
    this->value = other.value;
    this->count = other.count;
}

QPlaceRatingPrivate::~QPlaceRatingPrivate()
{
}

bool QPlaceRatingPrivate::operator==(const QPlaceRatingPrivate &other) const
{
    return (
            this->value == other.value
            && this->count == other.count
    );
}

/*!
    \class QPlaceRating

    \inmodule QPlaces

    \brief The QPlaceRating class represents a rating object.

    Each QPlaceRating represents a rating object with a count and value.

    QPlaceRating is an in memory representation of a rating object.
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
    Destructor.
*/
QPlaceRating::~QPlaceRating()
{
}

QPlaceRating &QPlaceRating::operator =(const QPlaceRating &other) {
    d = other.d;
    return *this;
}

bool QPlaceRating::operator==(const QPlaceRating &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns value.
*/
qreal QPlaceRating::value() const
{
    return d->value;
}

/*!
    Sets the \a value.
*/
void QPlaceRating::setValue(const qreal &value)
{
    d->value = value;
}

/*!
    Returns count.
*/
int QPlaceRating::count() const
{
    return d->count;
}

/*!
    Sets the \a count.
*/
void QPlaceRating::setCount(const int &count)
{
    d->count = count;
}
