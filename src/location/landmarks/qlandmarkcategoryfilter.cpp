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

#include "qlandmarkcategoryfilter.h"
#include "qlandmarkcategoryfilter_p.h"
#include "qlandmarkcategoryid.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkCategoryFilter
    \brief The QLandmarkCategoryFilter class is used to search for landmarks that
    belong to a certain category.

    \inmodule QtLocation
    \since 1.1

    \ingroup landmarks-filter
*/

Q_IMPLEMENT_LANDMARKFILTER_PRIVATE(QLandmarkCategoryFilter)

/*!
    Creates a filter that searches for landmarks that belong to the category
    identified by \a categoryId.
*/
QLandmarkCategoryFilter::QLandmarkCategoryFilter(const QLandmarkCategoryId &categoryId)
        : QLandmarkFilter(new QLandmarkCategoryFilterPrivate(categoryId))
{
}

/*!
    \fn QLandmarkCategoryFilter::QLandmarkCategoryFilter(const QLandmarkFilter &other)
    Constructs a copy of \a other if possible, otherwise constructs a new category filter.
*/

/*!
    Destroys the filter.
*/
QLandmarkCategoryFilter::~QLandmarkCategoryFilter()
{
    // pointer deleted in superclass destructor
}

/*!
    Returns the category identifier that this filter matches by.
*/
QLandmarkCategoryId QLandmarkCategoryFilter::categoryId() const
{
    Q_D(const QLandmarkCategoryFilter);
    return d->id;
}

/*!
    Sets the category identifier that this filter matches by
    to \a categoryId.
*/
void QLandmarkCategoryFilter::setCategoryId(const QLandmarkCategoryId &categoryId)
{
    Q_D(QLandmarkCategoryFilter);
    d->id = categoryId;
}

/*!
    Convenience function to set the \a category that this filter matches by.
    It is equivalent to calling setCategory(category.categoryId()).
*/
void QLandmarkCategoryFilter::setCategory(const QLandmarkCategory &category)
{
    Q_D(QLandmarkCategoryFilter);
    d->id = category.categoryId();
}

/*******************************************************************************
*******************************************************************************/

QLandmarkCategoryFilterPrivate::QLandmarkCategoryFilterPrivate(const QLandmarkCategoryId &id)
        : id(id)
{
    type = QLandmarkFilter::CategoryFilter;
}

QLandmarkCategoryFilterPrivate::QLandmarkCategoryFilterPrivate(const QLandmarkCategoryFilterPrivate &other)
        : QLandmarkFilterPrivate(other),
        id(other.id) {}

QLandmarkCategoryFilterPrivate::~QLandmarkCategoryFilterPrivate() {}

QTM_END_NAMESPACE
