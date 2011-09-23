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
    : QSharedData(),
      relevance(0),
      distance(0),
      heading(0),
      type(QPlaceSearchResult::UnknownSearchResult)
{
}

QPlaceSearchResultPrivate::QPlaceSearchResultPrivate(const QPlaceSearchResultPrivate &other)
    : QSharedData()
{
    this->additionalData = other.additionalData;
    this->relevance = other.relevance;
    this->distance = other.distance;
    this->heading = other.heading;
    this->type = other.type;
    this->place = other.place;
    this->dymString = other.dymString;
}

QPlaceSearchResultPrivate::~QPlaceSearchResultPrivate()
{
}

bool QPlaceSearchResultPrivate::operator==(const QPlaceSearchResultPrivate &other) const
{
    return (
            this->additionalData == other.additionalData
            && this->relevance == other.relevance
            && this->distance == other.distance
            && this->heading == other.heading
            && this->type == other.type
            && this->place == other.place
            && this->dymString == other.dymString
    );
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
    Returns relevance.
*/
qreal QPlaceSearchResult::relevance() const
{
    return d->relevance;
}

/*!
    Sets the \a relevance.
*/
void QPlaceSearchResult::setRelevance(const qreal &relevance)
{
    d->relevance = relevance;
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
void QPlaceSearchResult::setDistance(const qreal &distance)
{
    d->distance = distance;
}

/*!
    Returns heading.
*/
qreal QPlaceSearchResult::heading() const
{
    return d->heading;
}

/*!
    Sets the \a heading.
*/
void QPlaceSearchResult::setHeading(const qreal &heading)
{
    d->heading = heading;
}

/*!
    Returns additional data.
*/
QVariantHash QPlaceSearchResult::additionalData() const
{
    return d->additionalData;
}

/*!
    Sets additional \a data.
*/
void QPlaceSearchResult::setAdditionalData(const QVariantHash &data)
{
    d->additionalData = data;
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
void QPlaceSearchResult::setType(const QPlaceSearchResult::SearchResultType &type)
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
    Returns the "did you mean" string.
*/
QString QPlaceSearchResult::didYouMeanSuggestion() const
{
    return d->dymString;
}

/*!
    Sets the "did you mean" \a string.
*/
void QPlaceSearchResult::setDidYouMeanSuggestion(const QString &dymString)
{
    d->dymString = dymString;
}
