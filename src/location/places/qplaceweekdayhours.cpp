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

#include "qplaceweekdayhours.h"
#include "qplaceweekdayhours_p.h"

QT_USE_NAMESPACE

QPlaceWeekdayHoursPrivate::QPlaceWeekdayHoursPrivate()
    : QSharedData(),
      weekday(Qt::Monday)
{
}

QPlaceWeekdayHoursPrivate::QPlaceWeekdayHoursPrivate(const QPlaceWeekdayHoursPrivate &other)
    : QSharedData()
{
    this->period = other.period;
    this->weekday = other.weekday;
}

QPlaceWeekdayHoursPrivate::~QPlaceWeekdayHoursPrivate()
{
}

bool QPlaceWeekdayHoursPrivate::operator==(const QPlaceWeekdayHoursPrivate &other) const
{
    return (
            this->period == other.period
            && this->weekday == other.weekday
    );
}

/*!
    \class QPlaces::QWeekdayHours

    \inmodule QPlaces

    \brief The QWeekdayHours class represents a day of week object.
    It might contains hours information and day of the week that place is open.
*/

/*!
    Constructs an new weekday object.
*/
QPlaceWeekdayHours::QPlaceWeekdayHours()
    : d(new QPlaceWeekdayHoursPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceWeekdayHours::QPlaceWeekdayHours(const QPlaceWeekdayHours &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceWeekdayHours::~QPlaceWeekdayHours()
{
}

QPlaceWeekdayHours &QPlaceWeekdayHours::operator =(const QPlaceWeekdayHours &other) {
    d = other.d;
    return *this;
}

bool QPlaceWeekdayHours::operator==(const QPlaceWeekdayHours &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns period. Do not delete it.
*/
QPlacePeriod QPlaceWeekdayHours::period() const
{
    return d->period;
}
/*!
    Sets period.
*/
void QPlaceWeekdayHours::setPeriod(const QPlacePeriod &period)
{
    d->period = period;
}

/*!
    Returns day of the week.
*/
Qt::DayOfWeek QPlaceWeekdayHours::weekday() const
{
    return d->weekday;
}

/*!
    Sets weekday.
*/
void QPlaceWeekdayHours::setWeekday(const Qt::DayOfWeek &weekday)
{
    d->weekday = weekday;
}

