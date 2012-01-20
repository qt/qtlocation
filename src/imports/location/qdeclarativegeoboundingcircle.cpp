/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
** $QT_END_LICENSE$
**
***************************************************************************/

#include <qnumeric.h>
#include "qdeclarativegeoboundingcircle_p.h"

QT_BEGIN_NAMESPACE

/*!
    \qmlclass BoundingCircle QDeclarativeGeoBoundingCircle
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-positioning
    \since QtLocation 5.0

    \brief The BoundingCircle element represents a circular geographic area.
*/

QDeclarativeGeoBoundingCircle::QDeclarativeGeoBoundingCircle(QObject *parent)
:   QDeclarativeGeoBoundingArea(parent), m_center(0), m_radius(-1.0)
{
}

QDeclarativeGeoBoundingCircle::QDeclarativeGeoBoundingCircle(const QGeoBoundingCircle &circle,
                                                             QObject *parent)
:   QDeclarativeGeoBoundingArea(parent), m_center(0), m_circle(circle), m_radius(qQNaN())
{
    synchronizeDeclarative(circle, false);
}

/*!
    \qmlproperty QGeoBoundingCircle BoundingCircle::circle

    This property is used to provide an interface between C++ and QML code.  First a pointer to a
    BoundingCircle object must be obtained from C++, then use the
    \l {QObject::property()}{property()} and \l {QObject::setProperty()}{setProperty()} functions
    to get and set the \c circle property.

    The following gets the QGeoBoundingCircle representing this object from C++:

    \snippet snippets/cpp/cppqml.cpp BoundingCircle get

    The following sets the properties of this object based on a QGeoBoundingCircle object from C++:

    \snippet snippets/cpp/cppqml.cpp BoundingCircle set
*/
void QDeclarativeGeoBoundingCircle::setCircle(const QGeoBoundingCircle& circle)
{
    QGeoBoundingCircle oldCircle = m_circle;
    m_circle = circle;
    synchronizeDeclarative(oldCircle, false);
}

QGeoBoundingCircle QDeclarativeGeoBoundingCircle::circle() const
{
    return m_circle;
}

QGeoBoundingArea *QDeclarativeGeoBoundingCircle::area() const
{
    return m_circle.clone();
}

/*!
    \qmlmethod bool QDeclarativeGeoBoundingCircle::contains(Coordinate coordinate)

    Returns the true if \a coordinate is within the bounding circle; otherwise returns false.
*/
bool QDeclarativeGeoBoundingCircle::contains(QDeclarativeCoordinate *coordinate)
{
    if (!coordinate)
        return false;

    return m_circle.contains(coordinate->coordinate());
}

/*!
    \qmlproperty Coordinate BoundingCircle::center

    This property holds the coordinate of the center of the bounding circle.
*/
QDeclarativeCoordinate* QDeclarativeGeoBoundingCircle::center()
{
    if (!m_center) {
        m_center = new QDeclarativeCoordinate(m_circle.center(), this);
        connect(m_center, SIGNAL(coordinateChanged(QGeoCoordinate)),
                this, SLOT(coordinateChanged()));
    }

    return m_center;
}

void QDeclarativeGeoBoundingCircle::setCenter(QDeclarativeCoordinate *coordinate)
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

    QGeoBoundingCircle oldCircle = m_circle;
    m_circle.setCenter(coordinate ? coordinate->coordinate() : QGeoCoordinate());
    synchronizeDeclarative(oldCircle, true);

    emit centerChanged();
}

/*!
    \qmlproperty qreal BoundingCircle::radius

    This property holds the radius of the bounding circle in meters.

    The default value for the radius is -1 indicating an invalid bounding circle area.
*/
qreal QDeclarativeGeoBoundingCircle::radius() const
{
    return m_radius;
}

void QDeclarativeGeoBoundingCircle::setRadius(qreal radius)
{
    QGeoBoundingCircle oldCircle = m_circle;
    m_circle.setRadius(radius);
    synchronizeDeclarative(oldCircle, false);
}

void QDeclarativeGeoBoundingCircle::coordinateChanged()
{
    QDeclarativeCoordinate *c = qobject_cast<QDeclarativeCoordinate *>(sender());
    if (!c)
        return;

    QGeoBoundingCircle oldCircle = m_circle;

    if (c == m_center) {
        m_circle.setCenter(c->coordinate());
        synchronizeDeclarative(oldCircle, true);
    }
}

void QDeclarativeGeoBoundingCircle::synchronizeDeclarative(const QGeoBoundingCircle &old, bool skipCenter)
{
    if (!skipCenter && m_center && old.center() != m_circle.center())
        m_center->setCoordinate(m_circle.center());

    // Check not to compare two Not a Numbers, which by definition is 'false'.
    if ((!qIsNaN(old.radius()) || !qIsNaN(m_circle.radius())) && old.radius() != m_circle.radius()) {
        m_radius = m_circle.radius();
        emit radiusChanged();
    }
}

#include "moc_qdeclarativegeoboundingcircle_p.cpp"

QT_END_NAMESPACE
