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

#include "qgeomappolylineobject.h"
#include "qgeomappolylineobject_p.h"

#include "qgeocoordinate.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMapPolylineObject
    \brief The QGeoMapPolylineObject class is a QGeoMapObject used to draw
    a segmented line on a map.

    \inmodule QtLocation
    \since 1.1

    \ingroup maps-mapping-objects

    The polyline is specified by a list of at least 2 valid QGeoCoordinate
    instances and a line will be drawn between every adjacent pairs of
    coordinates in the list.
*/

/*!
    Constructs a new polyline object.
*/
QGeoMapPolylineObject::QGeoMapPolylineObject()
    : d_ptr(new QGeoMapPolylineObjectPrivate())
{
    setUnits(QGeoMapObject::RelativeArcSecondUnit);
    setTransformType(QGeoMapObject::ExactTransform);
}

/*!
    Destroys this polyline object.
*/
QGeoMapPolylineObject::~QGeoMapPolylineObject()
{
    delete d_ptr;
}

/*!
    \reimp
*/
QGeoMapObject::Type QGeoMapPolylineObject::type() const
{
    return QGeoMapObject::PolylineType;
}

/*!
    \property QGeoMapPolylineObject::path
    \brief This property holds the ordered list of coordinates which define the
    segmented line to be drawn by this polyline object.

    The default value of this property is an empty list of coordinates.

    A line will be drawn between every pair of coordinates which are adjacent in
    the list.

    Invalid coordinates in the list will be ignored, and if the list of
    coordinates contains less than 2 valid coordinates then the polyline object
    will not be displayed.
*/
void QGeoMapPolylineObject::setPath(const QList<QGeoCoordinate> &path)
{
    if (d_ptr->path != path) {
        d_ptr->path = path;
        setOrigin(path.at(0));
        emit pathChanged(d_ptr->path);
    }
}

QList<QGeoCoordinate> QGeoMapPolylineObject::path() const
{
    return d_ptr->path;
}

/*!
    \property QGeoMapPolylineObject::pen
    \brief This property holds the pen that will be used to draw this object.

    The pen is used to draw the polyline.

    The pen will be treated as a cosmetic pen, which means that the width
    of the pen will be independent of the zoom level of the map.
*/
void QGeoMapPolylineObject::setPen(const QPen &pen)
{
    QPen newPen = pen;
    newPen.setCosmetic(false);

    if (d_ptr->pen == newPen)
        return;

    d_ptr->pen = pen;
    emit penChanged(pen);
}

QPen QGeoMapPolylineObject::pen() const
{
    return d_ptr->pen;
}

/*!
\fn void QGeoMapPolylineObject::pathChanged(const QList<QGeoCoordinate> &path)

    This signal is emitted when the ordered list of coordinates that define
    the polyline to be drawn by this polyline object has changed.

    The new value is \a path.
*/

/*!
\fn void QGeoMapPolylineObject::penChanged(const QPen &pen)

    This signal is emitted when the pen used to draw the edge of this
    polyline object has changed.

    The new value is \a pen.
*/

/*******************************************************************************
*******************************************************************************/

QGeoMapPolylineObjectPrivate::QGeoMapPolylineObjectPrivate()
{
    pen.setCosmetic(true);
}

QGeoMapPolylineObjectPrivate::~QGeoMapPolylineObjectPrivate() {}

#include "moc_qgeomappolylineobject.cpp"

QTM_END_NAMESPACE

