/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qgeoboundingarea.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoBoundingArea
    \brief The QGeoBoundingArea class defines a geographic area.

    \inmodule QtLocation
    \since 1.1

    \ingroup maps

    This class is the base class for classes which specify a geographic
    area.

    For the sake of consistency, subclasses should describe the specific
    details of the associated areas in terms of QGeoCoordinate instances
    and distances in metres.
*/

/*!
    \enum QGeoBoundingArea::AreaType

    Describes the type of a bounding area.

    \value BoxType A box shaped bounding area.

    \value CircleType A circular bounding area.
*/

/*!
    Destroys this bounding area.
*/
QGeoBoundingArea::~QGeoBoundingArea() {}

/*!
\fn virtual QGeoBoundingArea::AreaType QGeoBoundingArea::type() const

Returns the type of this area.
*/

/*!
\fn bool QGeoBoundingArea::isValid() const

Returns whether this bounding area is valid.

An area is considered to be invalid if some of the data that is required to
unambiguously describe the area has not been set or has been set to an
unsuitable value.
*/

/*!
\fn bool QGeoBoundingArea::isEmpty() const

Returns whether this bounding area is empty.

An empty area is a region which has a geometrical area of 0.
*/

/*!
\fn bool QGeoBoundingArea::contains(const QGeoCoordinate &coordinate) const

Returns whether the coordinate \a coordinate is contained within this
area.
*/

QTM_END_NAMESPACE
