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
    \qmlclass Location QDeclarativeGeoLocation
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-positioning
    \since QtLocation 5.0

    \brief The Location element holds location data.

    Location cointains many properties holding data of the location like geo coordinates,
    address, etc.
*/

QDeclarativeGeoLocation::QDeclarativeGeoLocation(QObject* parent)
    : QObject(parent), m_address(0), m_coordinate(0), m_boundingBox(0)

{
    setLocation(QGeoLocation());
}

QDeclarativeGeoLocation::QDeclarativeGeoLocation(const QGeoLocation &src, QObject *parent)
    : QObject(parent), m_address(0), m_coordinate(0), m_boundingBox(0)
{
    setLocation(src);
}

QDeclarativeGeoLocation::~QDeclarativeGeoLocation()
{
}

/*!
    \qmlproperty QGeoLocation Location::location

    This property is used to provide an interface between C++ and QML code.  First a pointer to a
    Location object must be obtained from C++, then use the \l {QObject::property()}{property()} and
    \l {QObject::setProperty()}{setProperty()} functions to get and set the \c location property.

    The following gets the QGeoLocation representing this object from C++:

    \snippet snippets/cpp/cppqml.cpp Location get

    The following sets the properties of this object based on a QGeoLocation object from C++:

    \snippet snippets/cpp/cppqml.cpp Location set
*/
void QDeclarativeGeoLocation::setLocation(const QGeoLocation &src)
{
    if (m_address && m_address->parent() == this) {
        m_address->setAddress(src.address());
    } else if (!m_address || m_address->parent() != this) {
        m_address = new QDeclarativeGeoAddress(src.address(), this);
        emit addressChanged();
    }

    if (m_coordinate && m_coordinate->parent() == this) {
        m_coordinate->setCoordinate(src.coordinate());
    } else if (!m_coordinate || m_coordinate->parent() != this){
        m_coordinate = new QDeclarativeCoordinate(src.coordinate(), this);
        emit coordinateChanged();
    }

    if (m_boundingBox && m_boundingBox->parent() == this) {
        m_boundingBox->setBox(src.boundingBox());
    } else if (!m_boundingBox || m_boundingBox->parent() != this){
        m_boundingBox = new QDeclarativeGeoBoundingBox(src.boundingBox(), this);
        emit boundingBoxChanged();
    }
}

QGeoLocation QDeclarativeGeoLocation::location()
{
    QGeoLocation retValue;
    retValue.setAddress(m_address ? m_address->address() : QGeoAddress());
    retValue.setCoordinate(m_coordinate ? m_coordinate->coordinate() : QGeoCoordinate());
    retValue.setBoundingBox(m_boundingBox ? m_boundingBox->box() : QGeoBoundingBox());
    return retValue;
}

/*!
    \qmlproperty string Location::address

    This property holds address of the location.
*/
void QDeclarativeGeoLocation::setAddress(QDeclarativeGeoAddress *address)
{
    if (m_address == address)
        return;

    if (m_address && m_address->parent() == this)
        delete m_address;

    m_address = address;
    emit addressChanged();
}

QDeclarativeGeoAddress *QDeclarativeGeoLocation::address()
{
    return m_address;
}

/*!
    \qmlproperty string Location::coordinate

    This property holds display coordinates of the location.

   Note: this property's changed() signal is currently emitted only if the
   whole element changes, not if only the contents of the element change.
*/
void QDeclarativeGeoLocation::setCoordinate(QDeclarativeCoordinate *coordinate)
{
    if (m_coordinate == coordinate)
        return;

    if (m_coordinate && m_coordinate->parent() == this)
        delete m_coordinate;

    m_coordinate = coordinate;
    emit coordinateChanged();
}

QDeclarativeCoordinate *QDeclarativeGeoLocation::coordinate()
{
    return m_coordinate;
}

/*!
    \qmlproperty BoundingBox Location::boundingBox

    This property holds bouding box of area on map ocupied by location.

    Note: this property's changed() signal is currently emitted only if the
    whole element changes, not if only the contents of the element change.
*/
void QDeclarativeGeoLocation::setBoundingBox(QDeclarativeGeoBoundingBox *boundingBox)
{
    if (m_boundingBox == boundingBox)
        return;

    if (m_boundingBox && m_boundingBox->parent() == this)
        delete m_boundingBox;

    m_boundingBox = boundingBox;
    emit boundingBoxChanged();
}

QDeclarativeGeoBoundingBox *QDeclarativeGeoLocation::boundingBox()
{
    return m_boundingBox;
}
