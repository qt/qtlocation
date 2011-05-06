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

#include "qlandmarkfetchrequest.h"
#include "qlandmarkrequests_p.h"
#include "qlandmarkfilter.h"
#include "qlandmarkidfilter.h"
#include "qlandmarksortorder.h"
#include "qlandmark.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkFetchRequest
    \brief The QLandmarkFetchRequest class allows a client to asynchronously
    request a list of landmarks from a landmark manager.

    For a QLandmarkFetchRequest, the resultsAvailable() signal will be emitted when the resultant
    landmarks (which may be retrieved by calling landmarks()) are updated, as well as if
    the overall operation error (which may be retrieved by calling error()) is updated.

    Please see the class documentation for QLandmarkAbstractRequest for more information about
    the usage of request classes and ownership semantics.

    \inmodule QtLocation

    \ingroup landmarks-request
    \since 1.1
*/

/*!
    Creates a new landmark fetch request object with the given \a manager \a parent.
*/
QLandmarkFetchRequest::QLandmarkFetchRequest(QLandmarkManager *manager, QObject *parent)
    : QLandmarkAbstractRequest(new QLandmarkFetchRequestPrivate(manager), parent)
{
}

/*!
    Destroys the request object.
*/
QLandmarkFetchRequest::~QLandmarkFetchRequest()
{
}

/*!
    Returns the filter which will be used to select the landmarks.

    By default, the filter's type will be a QLandmarkFilter::DefaultFilter
    and thus match all landmarks.
*/
QLandmarkFilter QLandmarkFetchRequest::filter() const
{
    Q_D(const QLandmarkFetchRequest);
    QMutexLocker ml(&d->mutex);
    return d->filter;
}

/*!
    Sets the \a filter which will be used to select landmarks.
*/
void QLandmarkFetchRequest::setFilter(const QLandmarkFilter &filter)
{
    Q_D(QLandmarkFetchRequest);
    QMutexLocker ml(&d->mutex);
    d->filter = filter;
}

/*!
    Returns the sort ordering which is used to sort the result.  By default
    the sort order list is empty, thus no sorting will take place.
*/
QList<QLandmarkSortOrder> QLandmarkFetchRequest::sorting() const
{
    Q_D(const QLandmarkFetchRequest);
    QMutexLocker ml(&d->mutex);
    return d->sorting;
}

/*!
    Sets the sort ordering of the request to \a sorting.  This
    function will only have an effect on the results if invoked
    prior to calling \l QLandmarkAbstractRequest::start().
*/
void QLandmarkFetchRequest::setSorting(const QList<QLandmarkSortOrder> &sorting)
{
    Q_D(QLandmarkFetchRequest);
    QMutexLocker ml(&d->mutex);
    d->sorting = sorting;
}

/*!
    Convenience function which sets the sort ordering of the request to a single \a sorting.  This function
    will only have an effect on results if invoked prior to calling
    \l QLandmarkAbstractRequest::start().
*/
void QLandmarkFetchRequest::setSorting(const QLandmarkSortOrder &sorting)
{
    Q_D(QLandmarkFetchRequest);
    QMutexLocker ml(&d->mutex);
    d->sorting.clear();
    d->sorting.append(sorting);
}

/*!
    Returns the maximum number of landmarks to be returned.  By default the limit
    is -1 indicating that all landmarks matching the filter sould be retrieved.
*/
int QLandmarkFetchRequest::limit() const
{
    Q_D(const QLandmarkFetchRequest);
    QMutexLocker ml(&d->mutex);
    return d->limit;
}

/*!
    Sets the maximum number of landmarks to be returned to \a limit.

    A limit of -1 will retrieve all landmarks that match the filter.

    (A limit of 0 will retrieve no landmarks.)
 */
void QLandmarkFetchRequest::setLimit(int limit)
{
    Q_D(QLandmarkFetchRequest);
    QMutexLocker ml(&d->mutex);
    d->limit = limit;
}

/*!
    Returns the index offset for the request.  By default the offset is set to 0.
    The offset determines the first index which is retrieved, it is generally
    used in conjunction with limit() to facilitate paging.

    For example, if there are 10 landmarks in the landmark store, setting the offset
    to 2 and limit to 5 will retrieve the 3rd to 7th landmarks inclusively.  (The order
    of the landmarks is specified by the sorting field).
*/
int QLandmarkFetchRequest::offset() const
{
    Q_D(const QLandmarkFetchRequest);
    QMutexLocker ml(&d->mutex);
    return d->offset;
}

/*!
    Sets the index \a offset for the request.
*/
void QLandmarkFetchRequest::setOffset(int offset) {
    Q_D(QLandmarkFetchRequest);
    QMutexLocker ml(&d->mutex);
    d->offset = offset;
}

/*!
    Returns the list of landmarks which matched the
    filter.
*/
QList<QLandmark> QLandmarkFetchRequest::landmarks() const
{
    Q_D(const QLandmarkFetchRequest);
    QMutexLocker ml(&d->mutex);
    return d->landmarks;
}

#include "moc_qlandmarkfetchrequest.cpp"

QTM_END_NAMESPACE
