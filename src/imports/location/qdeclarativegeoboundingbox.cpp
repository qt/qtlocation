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
#include "qdeclarativegeoboundingbox_p.h"

QT_BEGIN_NAMESPACE

/*!
    \qmlclass BoundingBox QDeclarativeGeoBoundingBox
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-positioning
    \since Qt Location 5.0

    \brief The BoundingBox element represents a rectangular geographic area.

    A BoundingBox is described by a \l{QDeclarativeCoordinate}{Coordinate} which
    represents the top-left of the BoundingBox and a second
    \l{QDeclarativeCoordinate}{Coordinate} which represents the bottom-right of
    BoundingBox.

    A BoundingBox is considered invalid if the top-left or bottom-right
    coordinates are invalid or if the top-left coordinate is South of the
    bottom-right coordinate.

    Bounding boxes can never cross the poles.

    If the height or center of a bounding box is adjusted such that it would
    cross one of the poles the height is modified such that the bounding box
    touches but does not cross the pole and that the center coordinate is still
    in the center of the bounding box.

    \section2 Example Usage

    The following code snippet shows the declaration of a BoundingBox element.

    \code
    BoundingBox {
        topLeft: Coordinate {
            latitude: 23.34
            longitude: 44.4
        }
        bottomRight: Coordinate {
            latitude: 22.25
            longitude: 42.88
        }
    }
    \endcode

    This could then be used, for example, as a region to scan for landmarks,
    or refining searches.

    \sa QGeoBoundingBox.
*/

QDeclarativeGeoBoundingBox::QDeclarativeGeoBoundingBox(QObject* parent)
:   QDeclarativeGeoBoundingArea(parent), m_bottomLeft(0), m_bottomRight(0),
    m_topLeft(0), m_topRight(0), m_center(0), m_width(qQNaN()), m_height(qQNaN())
{
}

QDeclarativeGeoBoundingBox::QDeclarativeGeoBoundingBox(const QGeoBoundingBox &box, QObject *parent)
:   QDeclarativeGeoBoundingArea(parent), m_bottomLeft(0), m_bottomRight(0), m_topLeft(0),
    m_topRight(0), m_center(0), m_box(box), m_width(qQNaN()), m_height(qQNaN())
{
    synchronizeDeclarative(box, SkipNone);
}

/*!
    \qmlproperty QGeoBoundingBox BoundingBox::box

    For details on how to use this property to interface between C++ and QML see
    "\l {location-cpp-qml.html#boundingbox} {Interfaces between C++ and QML Code}".
*/
void QDeclarativeGeoBoundingBox::setBox(const QGeoBoundingBox& box)
{
    QGeoBoundingBox oldBox = m_box;
    m_box = box;
    synchronizeDeclarative(oldBox, SkipNone);
}

QGeoBoundingBox QDeclarativeGeoBoundingBox::box()
{
    return m_box;
}

QGeoBoundingArea QDeclarativeGeoBoundingBox::area() const
{
    return m_box;
}

/*!
    \qmlmethod bool QDeclarativeGeoBoundingBox::contains(Coordinate coordinate)

    Returns the true if \a coordinate is within the bounding box; otherwise returns false.
*/
bool QDeclarativeGeoBoundingBox::contains(QDeclarativeCoordinate *coordinate)
{
    if (!coordinate)
        return false;

    return m_box.contains(coordinate->coordinate());
}

QDeclarativeCoordinate* QDeclarativeGeoBoundingBox::bottomLeft()
{
    if (!m_bottomLeft) {
        m_bottomLeft = new QDeclarativeCoordinate(m_box.bottomLeft(), this);
        connect(m_bottomLeft, SIGNAL(coordinateChanged(QGeoCoordinate)),
                this, SLOT(coordinateChanged()));
    }

    return m_bottomLeft;
}

/*!
  \qmlproperty Coordinate BoundingBox::bottomLeft

  This property holds the bottom left coordinate of this bounding box.

  Note: this property's changed() signal is currently emitted only if the
  whole element changes, not if only the contents of the element change.

  \sa {QGeoBoundingBox}
  */

void QDeclarativeGeoBoundingBox::setBottomLeft(QDeclarativeCoordinate *coordinate)
{
    if (m_bottomLeft == coordinate)
        return;

    if (m_bottomLeft) {
        disconnect(m_bottomLeft, SIGNAL(coordinateChanged(QGeoCoordinate)),
                   this, SLOT(coordinateChanged()));

        if (m_bottomLeft->parent() == this)
            delete m_bottomLeft;
    }

    m_bottomLeft = coordinate;

    if (m_bottomLeft) {
        connect(m_bottomLeft, SIGNAL(coordinateChanged(QGeoCoordinate)),
                this, SLOT(coordinateChanged()));
    }

    QGeoBoundingBox oldBox = m_box;
    m_box.setBottomLeft(coordinate ? coordinate->coordinate() : QGeoCoordinate());
    synchronizeDeclarative(oldBox, SkipBottomLeft);

    emit bottomLeftChanged();
}

