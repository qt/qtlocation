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

#include "qlandmarkcategoryremoverequest.h"
#include "qlandmarkrequests_p.h"
#include "qlandmarkcategoryid.h"
#include <QMap>


QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkCategoryRemoveRequest
    \brief The QLandmarkCategoryRemoveRequest class allows a client to asynchronously
    request that certain categories be removed from a landmark manager.

    For a QLandmarkCategoryRemoveRequest, the resultsAvailable() signal will be emitted
    when either an individual items error out (individaul errors may be retrieved
    by calling errorMap()) or an  overall operation error occurs(which may be
    retrieved by calling QLandmarkAbstractRequest::error()).

    Please see the class documentation for QLandmarkAbstractRequest for more information about
    the usage of request classes and ownership semantics.

    \inmodule QtLocation

    \ingroup landmarks-request
    \since 1.1
*/

/*!
    Constructs a category remove request with the given \a manager and \a parent.
*/
QLandmarkCategoryRemoveRequest::QLandmarkCategoryRemoveRequest(QLandmarkManager *manager, QObject *parent)
    : QLandmarkAbstractRequest(new QLandmarkCategoryRemoveRequestPrivate(manager), parent)
{
}

/*!
    Destroys the request object.
*/
QLandmarkCategoryRemoveRequest::~QLandmarkCategoryRemoveRequest()
{
}

/*!
    Returns the list of identifiers of categories which will be removed.
*/
QList<QLandmarkCategoryId> QLandmarkCategoryRemoveRequest::categoryIds() const
{
    Q_D(const QLandmarkCategoryRemoveRequest);
    QMutexLocker ml(&d->mutex);
    return d->categoryIds;
}

/*!
    Sets the list of \a categoryIds of categories which will be removed.

    \sa setCategories(), setCategoryId()
*/
void QLandmarkCategoryRemoveRequest::setCategoryIds(const QList<QLandmarkCategoryId> &categoryIds)
{
    Q_D(QLandmarkCategoryRemoveRequest);
    QMutexLocker ml(&d->mutex);
    d->categoryIds = categoryIds;
}

/*!
    Convenience function to set the \a categoryId of a single category to be removed.
    It is the equivalent of calling setCategoryIds() with a single \a categoryId
    in the ID list.

    \sa setCategory(), setCategoryIds()
*/
void QLandmarkCategoryRemoveRequest::setCategoryId(const QLandmarkCategoryId &categoryId)
{
    Q_D(QLandmarkCategoryRemoveRequest);
    QMutexLocker ml(&d->mutex);
    d->categoryIds.clear();
    d->categoryIds.append(categoryId);
}

/*!
    Convenience function to set the list of \a categories which will be removed.
    This function effectively calls setCategoryIds() with the IDs of the
    \a categories.

    \sa setCategory(), setCategoryIds()
*/
void QLandmarkCategoryRemoveRequest::setCategories(const QList<QLandmarkCategory> &categories)
{
    Q_D(QLandmarkCategoryRemoveRequest);
    QMutexLocker ml(&d->mutex);
    d->categoryIds.clear();
    for (int i=0; i < categories.count(); ++i)
        d->categoryIds.append(categories.at(i).categoryId());
}

/*!
    Convenience function that sets a single \a category to be removed.
    This function effectively calls setCategoryIds() with the ID of \a category.

    \sa setCategories(), setCategoryId()
*/
void QLandmarkCategoryRemoveRequest::setCategory(const QLandmarkCategory &category)
{
    Q_D(QLandmarkCategoryRemoveRequest);
    QMutexLocker ml(&d->mutex);
    d->categoryIds.clear();
    d->categoryIds.append(category.categoryId());
}

/*!
    Returns the mapping of input category ID list indices
    to the errors which occurred.
*/
QMap<int, QLandmarkManager::Error> QLandmarkCategoryRemoveRequest::errorMap() const
{
    Q_D(const QLandmarkCategoryRemoveRequest);
    QMutexLocker ml(&d->mutex);
    return d->errorMap;
}

#include "moc_qlandmarkcategoryremoverequest.cpp"

QTM_END_NAMESPACE


