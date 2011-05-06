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

#include "qlandmarkfilter.h"
#include "qlandmarkfilter_p.h"

#include "qlandmarkintersectionfilter.h"
#include "qlandmarkunionfilter.h"

#include "qlandmarkcategoryid.h"
#include "qlandmarkid.h"
#include "qgeocoordinate.h"

#if !defined(Q_CC_MWERKS)
template<> QTM_PREPEND_NAMESPACE(QLandmarkFilterPrivate) *QSharedDataPointer<QTM_PREPEND_NAMESPACE(QLandmarkFilterPrivate)>::clone()
{
    return d->clone();
}
#endif

QTM_BEGIN_NAMESPACE

QLandmarkFilterPrivate::QLandmarkFilterPrivate()
        : QSharedData(),
          type(QLandmarkFilter::DefaultFilter)
{
}

QLandmarkFilterPrivate::QLandmarkFilterPrivate(const QLandmarkFilterPrivate &other)
        : QSharedData(),
          type(other.type)
{
}

QLandmarkFilterPrivate::~QLandmarkFilterPrivate()
{
}

/*!
    \class QLandmarkFilter
    \brief The QLandmarkFilter class serves as the base class for all filter classes.
    Also serves as the default filter which retrieves all landmarks.

    \inmodule QtLocation

    \ingroup landmarks-filter
    \since 1.1

    This class is used as a parameter to search for landmarks, it allows selection of
    landmarks which meet certain criteria.  It is used with QLandmarkManager
    for synchronous searches or the appropriate request classes for asynchronous
    searches.

    The QLandmarkFilter is primarily intended to serve as the base class
    for all filter types.  However it also has the type of a QLandmark::DefaultFilter,
    and may thus be used as a filter to match all landmarks.

*/

/*!
    \enum QLandmarkFilter::FilterType
    Describes the type of the filter
    \value InvalidFilter An invalid filter which matches nothing
    \value DefaultFilter A filter which matches all landmarks.
    \value IntersectionFilter A filter which performs an AND operation with its
                                      constituent filters
    \value UnionFilter A filter which performs an OR operation with its constiuent filters.
    \value NameFilter A filter which matches landmarks based on name
    \value ProximityFilter A filter which matches landmarks within a certain range of a given
           coordinate.
    \value CategoryFilter A filter which matches landmarks that belong to a given category
    \value BoxFilter A filter which matches landmarks within a given bounding box.
    \value AttributeFilter A filter which matches landmarks based on generic attributes.
    \value LandmarkIdFilter A filter which matches landmarks based on a list of landmark ids.
*/

/*!
    \enum QLandmarkFilter::MatchFlag
    Describes the semantics of matching followed by the filter.
    \value MatchExactly Performs QVariant-based matching. (For the Symbian and S60 platforms MatchExactly has the same semantics as MatchFixedString)
    \value MatchContains The search term is contained in the item. (For the Symbian and S60 platforms, MatchContains is not supported)
    \value MatchStartsWith The search term matches the start of the item.
    \value MatchEndsWith The search term matches the end of the item.
    \value MatchFixedString  Performs string-based matching. String-based comparisons are case-insensitive unless the MatchCaseSensitive flag is also specified.
    \value MatchCaseSensitive The search is case sensitive.
*/

/*!
    Constructs an default landmark filter.
*/
QLandmarkFilter::QLandmarkFilter()
        : d_ptr(new QLandmarkFilterPrivate)
{
}
/*!
    \internal
*/
QLandmarkFilter::QLandmarkFilter(QLandmarkFilterPrivate *dd)
        : d_ptr(dd)
{
}

/*!
    Constructs a copy of \a other.
*/
QLandmarkFilter::QLandmarkFilter(const QLandmarkFilter &other)
        : d_ptr(other.d_ptr)
{
}

/*!
    Assigns \a other to this filter and returns a reference to this filter.
*/
QLandmarkFilter &QLandmarkFilter::operator=(const QLandmarkFilter & other)
{
    if (this != &other)
        d_ptr = (other.d_ptr);
    return *this;
}


/*!
    Destroys the filter.
*/
QLandmarkFilter::~QLandmarkFilter()
{
}

/*!
    Returns the type of the filter.
*/
QLandmarkFilter::FilterType QLandmarkFilter::type() const
{
    return d_ptr->type;
}

/*!
    \fn QLandmarkFilter::operator!=(const QLandmarkFilter &other) const
    Returns true if this filter is not identical to \a other.

    \sa operator==()
*/

/*!
    Returns true if the filter has the same type and criteria as \a other.
    \sa operator!=()
*/
bool QLandmarkFilter::operator==(const QLandmarkFilter& other) const
{
    /* Different types can't be equal */
    if (other.type() != type())
        return false;

    /* Otherwise, use the virtual op == */
    return d_ptr->compare(other.d_ptr);
}

/*!
 \relates QLandmarkFilter
 Returns a filter which is the intersection of the \a left and \a right filters
 \sa QLandmarkIntersectionFilter
 */
const QLandmarkFilter operator&(const QLandmarkFilter &left, const QLandmarkFilter &right)
{
    /* TODO implement better handling when left or right is an intersection filter */

    /* usual fallback case */

    QLandmarkIntersectionFilter nif;
    nif << left << right;
    return nif;
}

/*!
 \relates QLandmarkFilter
 Returns a filter which is the union of the \a left and \a right filters
 \sa QLandmarkUnionFilter
 */
const QLandmarkFilter operator|(const QLandmarkFilter &left, const QLandmarkFilter &right)
{
    /* TODO implement better handling when left or right is a union filter */
    /* usual fallback case */

    QLandmarkUnionFilter nuf;
    nuf << left << right;
    return nuf;
}

QTM_END_NAMESPACE