QDeclarativeCoordinate* QDeclarativeGeoBoundingBox::bottomRight()
{
    if (!m_bottomRight) {
        m_bottomRight = new QDeclarativeCoordinate(m_box.bottomRight(), this);
        connect(m_bottomRight, SIGNAL(coordinateChanged(QGeoCoordinate)),
                this, SLOT(coordinateChanged()));
    }

    return m_bottomRight;
}

/*!
  \qmlproperty Coordinate BoundingBox::bottomRight

  This property holds the bottom right coordinate of this bounding box.

  Note: this property's changed() signal is currently emitted only if the
  whole element changes, not if only the contents of the element change.

  \sa {QGeoBoundingBox}
  */

void QDeclarativeGeoBoundingBox::setBottomRight(QDeclarativeCoordinate *coordinate)
{
    if (m_bottomRight == coordinate)
        return;

    if (m_bottomRight) {
        disconnect(m_bottomRight, SIGNAL(coordinateChanged(QGeoCoordinate)),
                   this, SLOT(coordinateChanged()));

        if (m_bottomRight->parent() == this)
            delete m_bottomRight;
    }

    m_bottomRight = coordinate;

    if (m_bottomRight) {
        connect(m_bottomRight, SIGNAL(coordinateChanged(QGeoCoordinate)),
                this, SLOT(coordinateChanged()));
    }

    QGeoBoundingBox oldBox = m_box;
    m_box.setBottomRight(coordinate ? coordinate->coordinate() : QGeoCoordinate());
    synchronizeDeclarative(oldBox, SkipBottomRight);

    emit bottomRightChanged();
}

QDeclarativeCoordinate* QDeclarativeGeoBoundingBox::topLeft()
{
    if (!m_topLeft) {
        m_topLeft = new QDeclarativeCoordinate(m_box.topLeft(), this);
        connect(m_topLeft, SIGNAL(coordinateChanged(QGeoCoordinate)),
                this, SLOT(coordinateChanged()));
    }

    return m_topLeft;
}

/*!
  \qmlproperty Coordinate BoundingBox::topLeft

  This property holds the top left coordinate of this bounding box.

  Note: this property's changed() signal is currently emitted only if the
  whole element changes, not if only the contents of the element change.

  \sa {QGeoBoundingBox}
  */

void QDeclarativeGeoBoundingBox::setTopLeft(QDeclarativeCoordinate *coordinate)
{
    if (m_topLeft == coordinate)
        return;

    if (m_topLeft) {
        disconnect(m_topLeft, SIGNAL(coordinateChanged(QGeoCoordinate)),
                   this, SLOT(coordinateChanged()));

        if (m_topLeft->parent() == this)
            delete m_topLeft;
    }

    m_topLeft = coordinate;

    if (m_topLeft) {
        connect(m_topLeft, SIGNAL(coordinateChanged(QGeoCoordinate)),
                this, SLOT(coordinateChanged()));
    }

    QGeoBoundingBox oldBox = m_box;
    m_box.setTopLeft(coordinate ? coordinate->coordinate() : QGeoCoordinate());
    synchronizeDeclarative(oldBox, SkipTopLeft);

    emit topLeftChanged();
}

QDeclarativeCoordinate* QDeclarativeGeoBoundingBox::topRight()
{
    if (!m_topRight) {
        m_topRight = new QDeclarativeCoordinate(m_box.topRight(), this);
        connect(m_topRight, SIGNAL(coordinateChanged(QGeoCoordinate)),
                this, SLOT(coordinateChanged()));
    }

    return m_topRight;
}

/*!
  \qmlproperty Coordinate BoundingBox::topRight

  This property holds the top right coordinate of this bounding box.

  Note: this property's changed() signal is currently emitted only if the
  whole element changes, not if only the contents of the element change.

  \sa {QGeoBoundingBox}
  */

void QDeclarativeGeoBoundingBox::setTopRight(QDeclarativeCoordinate *coordinate)
{
    if (m_topRight == coordinate)
        return;

    if (m_topRight) {
        disconnect(m_topRight, SIGNAL(coordinateChanged(QGeoCoordinate)),
                   this, SLOT(coordinateChanged()));

        if (m_topRight->parent() == this)
            delete m_topRight;
    }

    m_topRight = coordinate;

    if (m_topRight) {
        connect(m_topRight, SIGNAL(coordinateChanged(QGeoCoordinate)),
                this, SLOT(coordinateChanged()));
    }

    QGeoBoundingBox oldBox = m_box;
    m_box.setTopRight(coordinate ? coordinate->coordinate() : QGeoCoordinate());
    synchronizeDeclarative(oldBox, SkipTopRight);

    emit topRightChanged();
}

