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
***************************************************************************/

#include <qnumeric.h>
#include "qdeclarativegeorectangle.h"

QT_BEGIN_NAMESPACE

/*!
    \qmltype GeoRectangle
    \instantiates QDeclarativeGeoRectangle
    \inqmlmodule QtLocation 5.0
    \ingroup qml-QtLocation5-positioning
    \since Qt Location 5.0

    \brief The GeoRectangle type represents a rectangular geographic area.

    A GeoRectangle is described by a \l {QtLocation5::coordinate}{coordinate} which represents the
    top-left of the GeoRectangle and a second \l {QtLocation5::coordinate}{coordinate} which
    represents the bottom-right of GeoRectangle.

    A GeoRectangle is considered invalid if the top-left or bottom-right
    coordinates are invalid or if the top-left coordinate is South of the
    bottom-right coordinate.

    GeoRectangles can never cross the poles.

    If the height or center of a geo rectangle is adjusted such that it would
    cross one of the poles the height is modified such that the geo rectangle
    touches but does not cross the pole and that the center coordinate is still
    in the center of the geo rectangle.

    \section2 Example Usage

    The following code snippet shows the declaration of a GeoRectangle object.

    \code
    GeoRectangle {
        topLeft {
            latitude: 23.34
            longitude: 44.4
        }
        bottomRight {
            latitude: 22.25
            longitude: 42.88
        }
    }
    \endcode

    This could then be used, for example, as a region to search for places.

    \sa QGeoRectangle
*/

QDeclarativeGeoRectangle::QDeclarativeGeoRectangle(QObject *parent)
:   QDeclarativeGeoShape(parent), m_width(qQNaN()), m_height(qQNaN())
{
}

QDeclarativeGeoRectangle::QDeclarativeGeoRectangle(const QGeoRectangle &box, QObject *parent)
:   QDeclarativeGeoShape(parent), m_box(box), m_width(qQNaN()), m_height(qQNaN())
{
    emitChanged(box);
}

/*!
    \qmlproperty QGeoRectangle GeoRectangle::rectangle

    For details on how to use this property to interface between C++ and QML see
    "\l {location-cpp-qml.html#georectangle}{Interfaces between C++ and QML Code}".
*/
void QDeclarativeGeoRectangle::setRectangle(const QGeoRectangle &box)
{
    QGeoRectangle oldBox = m_box;
    m_box = box;
    emitChanged(oldBox);
}

QGeoRectangle QDeclarativeGeoRectangle::rectangle() const
{
    return m_box;
}

QGeoShape QDeclarativeGeoRectangle::shape() const
{
    return m_box;
}

/*!
    \qmlmethod bool QDeclarativeGeoRectangle::contains(coordinate coordinate)

    Returns the true if \a coordinate is within the geo rectangle; otherwise returns false.
*/
bool QDeclarativeGeoRectangle::contains(const QGeoCoordinate &coordinate)
{
    return m_box.contains(coordinate);
}

QGeoCoordinate QDeclarativeGeoRectangle::bottomLeft()
{
    return m_box.bottomLeft();
}

/*!
    \qmlproperty coordinate GeoRectangle::bottomLeft

    This property holds the bottom left coordinate of this geo rectangle.

    \sa {QGeoRectangle}
*/
void QDeclarativeGeoRectangle::setBottomLeft(const QGeoCoordinate &coordinate)
{
    if (m_box.bottomLeft() == coordinate)
        return;

    QGeoRectangle old = m_box;
    m_box.setBottomLeft(coordinate);
    emitChanged(old);
}

QGeoCoordinate QDeclarativeGeoRectangle::bottomRight()
{
    return m_box.bottomRight();
}

/*!
    \qmlproperty coordinate GeoRectangle::bottomRight

    This property holds the bottom right coordinate of this geo rectangle.

    \sa {QGeoRectangle}
*/
void QDeclarativeGeoRectangle::setBottomRight(const QGeoCoordinate &coordinate)
{
    if (m_box.bottomRight() == coordinate)
        return;

    QGeoRectangle old = m_box;
    m_box.setBottomRight(coordinate);
    emitChanged(old);
}

