/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qplacesearchresult.h"
#include "qplacesearchresult_p.h"
#include <QtCore/qnumeric.h>

QT_USE_NAMESPACE

QPlaceSearchResultPrivate::QPlaceSearchResultPrivate()
:   QSharedData(), distance(qQNaN()), type(QPlaceSearchResult::UnknownSearchResult),
    sponsored(false)
{
}

QPlaceSearchResultPrivate::QPlaceSearchResultPrivate(const QPlaceSearchResultPrivate &other)
    : QSharedData()
{
    distance = other.distance;
    type = other.type;
    place = other.place;
    correction = other.correction;
    sponsored = other.sponsored;
}

QPlaceSearchResultPrivate::~QPlaceSearchResultPrivate()
{
}

bool QPlaceSearchResultPrivate::operator==(const QPlaceSearchResultPrivate &other) const
{
    return distance == other.distance &&
           type == other.type &&
           place == other.place &&
           correction == other.correction &&
           sponsored == other.sponsored;
}

/*!
    \class QPlaceSearchResult
    \inmodule QtLocation
    \ingroup QtLocation-places
    \ingroup QtLocation-places-data
    \since QtLocation 5.0

    \brief The QPlaceSearchResult class represents a search result.

    There are two types of search results.  The first is a
    \l {QPlaceSearchResult::PlaceResult} {place result}, which contains
    a place that matched the search request, but also metadata about the place
    such as the distance from the search center of a search request and whether the result is a
    sponsored or \l {http://en.wikipedia.org/wiki/Organic_search}{organic} search result.

    The other type is a \l {QPlaceSearchResult::CorrectionResult}{correction}, which
    contains an alternative search term that may better reflect the
    user's intended query.

*/

/*!
    \enum QPlaceSearchResult::SearchResultType
    Defines the type of search result
    \value PlaceResult The search result contains a place.
    \value CorrectionResult The search result contains a search term correction.
    \value UnknownSearchResult The contents of the search result are unknown.
*/

/*!
    Constructs a new search result.
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
    Destroys the search result.
*/
QPlaceSearchResult::~QPlaceSearchResult()
{
}

/*!
    Assigns \a other to this search result and returns a reference to this
    search result.
*/
QPlaceSearchResult &QPlaceSearchResult::operator =(const QPlaceSearchResult &other) {
    d = other.d;
    return *this;
}

/*!
    Returns true if \a other is equal to this search result, otherwise
    returns false.
*/
bool QPlaceSearchResult::operator==(const QPlaceSearchResult &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    \fn bool QPlaceSearchResult::operator!=(const QPlaceSearchResult &other) const
    Returns true if \a other not equal to this search result, otherwise
    returns false.
*/

/*!
    Returns the distance of the place to the search center.  This field
    is only valid when the search result type is QPlaceSearchResult::PlaceResult,
    and where the search request contained a search center.  Otherwise,
    the distance is NaN indicating an undefined distance.  The default value
    for distance is NaN.
*/
qreal QPlaceSearchResult::distance() const
{
    return d->distance;
}

/*!
    Set the \a distance of the search result's place from a search center.
*/
void QPlaceSearchResult::setDistance(qreal distance)
{
    d->distance = distance;
}

/*!
    Returns the type of the search result.
*/
QPlaceSearchResult::SearchResultType QPlaceSearchResult::type() const
{
    return d->type;
}

/*!
    Sets the \a type of the search result.
*/
void QPlaceSearchResult::setType(QPlaceSearchResult::SearchResultType type)
{
    d->type = type;
}

/*!
    Returns the place of the search result.  This field is only valid when the search result
    type is QPlaceSearchResult::PlaceResult.
*/
QPlace QPlaceSearchResult::place() const
{
    return d->place;
}

/*!
    Sets the \a place that this search result refers to.
*/
void QPlaceSearchResult::setPlace(const QPlace &place)
{
    d->place = place;
}

/*!
    Returns the correction term that this particular search result represents.
    This field is only valid when the search result type is QPlaceSearchResult::CorrectionResult.
*/
QString QPlaceSearchResult::correction() const
{
    return d->correction;
}

/*!
    Sets the \a correction term of the search result.
*/
void QPlaceSearchResult::setCorrection(const QString &correction)
{
    d->correction = correction;
}

/*!
    Returns true if the search result represents a sponsored result.

    \sa setSponsored()
*/
bool QPlaceSearchResult::isSponsored() const
{
    return d->sponsored;
}

/*!
    Sets whether the search result represents a \a sponsored result or not.

    \sa isSponsored()
*/
void QPlaceSearchResult::setSponsored(bool sponsored)
{
    d->sponsored = sponsored;
}
