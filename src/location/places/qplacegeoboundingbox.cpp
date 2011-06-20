/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qplacegeoboundingbox.h"
#include "qplacegeoboundingbox_p.h"

QTM_USE_NAMESPACE

QPlaceGeoBoundingBoxPrivate::QPlaceGeoBoundingBoxPrivate()
    : QSharedData()
{
}

QPlaceGeoBoundingBoxPrivate::QPlaceGeoBoundingBoxPrivate(const QPlaceGeoBoundingBoxPrivate &other)
    : QSharedData()
{
    this->topLeft = other.topLeft;
    this->bottomRight = other.bottomRight;
}

QPlaceGeoBoundingBoxPrivate::~QPlaceGeoBoundingBoxPrivate()
{
}

bool QPlaceGeoBoundingBoxPrivate::operator==(const QPlaceGeoBoundingBoxPrivate &other) const
{
    return (
            this->topLeft == other.topLeft
            && this->bottomRight == other.bottomRight
    );
}

/*!
    \class QPlaceGeoBoundingBox

    \inmodule QPlaces

    \brief The QPlaceGeoBoundingBox class represents a bounding box object.

    Each QPlaceGeoBoundingBox represents a bounding box with a top/left and bottom/right point.
*/

/*!
    Constructs a new position object.
*/
QPlaceGeoBoundingBox::QPlaceGeoBoundingBox()
    : d(new QPlaceGeoBoundingBoxPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceGeoBoundingBox::QPlaceGeoBoundingBox(const QPlaceGeoBoundingBox &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceGeoBoundingBox::~QPlaceGeoBoundingBox()
{
}

QPlaceGeoBoundingBox &QPlaceGeoBoundingBox::operator =(const QPlaceGeoBoundingBox &other) {
    d = other.d;
    return *this;
}

bool QPlaceGeoBoundingBox::operator==(const QPlaceGeoBoundingBox &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}


/*!
    Returns coordinate of top left edge of bounding box.
*/
QGeoCoordinate QPlaceGeoBoundingBox::topLeft() const
{
    return d->topLeft;
}

/*!
    Sets \a coordinate of top left edge of bounding box.
*/
void QPlaceGeoBoundingBox::setTopLeft(const QGeoCoordinate &coordinate)
{
    d->topLeft = coordinate;
}

/*!
    Returns coordinate of bottom right edge of bounding box.
*/
QGeoCoordinate QPlaceGeoBoundingBox::bottomRight() const
{
    return d->bottomRight;
}

/*!
    Sets \a coordinate of bottom right edge of bounding box.
*/
void QPlaceGeoBoundingBox::setBottomRight(const QGeoCoordinate &coordinate)
{
    d->bottomRight = coordinate;
}

/*!
    Returns true if bounding box is valid.
*/
bool QPlaceGeoBoundingBox::isValid() const
{
    return (d->bottomRight.isValid() && d->topLeft.isValid());
}