QGeoCoordinate QDeclarativeGeoRectangle::topLeft()
{
    return m_box.topLeft();
}

/*!
    \qmlproperty coordinate GeoRectangle::topLeft

    This property holds the top left coordinate of this geo rectangle.

    \sa {QGeoRectangle}
*/
void QDeclarativeGeoRectangle::setTopLeft(const QGeoCoordinate &coordinate)
{
    if (m_box.topLeft() == coordinate)
        return;

    QGeoRectangle old = m_box;
    m_box.setTopLeft(coordinate);
    emitChanged(old);
}

QGeoCoordinate QDeclarativeGeoRectangle::topRight()
{
    return m_box.topRight();
}

/*!
  \qmlproperty coordinate GeoRectangle::topRight

  This property holds the top right coordinate of this geo rectangle.

  \note this property's changed() signal is currently emitted only if the
  whole type changes, not if only the contents of the type change.

  \sa {QGeoRectangle}
  */

void QDeclarativeGeoRectangle::setTopRight(QGeoCoordinate &coordinate)
{
    if (m_box.topRight() == coordinate)
        return;

    QGeoRectangle old = m_box;
    m_box.setTopRight(coordinate);
    emitChanged(old);
}

QGeoCoordinate QDeclarativeGeoRectangle::center()
{
    return m_box.center();
}

/*!
  \qmlproperty coordinate GeoRectangle::center

  This property holds the center coordinate of this geo rectangle.

  \note this property's changed() signal is currently emitted only if the
  whole object changes, not if only the contents of the object change.

  \sa {QGeoRectangle}

  */

void QDeclarativeGeoRectangle::setCenter(const QGeoCoordinate &coordinate)
{
    if (m_box.center() == coordinate)
        return;

    QGeoRectangle old = m_box;
    m_box.setCenter(coordinate);
    emitChanged(old);
}

double QDeclarativeGeoRectangle::height()
{
    return m_height;
}

/*!
  \qmlproperty double GeoRectangle::height

  This property holds the height of this geo rectangle (in degrees).

  \sa {QGeoRectangle}
  */

void QDeclarativeGeoRectangle::setHeight(double height)
{
    QGeoRectangle oldBox = m_box;
    if (!m_box.isValid())
        m_box.setCenter(QGeoCoordinate(0.0, 0.0));
    m_box.setHeight(height);
    emitChanged(oldBox);
}

double QDeclarativeGeoRectangle::width()
{
    return m_width;
}

/*!
  \qmlproperty double GeoRectangle::width

  This property holds the width of this geo rectangle (in degrees).

  \sa {QGeoRectangle}

  */

void QDeclarativeGeoRectangle::setWidth(double width)
{
    QGeoRectangle oldBox = m_box;
    if (!m_box.isValid())
        m_box.setCenter(QGeoCoordinate(0.0, 0.0));
    m_box.setWidth(width);
    emitChanged(oldBox);
}

void QDeclarativeGeoRectangle::emitChanged(const QGeoRectangle &old)
{
    if (old.bottomLeft() != m_box.bottomLeft())
        emit bottomLeftChanged();

    if (old.bottomRight() != m_box.bottomRight())
        emit bottomRightChanged();

    if (old.topLeft() != m_box.topLeft())
        emit topLeftChanged();

    if (old.topRight() != m_box.topRight())
        emit topRightChanged();

    if (old.center() != m_box.center())
        emit centerChanged();

    // Check not to compare two Not a Numbers, which by definition is 'false'.
    if ((!qIsNaN(old.width()) || !qIsNaN(m_box.width())) && old.width() != m_box.width()) {
        m_width = m_box.width();
        emit widthChanged();
    }
    if ((!qIsNaN(old.height()) || !qIsNaN(m_box.height())) && old.height() != m_box.height()) {
        m_height = m_box.height();
        emit heightChanged();
    }
}

#include "moc_qdeclarativegeorectangle.cpp"

QT_END_NAMESPACE
