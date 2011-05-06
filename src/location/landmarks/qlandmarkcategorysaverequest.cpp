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

#include "qlandmarkcategorysaverequest.h"
#include "qlandmarkrequests_p.h"
#include "qlandmarkcategory.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkCategorySaveRequest
    \brief The QLandmarkCategorySaveRequest class allows a client to asynchronously
    request that certain categories be saved by a landmark manager.

    For a QLandmarkCategorySaveRequest, the resultsAvailable() signal will be emitted
    when either an individual items error out (individaul errors may be retrieved
    by calling errorMap()), or an overall operation error occurs(which may be
    retrieved by calling error()).

    Please see the class documentation for QLandmarkAbstractRequest for more information about
    the usage of request classes and ownership semantics.

    \inmodule QtLocation

    \ingroup landmarks-request
    \since 1.1
*/

/*!
    Constructs a category save request with the given \a manager and \a parent.
*/
QLandmarkCategorySaveRequest::QLandmarkCategorySaveRequest(QLandmarkManager *manager, QObject *parent)
    : QLandmarkAbstractRequest(new QLandmarkCategorySaveRequestPrivate(manager), parent)
{
}

/*!
    Destroys the request object.
*/
QLandmarkCategorySaveRequest::~QLandmarkCategorySaveRequest()
{
}

/*!
    Returns the list of categories to be saved.  If called after the save operation has
    finished, any new categories saved will have had their QLandmarkCategoryId set.
    (Categories which were updated or failed to save will remain unchanged, use the
    errorMap() function to determine which particular categories failed to save.)
*/
QList<QLandmarkCategory> QLandmarkCategorySaveRequest::categories() const
{
    Q_D(const QLandmarkCategorySaveRequest);
    QMutexLocker ml(&d->mutex);
   return d->categories;
}

/*!
    Sets the list of \a categories to be saved.
    \sa setCategory()
*/
void QLandmarkCategorySaveRequest::setCategories(const QList<QLandmarkCategory> &categories)
{
    Q_D(QLandmarkCategorySaveRequest);
    QMutexLocker ml(&d->mutex);
    d->categories = categories;
}

/*!
    Convenience function to set a single \a category to be saved.  This function
    is the equivalent of calling setCategories() with a single \a category.

    \sa setCategories()
*/
void QLandmarkCategorySaveRequest::setCategory(const QLandmarkCategory& category)
{
    Q_D(QLandmarkCategorySaveRequest);
    QMutexLocker ml(&d->mutex);
    d->categories.clear();
    d->categories.append(category);
}

/*!
    Returns the mapping of the input category list indices to the
    errors which occurred.
*/
QMap<int, QLandmarkManager::Error> QLandmarkCategorySaveRequest::errorMap() const
{
    Q_D(const QLandmarkCategorySaveRequest);
    QMutexLocker ml(&d->mutex);
    return d->errorMap;
}

#include "moc_qlandmarkcategorysaverequest.cpp"

QTM_END_NAMESPACE


