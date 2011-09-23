/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
****************************************************************************/

#include "qdeclarativegeolocation_p.h"

QT_USE_NAMESPACE

/*!
    \qmlclass Location

    \brief The Location element holds location data.
    \inherits QObject

    Location cointains many properties holding data of the location like geo coordinates,
    address, etc.

    \ingroup qml-places
*/

QDeclarativeGeoLocation::QDeclarativeGeoLocation(QObject* parent)
        : QObject(parent)
{
}

QDeclarativeGeoLocation::QDeclarativeGeoLocation(const QGeoLocation &src,
        QObject *parent)
        : QObject(parent),
          m_address(src.address()),
          m_coordinate(src.coordinate()),
          m_boundingBox(src.viewport()),
          m_src(src)
{
}

QDeclarativeGeoLocation::~QDeclarativeGeoLocation()
{
}

void QDeclarativeGeoLocation::setLocation(const QGeoLocation &src)
{
    QGeoLocation previous = m_src;
    m_src = src;

    if (previous.address() != m_src.address()) {
        m_address.setAddress(m_src.address());
        emit addressChanged();
    }
    if (previous.coordinate() != m_src.coordinate()) {
        m_coordinate.setCoordinate(m_src.coordinate());
        emit coordinateChanged();
    }
    if (previous.locationId() != m_src.locationId()) {
        emit locationIdChanged();
    }
    if (previous.viewport() != m_src.viewport()) {
        emit viewport();
    }
}

QGeoLocation QDeclarativeGeoLocation::location()
{
    m_src.setAddress(m_address.address());
    m_src.setCoordinate(m_coordinate.coordinate());
    m_src.setViewport(m_boundingBox.box());
    return m_src;
}

/*!
    \qmlproperty string Location::address

    This property holds address of the location.
*/
void QDeclarativeGeoLocation::setAddress(QDeclarativeGeoAddress *address)
{
    if (m_src.address() != address->address()) {
        m_address.setAddress(address->address());
        m_src.setAddress(address->address());
        emit addressChanged();
    }
}

QDeclarativeGeoAddress *QDeclarativeGeoLocation::address()
{
    return &m_address;
}

/*!
    \qmlproperty string Location::coordinate

    This property holds display coordinates of the location.

   Note: this property's changed() signal is currently emitted only if the
   whole element changes, not if only the contents of the element change.
*/
void QDeclarativeGeoLocation::setCoordinate(QDeclarativeCoordinate *coordinate)
{
    if (m_src.coordinate() != coordinate->coordinate()) {
        m_coordinate.setCoordinate(coordinate->coordinate());
        m_src.setCoordinate(coordinate->coordinate());
        emit coordinateChanged();
    }
}

QDeclarativeCoordinate *QDeclarativeGeoLocation::coordinate()
{
    return &m_coordinate;
}

/*!
    \qmlproperty string Location::locationId

    This property holds location id.
*/
void QDeclarativeGeoLocation::setLocationId(const QString &locationId)
{
    if (m_src.locationId() != locationId) {
        m_src.setLocationId(locationId);
        emit locationIdChanged();
    }
}

QString QDeclarativeGeoLocation::locationId() const
{
    return m_src.locationId();
}

/*!
    \qmlproperty BoundingBox Location::viewport

    This property holds bouding box of area on map ocupied by location.

    Note: this property's changed() signal is currently emitted only if the
    whole element changes, not if only the contents of the element change.
*/
void QDeclarativeGeoLocation::setViewport(QDeclarativeGeoBoundingBox *viewport)
{
    if (m_src.viewport() != viewport->box()) {
        m_boundingBox.setBox(viewport->box());
        m_src.setViewport(viewport->box());
        emit viewportChanged();
    }
}

QDeclarativeGeoBoundingBox *QDeclarativeGeoLocation::viewport()
{
    return &m_boundingBox;
}