QDeclarativeCoordinate* QDeclarativeGeoBoundingBox::center()
{
    if (!m_center) {
        m_center = new QDeclarativeCoordinate(m_box.center(), this);
        connect(m_center, SIGNAL(coordinateChanged(QGeoCoordinate)),
                this, SLOT(coordinateChanged()));
    }

    return m_center;
}

/*!
  \qmlproperty Coordinate BoundingBox::center

  This property holds the center coordinate of this bounding box.

  Note: this property's changed() signal is currently emitted only if the
  whole element changes, not if only the contents of the element change.

  \sa {QGeoBoundingBox}

  */

void QDeclarativeGeoBoundingBox::setCenter(QDeclarativeCoordinate *coordinate)
{
    if (m_center == coordinate)
        return;

    if (m_center) {
        disconnect(m_center, SIGNAL(coordinateChanged(QGeoCoordinate)),
                   this, SLOT(coordinateChanged()));

        if (m_center->parent() == this)
            delete m_center;
    }

    m_center = coordinate;

    if (m_center) {
        connect(m_center, SIGNAL(coordinateChanged(QGeoCoordinate)),
                this, SLOT(coordinateChanged()));
    }

    QGeoBoundingBox oldBox = m_box;
    m_box.setCenter(coordinate ? coordinate->coordinate() : QGeoCoordinate());
    synchronizeDeclarative(oldBox, SkipCenter);

    emit centerChanged();
}

double QDeclarativeGeoBoundingBox::height()
{
    return m_height;
}

/*!
  \qmlproperty double BoundingBox::height

  This property holds the height of this bounding box (in degrees).

  \sa {QGeoBoundingBox}
  */

void QDeclarativeGeoBoundingBox::setHeight(double height)
{
    QGeoBoundingBox oldBox = m_box;
    if (!m_box.isValid())
        m_box.setCenter(QGeoCoordinate(0.0, 0.0));
    m_box.setHeight(height);
    synchronizeDeclarative(oldBox, SkipNone);
}

double QDeclarativeGeoBoundingBox::width()
{
    return m_width;
}

/*!
  \qmlproperty double BoundingBox::width

  This property holds the width of this bounding box (in degrees).

  \sa {QGeoBoundingBox}

  */

void QDeclarativeGeoBoundingBox::setWidth(double width)
{
    QGeoBoundingBox oldBox = m_box;
    if (!m_box.isValid())
        m_box.setCenter(QGeoCoordinate(0.0, 0.0));
    m_box.setWidth(width);
    synchronizeDeclarative(oldBox, SkipNone);
}

void QDeclarativeGeoBoundingBox::coordinateChanged()
{
    QDeclarativeCoordinate *c = qobject_cast<QDeclarativeCoordinate *>(sender());
    if (!c)
        return;

    QGeoBoundingBox oldBox = m_box;

    if (c == m_bottomLeft) {
        m_box.setBottomLeft(c->coordinate());
        synchronizeDeclarative(oldBox, SkipBottomLeft);
    } else if (c == m_bottomRight) {
        m_box.setBottomRight(c->coordinate());
        synchronizeDeclarative(oldBox, SkipBottomRight);
    } else if (c == m_topLeft) {
        m_box.setTopLeft(c->coordinate());
        synchronizeDeclarative(oldBox, SkipTopLeft);
    } else if (c == m_topRight) {
        m_box.setTopRight(c->coordinate());
        synchronizeDeclarative(oldBox, SkipTopRight);
    } else if (c == m_center) {
        m_box.setCenter(c->coordinate());
        synchronizeDeclarative(oldBox, SkipCenter);
    }
}

void QDeclarativeGeoBoundingBox::synchronizeDeclarative(const QGeoBoundingBox &old, SkipProp skip)
{
    if (skip != SkipBottomLeft && m_bottomLeft && old.bottomLeft() != m_box.bottomLeft())
        m_bottomLeft->setCoordinate(m_box.bottomLeft());

    if (skip != SkipBottomRight && m_bottomRight && old.bottomRight() != m_box.bottomRight())
        m_bottomRight->setCoordinate(m_box.bottomRight());

    if (skip != SkipTopLeft && m_topLeft && old.topLeft() != m_box.topLeft())
        m_topLeft->setCoordinate(m_box.topLeft());

    if (skip != SkipTopRight && m_topRight && old.topRight() != m_box.topRight())
        m_topRight->setCoordinate(m_box.topRight());

    if (skip != SkipCenter && m_center && old.center() != m_box.center())
        m_center->setCoordinate(m_box.center());

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

#include "moc_qdeclarativegeoboundingbox_p.cpp"

QT_END_NAMESPACE
