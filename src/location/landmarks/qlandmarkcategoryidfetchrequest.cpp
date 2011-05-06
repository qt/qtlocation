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

#include "qlandmarkcategoryidfetchrequest.h"
#include "qlandmarkrequests_p.h"
#include "qlandmarkcategoryid.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkCategoryIdFetchRequest
    \brief The QLandmarkCategoryIdFetchRequest class allows a client to asynchronously
    request a list of category identifiers from a landmark manager.

    For a QLandmarkCategoryIdFetchRequest, the resultsAvailable() signal will be emitted
    as resultant category identifiers are found (these are retrievable by calling categoryIds()),
     as well as if an overall operation error occurred(which may be retrieved by calling
     QLandmarkAbstractRequest::error()).

    Please see the class documentation for QLandmarkAbstractRequest for more information about
    the usage of request classes and ownership semantics.

    \inmodule QtLocation
    \since 1.1

    \ingroup landmarks-request
*/

/*!
    Creates a new category identifier fetch request object with the given \a manager
    aand \a parent.
*/
QLandmarkCategoryIdFetchRequest::QLandmarkCategoryIdFetchRequest(QLandmarkManager * manager, QObject *parent)
    : QLandmarkAbstractRequest(new QLandmarkCategoryIdFetchRequestPrivate(manager), parent)
{
}

/*!
    Destroys the request object.
*/
QLandmarkCategoryIdFetchRequest::~QLandmarkCategoryIdFetchRequest()
{
}

/*!
    Returns the list of category identifiers that have been found during the
    request.
*/
QList<QLandmarkCategoryId> QLandmarkCategoryIdFetchRequest::categoryIds() const
{
    Q_D(const QLandmarkCategoryIdFetchRequest);
    QMutexLocker ml(&d->mutex);
    return d->categoryIds;
}

/*!
    Returns the sorting of the categories.
    By default the sorting is case insensitive and in ascending order
    according to the category name.
*/
QLandmarkNameSort QLandmarkCategoryIdFetchRequest::sorting() const
{
    Q_D(const QLandmarkCategoryIdFetchRequest);
    QMutexLocker ml(&d->mutex);
    return d->sorting;
}

/*!
    Sets \a nameSort to specify the sorting of the returned category ids.
*/
void QLandmarkCategoryIdFetchRequest::setSorting(const QLandmarkNameSort &nameSort)
{
    Q_D(QLandmarkCategoryIdFetchRequest);
    QMutexLocker ml(&d->mutex);
    d->sorting = nameSort;
}

/*!
    Returns the maximum number of category IDs to be returned.  By default the limit is set to -1
    indicating that IDs of all categories should be retrieved.
*/
int QLandmarkCategoryIdFetchRequest::limit() const
{
    Q_D(const QLandmarkCategoryIdFetchRequest);
    QMutexLocker ml(&d->mutex);
    return d->limit;
}

/*!
    Sets the maximum number of category IDs to be returned to \a limit.

    A limit of -1 indicates that IDs of all categories are to be retrieved.

    (A limit of 0 will retrieve no categories.)
 */
void QLandmarkCategoryIdFetchRequest::setLimit(int limit)
{
    Q_D(QLandmarkCategoryIdFetchRequest);
    QMutexLocker ml(&d->mutex);
    d->limit = limit;
}

/*!
    Returns the index offset for the request.  By default the offset is set to 0.
    The offset determines the first index which is retrieved. it is generally
    used in conjunction with limit() to facilitate paging.

    For example, if there are 10 categories in the landmark store, setting the offset
    to 2 and limit to 5 will retrieve the 3rd to 7th category ids inclusively.  (The order
    of category ids is specified by the sorting field).
*/
int QLandmarkCategoryIdFetchRequest::offset() const
{
    Q_D(const QLandmarkCategoryIdFetchRequest);
    QMutexLocker ml(&d->mutex);
    return d->offset;
}

/*!
    Sets the index \a offset for the request.
*/
void QLandmarkCategoryIdFetchRequest::setOffset(int offset) {
    Q_D(QLandmarkCategoryIdFetchRequest);
    QMutexLocker ml(&d->mutex);
    d->offset = offset;
}

#include "moc_qlandmarkcategoryidfetchrequest.cpp"

QTM_END_NAMESPACE











