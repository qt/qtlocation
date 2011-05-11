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
***************************************************************************/

#include "qdeclarativegeoplace_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass Place QDeclarativeGeoPlace
    \brief The A Place element presents basic information about a place.
    \ingroup qml-location

    This element is part of the \bold{QtMobility.location 1.1} module.

    The Place element represents basic information about a place.

    A Place contains a coordinate and the corresponding address,
    along with an optional bounding box describing the minimum viewport
    necessary to display the entirety of the place.

    \sa Landmark, Address, {QGeoPlace}
*/

QDeclarativeGeoPlace::QDeclarativeGeoPlace(QObject* parent) :
        QObject(parent)
{
}

QDeclarativeGeoPlace::QDeclarativeGeoPlace(const QGeoPlace& place, QObject* parent) :
        QObject(parent),
        m_declarativeBox(place.viewport(), this),
        m_declarativeCoordinate(place.coordinate(), this),
        m_declarativeAddress(place.address(), this)
{
}

QDeclarativeGeoPlace::~QDeclarativeGeoPlace()
{
}

void QDeclarativeGeoPlace::setPlace(const QGeoPlace& place)
{
    m_declarativeBox.setBox(place.viewport());
    m_declarativeCoordinate.setCoordinate(place.coordinate());
    m_declarativeAddress.setAddress(place.address());
}

QDeclarativeGeoBoundingBox* QDeclarativeGeoPlace::viewport()
{
    return &m_declarativeBox;
}

/*!
    \qmlproperty BoundingBox Place::viewport

    This property holds the viewport of the place
    (minimum viewport necessary to display the entirety of the place).

    Note: viewportChanged() signal is currently emitted only if the
    whole element changes, not if only the contents of the element change.
  */

void QDeclarativeGeoPlace::setViewport(QDeclarativeGeoBoundingBox* box)
{
    if (m_declarativeBox.box() == box->box())
        return;
    m_declarativeBox.setBox(box->box());
    emit viewportChanged();
}

QDeclarativeCoordinate* QDeclarativeGeoPlace::coordinate()
{
    return &m_declarativeCoordinate;
}

/*!
    \qmlproperty Coordinate Place::coordinate

    This property holds coordinate where this place is located at.

    Note: coordinateChanged() signal is currently emitted only if the
    whole element changes, not if only the contents of the element change.

  */

void QDeclarativeGeoPlace::setCoordinate(QDeclarativeCoordinate* coordinate)
{
    if (m_declarativeCoordinate.coordinate() == coordinate->coordinate())
        return;
    m_declarativeCoordinate.setCoordinate(coordinate->coordinate());
    emit coordinateChanged();
}

QDeclarativeGeoAddress* QDeclarativeGeoPlace::address()
{
    return &m_declarativeAddress;
}

/*!
    \qmlproperty Address Place::address

    This property holds address of the place.

    Note: addressChanged() signal is currently emitted only if the
    whole address element changes, not if only the contents of the element
    change.

  */

void QDeclarativeGeoPlace::setAddress(QDeclarativeGeoAddress* address)
{
    if (m_declarativeAddress.address() == address->address())
        return;
    m_declarativeAddress.setAddress(address->address());
    emit addressChanged();
}

#include "moc_qdeclarativegeoplace_p.cpp"

QTM_END_NAMESPACE
