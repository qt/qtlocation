/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtLocation module of the Qt Toolkit.
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeoshape.h"
#include "qgeoshape_p.h"

QT_BEGIN_NAMESPACE

QGeoShapePrivate::QGeoShapePrivate(QGeoShape::ShapeType type)
:   type(type)
{
}

QGeoShapePrivate::~QGeoShapePrivate()
{
}

bool QGeoShapePrivate::operator==(const QGeoShapePrivate &other) const
{
    return type == other.type;
}

/*!
    \class QGeoShape
    \inmodule QtLocation
    \ingroup QtLocation-positioning
    \since Qt Location 5.0

    \brief The QGeoShape class defines a geographic area.

    This class is the base class for classes which specify a geographic
    area.

    For the sake of consistency, subclasses should describe the specific
    details of the associated areas in terms of QGeoCoordinate instances
    and distances in meters.
*/

/*!
    \enum QGeoShape::ShapeType

    Describes the type of the shape.

    \value UnknownType      A shape of unknown type.
    \value RectangleType    A rectangular shape.
    \value CircleType       A circular shape.
*/

inline QGeoShapePrivate *QGeoShape::d_func()
{
    return static_cast<QGeoShapePrivate *>(d_ptr.data());
}

inline const QGeoShapePrivate *QGeoShape::d_func() const
{
    return static_cast<const QGeoShapePrivate *>(d_ptr.constData());
}

/*!
    Constructs a new invalid geo shape of \l UnknownType.
*/
QGeoShape::QGeoShape()
{
}

/*!
    Constructs a new geo shape which is a copy of \a other.
*/
QGeoShape::QGeoShape(const QGeoShape &other)
:   d_ptr(other.d_ptr)
{
}

/*!
    \internal
*/
QGeoShape::QGeoShape(QGeoShapePrivate *d)
:   d_ptr(d)
{
}

/*!
    Destroys this geo shape.
*/
QGeoShape::~QGeoShape()
{
}

/*!
    Returns the type of this geo shape.
*/
QGeoShape::ShapeType QGeoShape::type() const
{
    Q_D(const QGeoShape);

    if (d)
        return d->type;
    else
        return UnknownType;
}

/*!
    Returns whether this geo shape is valid.

    An geo shape is considered to be invalid if some of the data that is required to
    unambiguously describe the geo shape has not been set or has been set to an
    unsuitable value.
*/
bool QGeoShape::isValid() const
{
    Q_D(const QGeoShape);

    if (d)
        return d->isValid();
    else
        return false;
}

/*!
    Returns whether this geo shape is empty.

    An empty geo shape is a region which has a geometrical area of 0.
*/
bool QGeoShape::isEmpty() const
{
    Q_D(const QGeoShape);

    if (d)
        return d->isEmpty();
    else
        return true;
}

/*!
    Returns whether the coordinate \a coordinate is contained within this geo shape.
*/
bool QGeoShape::contains(const QGeoCoordinate &coordinate) const
{
    Q_D(const QGeoShape);

    if (d)
        return d->contains(coordinate);
    else
        return false;
}


/*!
    Returns true if the \a other geo shape is equivalent to this geo shape, otherwise returns
    false.
*/
bool QGeoShape::operator==(const QGeoShape &other) const
{
    Q_D(const QGeoShape);

    if (d == other.d_func())
        return true;

    if (!d || !(other.d_func()))
        return false;

    return *d == *other.d_func();
}

/*!
    Returns true if the \a other geo shape is not equivalent to this geo shape, otherwise returns
    false.
*/
bool QGeoShape::operator!=(const QGeoShape &other) const
{
    return !(*this == other);
}

/*!
    Assigns \a other to this geo shape and returns a reference to this geo shape.
*/
QGeoShape &QGeoShape::operator=(const QGeoShape &other)
{
    if (this == &other)
        return *this;

    d_ptr = other.d_ptr;
    return *this;
}

QT_END_NAMESPACE
