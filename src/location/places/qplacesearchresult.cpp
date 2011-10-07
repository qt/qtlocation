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

#include "qplacesearchresult.h"
#include "qplacesearchresult_p.h"

QT_USE_NAMESPACE

QPlaceSearchResultPrivate::QPlaceSearchResultPrivate()
:   QSharedData(), distance(0), type(QPlaceSearchResult::UnknownSearchResult)
{
}

QPlaceSearchResultPrivate::QPlaceSearchResultPrivate(const QPlaceSearchResultPrivate &other)
    : QSharedData()
{
    distance = other.distance;
    type = other.type;
    place = other.place;
    correction = other.correction;
}

QPlaceSearchResultPrivate::~QPlaceSearchResultPrivate()
{
}

bool QPlaceSearchResultPrivate::operator==(const QPlaceSearchResultPrivate &other) const
{
    return distance == other.distance &&
           type == other.type &&
           place == other.place &&
           correction == other.correction;
}

/*!
    \class QPlaceSearchResult

    \inmodule QPlaces

    \brief The QPlaceSearchResult class represents a search result object.

    \ingroup places-main

    Each QPlaceSearchResult represents a place with a number of attributes
    such as distance, relevance, etc.
*/

/*!
    Default constructor. Constructs an new search result.
*/
QPlaceSearchResult::QPlaceSearchResult()
    : d(new QPlaceSearchResultPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceSearchResult::QPlaceSearchResult(const QPlaceSearchResult &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceSearchResult::~QPlaceSearchResult()
{
}

QPlaceSearchResult &QPlaceSearchResult::operator =(const QPlaceSearchResult &other) {
    d = other.d;
    return *this;
}

bool QPlaceSearchResult::operator==(const QPlaceSearchResult &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns the distance.
*/
qreal QPlaceSearchResult::distance() const
{
    return d->distance;
}

/*!
    Sets the \a distance.
*/
void QPlaceSearchResult::setDistance(qreal distance)
{
    d->distance = distance;
}

/*!
    Returns the place.
*/
QPlaceSearchResult::SearchResultType QPlaceSearchResult::type() const
{
    return d->type;
}

/*!
    Sets the \a place.
*/
void QPlaceSearchResult::setType(QPlaceSearchResult::SearchResultType type)
{
    d->type = type;
}

/*!
    Returns the place.
*/
QPlace QPlaceSearchResult::place() const
{
    return d->place;
}

/*!
    Sets the \a place.
*/
void QPlaceSearchResult::setPlace(const QPlace &place)
{
    d->place = place;
}

/*!
    Returns the suggested search term correction.
*/
QString QPlaceSearchResult::correction() const
{
    return d->correction;
}

/*!
    Sets the "did you mean" \a string.
*/
void QPlaceSearchResult::setCorrection(const QString &correction)
{
    d->correction = correction;
}
