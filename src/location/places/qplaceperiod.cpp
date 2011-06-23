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

#include "qplaceperiod.h"
#include "qplaceperiod_p.h"

QTM_USE_NAMESPACE

QPlacePeriodPrivate::QPlacePeriodPrivate()
    : QSharedData()
{
    QDate date;
    QTime time;
    begin.setDate(date);
    begin.setTime(time);
    end.setDate(date);
    end.setTime(time);
}

QPlacePeriodPrivate::QPlacePeriodPrivate(const QPlacePeriodPrivate &other)
    : QSharedData()
{
    this->begin = other.begin;
    this->end = other.end;
}

QPlacePeriodPrivate::~QPlacePeriodPrivate()
{
}

bool QPlacePeriodPrivate::operator==(const QPlacePeriodPrivate &other) const
{
    return (
            this->begin == other.begin
            && this->end == other.end
    );
}

/*!
    \class QPlacePeriod

    \inmodule QPlaces

    \brief The QPlacePeriod class represents a time period object.
    It might contains just hours information but it might contains also
    date. If some field is unasaignd it will return -1.
*/

/*!
    Constructs an new period object.
*/
QPlacePeriod::QPlacePeriod()
    : d(new QPlacePeriodPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlacePeriod::QPlacePeriod(const QPlacePeriod &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlacePeriod::~QPlacePeriod()
{
}

QPlacePeriod &QPlacePeriod::operator =(const QPlacePeriod &other) {
    d = other.d;
    return *this;
}

bool QPlacePeriod::operator==(const QPlacePeriod &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns sart date.  Null returned if not set.
    If year was not set, year value will be default(4713 BC)
*/
QDate QPlacePeriod::startDate() const
{
    return d->begin.date();
}

/*!
    Sets th start \a date
*/
void QPlacePeriod::setStartDate(const QDate &date)
{
    d->begin.setDate(date);
}

/*!
    Returns the start time.  Null returned if not set.
*/
QTime QPlacePeriod::startTime() const
{
    return d->begin.time();
}

/*!
    Sets the start time.
*/
void QPlacePeriod::setStartTime(const QTime &time)
{
    d->begin.setTime(time);
}

/*!
    Returns the end date.  Null is returned if not set.
    If the year was not set, year value will be default (4713 BC)
*/
QDate QPlacePeriod::endDate() const
{
    return d->end.date();
}

/*!
    Sets the end \a date.
*/
void QPlacePeriod::setEndDate(const QDate &date)
{
    d->end.setDate(date);
}

/*!
    Returns end time.  Null returned if not set.
*/
QTime QPlacePeriod::endTime() const
{
    return d->end.time();
}

/*!
    Sets the end \a time
*/
void QPlacePeriod::setEndTime(const QTime &time)
{
    d->end.setTime(time);
}
