/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qlandmarkintersectionfilter.h"
#include "qlandmarkintersectionfilter_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkIntersectionFilter
    \brief The QLandmarkIntersectionFilter class provides a filter which intersects the results
            of its constituent filters.

    \inmodule QtLocation

    \ingroup landmarks-filter
    \since 1.1

    Conceptually it performs an AND operation and thus may be used to
    select landmarks which match all of it's constituent filters.
    If the intersection filter contains a proximity filter at the top level (i.e. which is not nested
    inside another filter) then the landmarks will be sorted by distance according to that filter.
    Note that a using sort order other than QLandmarkSortOrder (i.e. no sort) will override the
    sorting by distance).

    Whether an intersection filter can be comprised of
    compound filters  (i.e. union or intersection filters) is backend specific.
    Even if this is supported the performance of such a filter is likely to be poor.
*/

Q_IMPLEMENT_LANDMARKFILTER_PRIVATE(QLandmarkIntersectionFilter);

/*!
    Constructs an intersection filter.
*/
QLandmarkIntersectionFilter::QLandmarkIntersectionFilter()
        : QLandmarkFilter(new QLandmarkIntersectionFilterPrivate())
{
}

/*!
    \fn QLandmarkIntersectionFilter::QLandmarkIntersectionFilter(const QLandmarkFilter &other)
    Constructs a copy of \a other if possible, otherwise constructs a new intersection filter.
*/

/*!
    Destroys the filter.
*/
QLandmarkIntersectionFilter::~QLandmarkIntersectionFilter()
{
    // pointer deleted in superclass destructor
}

/*!
    Sets the \a filters whose criteria will be intersected.
    \sa filters()
 */
void QLandmarkIntersectionFilter::setFilters(const QList<QLandmarkFilter> &filters)
{
    Q_D(QLandmarkIntersectionFilter);
    d->filters = filters;
}

/*!
    Prepends the given \a filter to the list of intersected filters
    \sa append(), filters()
*/
void QLandmarkIntersectionFilter::prepend(const QLandmarkFilter &filter)
{
    Q_D(QLandmarkIntersectionFilter);
    d->filters.prepend(filter);
}

/*!
    Appends the given \a filter to the list of intersected filters
    \sa operator<<(), prepend(), filters()
*/
void QLandmarkIntersectionFilter::append(const QLandmarkFilter &filter)
{
    Q_D(QLandmarkIntersectionFilter);
    d->filters.append(filter);
}

/*!
    Removes the given \a filter from the list of intersected filters.
    \sa filters(), append(), prepend()
*/
void QLandmarkIntersectionFilter::remove(const QLandmarkFilter &filter)
{
    Q_D(QLandmarkIntersectionFilter);
    d->filters.removeAll(filter);
}

/*!
    Removes all filters from the list of intersected filters.
*/
void QLandmarkIntersectionFilter::clear()
{
    Q_D(QLandmarkIntersectionFilter);
    d->filters.clear();
}

/*!
    Appends the given \a filter to the list of intersected filters
    \sa append()
 */
QLandmarkIntersectionFilter& QLandmarkIntersectionFilter::operator<<(const QLandmarkFilter &filter)
{
    Q_D(QLandmarkIntersectionFilter);
    d->filters.append(filter);
    return *this;
}

/*!
    Returns the list of filters which form the intersection filter
    \sa setFilters(), prepend(), append(), remove()
 */
QList<QLandmarkFilter> QLandmarkIntersectionFilter::filters() const
{
    Q_D(const QLandmarkIntersectionFilter);
    return d->filters;
}

/*******************************************************************************
*******************************************************************************/

QLandmarkIntersectionFilterPrivate::QLandmarkIntersectionFilterPrivate()
{
    type = QLandmarkFilter::IntersectionFilter;
}

QLandmarkIntersectionFilterPrivate::QLandmarkIntersectionFilterPrivate(const QLandmarkIntersectionFilterPrivate &other)
        : QLandmarkFilterPrivate(other),
        filters(other.filters) {}

QLandmarkIntersectionFilterPrivate::~QLandmarkIntersectionFilterPrivate() {}

QTM_END_NAMESPACE
