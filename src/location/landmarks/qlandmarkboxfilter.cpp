/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qlandmarkboxfilter.h"
#include "qlandmarkboxfilter_p.h"
#include "qgeocoordinate.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkBoxFilter
    \brief The QLandmarkBoxFilter class is used to search for landmarks within a given bounding box.

    \inmodule QtLocation

    \ingroup landmarks-filter
    \since 1.1

    In order to be a valid filter, the region covered by the box filter must not cover one of the poles.
*/

Q_IMPLEMENT_LANDMARKFILTER_PRIVATE(QLandmarkBoxFilter);

/*!
    Creates a filter that will search for landmarks within a bounding
    box defined by the \a topLeft and \a bottomRight coordinates.
    \since 1.1
*/
QLandmarkBoxFilter::QLandmarkBoxFilter(const QGeoCoordinate &topLeft,
                                       const QGeoCoordinate &bottomRight)
        : QLandmarkFilter(new QLandmarkBoxFilterPrivate(QGeoBoundingBox(topLeft, bottomRight)))
{
}

/*!
    Creates a filter that will search for landmarks within the \a boundingBox.
    \since 1.1
*/
QLandmarkBoxFilter::QLandmarkBoxFilter(const QGeoBoundingBox &boundingBox)
        : QLandmarkFilter(new QLandmarkBoxFilterPrivate(boundingBox))
{
}

/*!
    \fn QLandmarkBoxFilter::QLandmarkBoxFilter(const QLandmarkFilter &other)
    Constructs a copy of \a other if possible, otherwise constructs a new box filter.
    \since 1.1
*/

/*!
    Destroys the filter.
*/
QLandmarkBoxFilter::~QLandmarkBoxFilter()
{
    // pointer deleted in superclass destructor
}

/*!
    Returns the filter's bounding box.
    \since 1.1
*/
QGeoBoundingBox QLandmarkBoxFilter::boundingBox() const
{
    Q_D(const QLandmarkBoxFilter);
    return d->boundingBox;
}

/*!
    Sets the \a boundingBox of the filter.
    \since 1.1
*/
void QLandmarkBoxFilter::setBoundingBox(const QGeoBoundingBox &boundingBox)
{
    Q_D(QLandmarkBoxFilter);
    d->boundingBox = boundingBox;
}

/*!
    Returns the top left coordinate of the box filter.
    \since 1.1
*/
QGeoCoordinate QLandmarkBoxFilter::topLeft()
{
    Q_D(const QLandmarkBoxFilter);
    return d->boundingBox.topLeft();
}

/*!
  Sets the \a topLeft coordinate of the box filter.
    \since 1.1
*/
void QLandmarkBoxFilter::setTopLeft(const QGeoCoordinate &topLeft)
{
    Q_D(QLandmarkBoxFilter);
    d->boundingBox.setTopLeft(topLeft);
}


/*!
    Returns the bottom right coordinate of the box filter.
    \since 1.1
*/
QGeoCoordinate QLandmarkBoxFilter::bottomRight()
{
    Q_D(const QLandmarkBoxFilter);
    return d->boundingBox.bottomRight();
}

/*!
    Sets the \a bottomRight coordinate of the box filter.
    \since 1.1
*/
void QLandmarkBoxFilter::setBottomRight(const QGeoCoordinate &bottomRight)
{
    Q_D(QLandmarkBoxFilter);
    return d->boundingBox.setBottomRight(bottomRight);
}

/*******************************************************************************
*******************************************************************************/

QLandmarkBoxFilterPrivate::QLandmarkBoxFilterPrivate(const QGeoBoundingBox &box)
        : QLandmarkFilterPrivate(),
        boundingBox(box)
{
    type = QLandmarkFilter::BoxFilter;
}

QLandmarkBoxFilterPrivate::QLandmarkBoxFilterPrivate(const QLandmarkBoxFilterPrivate &other)
        : QLandmarkFilterPrivate(other),
        boundingBox(other.boundingBox) {}

QLandmarkBoxFilterPrivate::~QLandmarkBoxFilterPrivate() {}

QTM_END_NAMESPACE
