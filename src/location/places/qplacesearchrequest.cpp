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

#include "qplacesearchrequest.h"
#include "qgeocoordinate.h"

QT_BEGIN_NAMESPACE

class QPlaceSearchRequestPrivate : public QSharedData
{
public:
    QPlaceSearchRequestPrivate();
    QPlaceSearchRequestPrivate(const QPlaceSearchRequestPrivate &other);

    ~QPlaceSearchRequestPrivate();

    bool operator==(const QPlaceSearchRequestPrivate &other) const;

    QString searchTerm;
    QList<QPlaceCategory> categories;
    QGeoBoundingArea  *searchArea;
    int dymNumber;
    QPlaceManager::VisibilityScopes scope;
};

QT_END_NAMESPACE

QT_USE_NAMESPACE

QPlaceSearchRequestPrivate::QPlaceSearchRequestPrivate()
    : QSharedData(),
      dymNumber(0),
      searchArea(0),
      scope(QPlaceManager::NoScope)
{
}

QPlaceSearchRequestPrivate::QPlaceSearchRequestPrivate(const QPlaceSearchRequestPrivate &other)
    : QSharedData(),
       dymNumber(0)
{
    this->searchTerm = other.searchTerm;
    this->categories = other.categories;
    if (other.searchArea)
        this->searchArea = other.searchArea->clone();
    this->dymNumber = other.dymNumber;
    this->scope = other.scope;
}

QPlaceSearchRequestPrivate::~QPlaceSearchRequestPrivate()
{
    delete searchArea;
    searchArea = 0;
}

bool QPlaceSearchRequestPrivate::operator==(const QPlaceSearchRequestPrivate &other) const
{
    bool searchAreaMatch = false;
    if ((this->searchArea == 0) && (other.searchArea == 0)) {
        searchAreaMatch = true;
    } else if (this->searchArea && other.searchArea) {
        if ((*this->searchArea) == (*other.searchArea))
            searchAreaMatch = true;
        else
            searchAreaMatch = false;
    } else {
        searchAreaMatch = false;
    }

    return (
            this->searchTerm == other.searchTerm
            && this->categories == other.categories
            && this->dymNumber == other.dymNumber
            && searchAreaMatch
            && this->scope == other.scope
    );
}

/*!
    \class QPlaceSearchRequest

    \inmodule Location

    \brief The QPlaceSearchRequest class represents the query parameters
    of a search request.

    \ingroup places-main

    The QPlaceSearchRequest class represents a query parameters object. Each
    QPlaceSearchRequest cointans search query parameters like search term.
*/

/*!
    \enum QPlaceSearchRequest::RelevanceHint

    Defines hints to help rank place results.
    \value DistanceHint
        Distance to the user's current location is relevant.  This is only useful
        if a circular bounding area is used in the query.
    \value RatingHint
        The rating of the place is relevant to the user.
    \value AlphabetHint
        Alphabetic ordering of places is relevant to the user.
*/

/*!
    Default constructor. Constructs an new request object.
*/
QPlaceSearchRequest::QPlaceSearchRequest()
    : QPlaceRequest(),
      d(new QPlaceSearchRequestPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceSearchRequest::QPlaceSearchRequest(const QPlaceSearchRequest &other)
    : QPlaceRequest(other),
      d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceSearchRequest::~QPlaceSearchRequest()
{
}

QPlaceSearchRequest &QPlaceSearchRequest::operator =(const QPlaceSearchRequest &other) {
    this->QPlaceRequest::operator =(other);
    d = other.d;
    return *this;
}

bool QPlaceSearchRequest::operator==(const QPlaceSearchRequest &other) const
{
    return (this->QPlaceRequest::operator ==(other)
            && (*(d.constData()) == *(other.d.constData())));
}


/*!
    Returns the search term.
*/
QString QPlaceSearchRequest::searchTerm() const
{
    return d->searchTerm;
}

/*!
    Sets the search \a term.
*/
void QPlaceSearchRequest::setSearchTerm(const QString &term)
{
    d->searchTerm = term;
}

/*!
    Return the categories to be used in the search request.
    Places need only to belong to one of the categories
    to be considered a match by the request.
*/
QList<QPlaceCategory> QPlaceSearchRequest::categories() const
{
    return d->categories;
}

/*!
    Sets the search request to search by a single \a category
*/
void QPlaceSearchRequest::setCategory(const QPlaceCategory &category)
{
    d->categories.clear();

    if (!category.categoryId().isEmpty())
        d->categories.append(category);
}

/*!
    Returns search area.  The default search area is a null pointer.
*/
QGeoBoundingArea *QPlaceSearchRequest::searchArea() const
{
    return d->searchArea;
}

/*!
    Sets the search request to search within the given \a area.  Ownership of the \a area is
    transferred to the request  who is responsible for pointer deletion.  If a new \a area
    is being assigned, the old area is deleted.
*/
void QPlaceSearchRequest::setSearchArea(QGeoBoundingArea *area)
{
    if (d->searchArea != area)
        delete d->searchArea;

    d->searchArea = area;
}

/*!
    Returns maximum number of "did you mean" suggestions returned by search request.
*/
int QPlaceSearchRequest::didYouMeanSuggestionNumber() const
{
    return d->dymNumber;
}

/*!
    Sets maximum \a number of "did you mean" suggestions returned by search request.
*/
void QPlaceSearchRequest::setDidYouMeanSuggestionNumber(const int &number)
{
    d->dymNumber = number;
}

/*!
    Returns the visibility scope used when searching for places.  The default value
    is QPlaceManager::NoScope meaning no explicit scope has been assigned.
    It is up to the manager implementation to decide what scope it searches
    by default.
*/
QPlaceManager::VisibilityScopes QPlaceSearchRequest::visibilityScope() const
{
    return d->scope;
}

/*!
    Sets the \a scope used when searching for places.
*/
void QPlaceSearchRequest::setVisibilityScope(QPlaceManager::VisibilityScopes scope)
{
    d->scope = scope;
}

/*!
    Clears the parameters of the search request.
*/
void QPlaceSearchRequest::clear()
{
    QPlaceRequest::clear();
    d->searchTerm.clear();
    d->categories.clear();
    if (d->searchArea)
        delete d->searchArea;
    d->searchArea = 0;
    d->dymNumber = 0;
    d->scope = QPlaceManager::NoScope;
}
