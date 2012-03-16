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

#include "qgeoboundingarea.h"
#include "qgeoboundingarea_p.h"

QT_BEGIN_NAMESPACE

QGeoBoundingAreaPrivate::QGeoBoundingAreaPrivate(QGeoBoundingArea::AreaType type)
:   type(type)
{
}

QGeoBoundingAreaPrivate::~QGeoBoundingAreaPrivate()
{
}

bool QGeoBoundingAreaPrivate::operator==(const QGeoBoundingAreaPrivate &other) const
{
    return type == other.type;
}

/*!
    \class QGeoBoundingArea
    \inmodule QtLocation
    \ingroup QtLocation-positioning
    \since QtLocation 5.0

    \brief The QGeoBoundingArea class defines a geographic area.

    This class is the base class for classes which specify a geographic
    area.

    For the sake of consistency, subclasses should describe the specific
    details of the associated areas in terms of QGeoCoordinate instances
    and distances in meters.
*/

/*!
    \enum QGeoBoundingArea::AreaType

    Describes the type of a bounding area.

    \value BoxType A box shaped bounding area.

    \value CircleType A circular bounding area.
*/

inline QGeoBoundingAreaPrivate *QGeoBoundingArea::d_func()
{
    return static_cast<QGeoBoundingAreaPrivate *>(d_ptr.data());
}

inline const QGeoBoundingAreaPrivate *QGeoBoundingArea::d_func() const
{
    return static_cast<const QGeoBoundingAreaPrivate *>(d_ptr.constData());
}

QGeoBoundingArea::QGeoBoundingArea()
{
}

QGeoBoundingArea::QGeoBoundingArea(const QGeoBoundingArea &other)
:   d_ptr(other.d_ptr)
{
}

QGeoBoundingArea::QGeoBoundingArea(QGeoBoundingAreaPrivate *d)
:   d_ptr(d)
{
}

/*!
    Destroys this bounding area.
*/
QGeoBoundingArea::~QGeoBoundingArea()
{
}

/*!
    Returns the type of this area.
*/
QGeoBoundingArea::AreaType QGeoBoundingArea::type() const
{
    Q_D(const QGeoBoundingArea);

    if (d)
        return d->type;
    else
        return UnknownType;
}

/*!
    Returns whether this bounding area is valid.

    An area is considered to be invalid if some of the data that is required to
    unambiguously describe the area has not been set or has been set to an
    unsuitable value.
*/
bool QGeoBoundingArea::isValid() const
{
    Q_D(const QGeoBoundingArea);

    if (d)
        return d->isValid();
    else
        return false;
}

/*!
    Returns whether this bounding area is empty.

    An empty area is a region which has a geometrical area of 0.
*/
bool QGeoBoundingArea::isEmpty() const
{
    Q_D(const QGeoBoundingArea);

    if (d)
        return d->isEmpty();
    else
        return true;
}

/*!
    Returns whether the coordinate \a coordinate is contained within this area.
*/
bool QGeoBoundingArea::contains(const QGeoCoordinate &coordinate) const
{
    Q_D(const QGeoBoundingArea);

    if (d)
        return d->contains(coordinate);
    else
        return false;
}


/*!
    Returns true if the \a other bounding area is equivalent to this bounding area, otherwise
    returns false
*/
bool QGeoBoundingArea::operator==(const QGeoBoundingArea &other) const
{
    Q_D(const QGeoBoundingArea);

    if (d == other.d_func())
        return true;

    if (!d || !(other.d_func()))
        return false;

    return *d == *other.d_func();
}

bool QGeoBoundingArea::operator!=(const QGeoBoundingArea &other) const
{
    return !(*this == other);
}

QGeoBoundingArea &QGeoBoundingArea::operator=(const QGeoBoundingArea &other)
{
    d_ptr = other.d_ptr;
    return *this;
}

QT_END_NAMESPACE
