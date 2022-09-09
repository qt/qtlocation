/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
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

#include "qplaceratings.h"
#include "qplaceratings_p.h"

QT_USE_NAMESPACE

QT_DEFINE_QSDP_SPECIALIZATION_DTOR(QPlaceRatingsPrivate)

bool QPlaceRatingsPrivate::operator==(const QPlaceRatingsPrivate &other) const
{
    return average == other.average && maximum == other.maximum && count == other.count;
}

bool QPlaceRatingsPrivate::isEmpty() const
{
    return count == 0 && average == 0 && maximum == 0;
}

/*!
    \class QPlaceRatings
    \inmodule QtLocation
    \ingroup QtLocation-places
    \ingroup QtLocation-places-data
    \since 5.6

    \brief The QPlaceRatings class holds rating information about a place.

    Rating information is used to describe how good a place is conceived to be.
    Typically this information is visualized as a number of stars.
    The average() function returns an aggregated ratings value out of a possible
    maximum as given by the maximum() function.

    \snippet places/requesthandler.h Ratings
*/

/*!
    \qmlvaluetype ratings
    \inqmlmodule QtLocation
    \ingroup qml-QtLocation5-places
    \ingroup qml-QtLocation5-places-data
    \since QtLocation 5.5

    \brief The ratings type holds place rating information.

    Rating information is used to describe how \e good a place is conceived to be.  Typically this
    information is visualized as a number of stars.  The \l average property gives an aggregated
    ratings value out of a possible maximum as given by the \l maximum property.

    \snippet declarative/places.qml QtQuick import
    \snippet declarative/maps.qml QtLocation import
    \codeline
    \snippet declarative/places.qml ratings
*/

/*!
    Constructs a new ratings object.
*/
QPlaceRatings::QPlaceRatings()
    : d(new QPlaceRatingsPrivate)
{
}

/*!
    Constructs a copy of \a other.
*/
QPlaceRatings::QPlaceRatings(const QPlaceRatings &other) noexcept = default;

/*!
    Destroys the ratings object.
*/
QPlaceRatings::~QPlaceRatings() = default;

/*!
    Assigns \a other to this ratings object and returns
    a reference to this ratings object.
*/
QPlaceRatings &QPlaceRatings::operator=(const QPlaceRatings &other) noexcept
{
    if (this == &other)
        return *this;

    d = other.d;
    return *this;
}

/*!
    \fn bool QPlaceRatings::operator==(const QPlaceRatings &lhs, const QPlaceRatings &rhs) noexcept

    Returns true if \a lhs is equal to \a rhs, otherwise returns false.
*/

/*!
    \fn bool QPlaceRatings::operator!=(const QPlaceRatings &lhs, const QPlaceRatings &rhs) noexcept

    Returns true if \a lhs is not equal to \a rhs, otherwise returns false.
*/

bool QPlaceRatings::isEqual(const QPlaceRatings &other) const noexcept
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    \qmlproperty real ratings::average

    This property holds the average of the individual ratings.

    \sa maximum
*/

/*!
    \property QPlaceRatings::average
    \brief the average value of individual ratings.
*/
qreal QPlaceRatings::average() const
{
    return d->average;
}

void QPlaceRatings::setAverage(qreal average)
{
    d->average = average;
}

/*!
    \qmlproperty real ratings::maximum

    This property holds the maximum rating value.
*/

/*!
    \property QPlaceRatings::maximum
    \brief the maximum possible rating value.
*/
qreal QPlaceRatings::maximum() const
{
    return d->maximum;
}

void QPlaceRatings::setMaximum(qreal max)
{
    d->maximum = max;
}

/*!
    \qmlproperty int ratings::count

    This property holds the total number of individual user ratings
    used in determining the overall ratings \l average.
*/

/*!
    \property QPlaceRatings::count
    \brief the total number of individual ratings.
*/
int QPlaceRatings::count() const
{
    return d->count;
}

void QPlaceRatings::setCount(int count)
{
    d->count = count;
}

/*!
    Returns true if all fields of the place ratings are 0; otherwise returns false.
*/
bool QPlaceRatings::isEmpty() const
{
    return d->isEmpty();
}

#include "moc_qplaceratings.cpp"
