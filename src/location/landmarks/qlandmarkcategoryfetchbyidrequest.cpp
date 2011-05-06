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

#include "qlandmarkcategoryfetchbyidrequest.h"
#include "qlandmarkrequests_p.h"
#include "qlandmarkcategory.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkCategoryFetchByIdRequest
    \brief The QLandmarkCategoryFetchByIdRequest class allows a client to asynchronously
    request a list of categories by ID from a landmark manager.

    The categories fetched by the manager should have a one-to-one correspondence to the
    IDs passed into this class.  That is, the nth category in the returned list should
    have an ID which is equal to the nth ID in the list of IDs.  Any invalid ID should
    correspond to an empty QLandmarkCategory.

    For a QLandmarkCategoryFetchByIdRequest, the resultsAvailable() signal will be emitted
    as resultant categories are found (these are retrievable by calling categories()),
    whenever individual items error out(individual errors may be retrieved by calling errorMap()),
    as well as if an overall operation error occurred(which may be retrieved by calling
    QLandmarkAbstractRequest::error()).

     Please see the class documentation for QLandmarkAbstractRequest for more information
     about he usage of request classes and ownership semantics.


    \inmodule QtLocation

    \ingroup landmarks-request
    \since 1.2
*/

/*!
    Creates a new a request object with the given \a manager and \a parent.
*/
QLandmarkCategoryFetchByIdRequest::QLandmarkCategoryFetchByIdRequest(QLandmarkManager *manager, QObject *parent)
    :QLandmarkAbstractRequest(new QLandmarkCategoryFetchByIdRequestPrivate(manager), parent)
{
}

/*!
    Destroys the request object.
*/
QLandmarkCategoryFetchByIdRequest::~QLandmarkCategoryFetchByIdRequest()
{
}

/*!
    Returns a list of identifiers of categories which are to be retrieved by this request.
*/
QList<QLandmarkCategoryId> QLandmarkCategoryFetchByIdRequest::categoryIds() const
{
    Q_D(const QLandmarkCategoryFetchByIdRequest);
    QMutexLocker ml(&d->mutex);
    return d->categoryIds;
}

/*!
    Sets this request to retrieve a list of categories identified by \a categoryIds.
*/
void QLandmarkCategoryFetchByIdRequest::setCategoryIds(const QList<QLandmarkCategoryId> &categoryIds)
{
    Q_D(QLandmarkCategoryFetchByIdRequest);
    QMutexLocker ml(&d->mutex);
    d->categoryIds = categoryIds;
}

/*!
    Convenience function that sets a single category, specified by \a categoryId,
    to be retrieved by this request.
*/
void QLandmarkCategoryFetchByIdRequest::setCategoryId(const QLandmarkCategoryId &categoryId)
{
    Q_D(QLandmarkCategoryFetchByIdRequest);
    QMutexLocker ml(&d->mutex);
    d->categoryIds.clear();
    d->categoryIds.append(categoryId);
}

/*!
    Returns the list of categories fetched by this request
*/
QList<QLandmarkCategory> QLandmarkCategoryFetchByIdRequest::categories() const
{
    Q_D(const QLandmarkCategoryFetchByIdRequest);
    QMutexLocker ml(&d->mutex);
    return d->categories;
}

/*!
    Returns the mapping of input category id list indices
    to the errors which occurred.
*/
QMap<int, QLandmarkManager::Error> QLandmarkCategoryFetchByIdRequest::errorMap() const
{
    Q_D(const QLandmarkCategoryFetchByIdRequest);
    QMutexLocker ml(&d->mutex);
    return d->errorMap;
}

#include "moc_qlandmarkcategoryfetchbyidrequest.cpp"

QTM_END_NAMESPACE
