/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtPositioning module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativegeolocation_p.h"

QT_USE_NAMESPACE

/*!
    \qmltype Location
    \inqmlmodule QtPositioning
    \since 5.2

    \brief The Location type holds location data.

    Location types represent a geographic "location", in a human sense. This
    consists of a specific \l {coordinate}, an \l {address} and a
    \l {boundingShape}{bounding shape}.
    The \l {boundingShape}{bounding shape} represents the recommended region
    to display when viewing this location.

    The Location type is most commonly seen as the contents of a search
    model such as the GeocodeModel. When a GeocodeModel returns the list of
    locations found for a given query, it represents these as Location objects.

    \section2 Example Usage

    The following example shows a simple Location object being declared:

    \code
    Location {
        coordinate {
            latitude: -27.3
            longitude: 153.1
        }
        address: Address {
            ...
        }
    }
    \endcode
*/

/*!
    \qmlproperty VariantMap QDeclarativeGeoLocation::extendedAttributes

    This property holds the extended attributes for this Location.
    Extended attributes are backend-dependent and can be location-dependent.

    \since 5.13
*/

QDeclarativeGeoLocation::QDeclarativeGeoLocation(QObject *parent)
:   QObject(parent)

{
    setLocation(QGeoLocation());
}

QDeclarativeGeoLocation::QDeclarativeGeoLocation(const QGeoLocation &src, QObject *parent)
:   QObject(parent)
{
    setLocation(src);
}

QDeclarativeGeoLocation::~QDeclarativeGeoLocation()
{
}

/*!
    \qmlproperty QGeoLocation QtPositioning::Location::location

    For details on how to use this property to interface between C++ and QML see
    "\l {Location - QGeoLocation} {Interfaces between C++ and QML Code}".

    \note This property updates the whole geo location information, so using it
    will result in breaking of all the bindings for all other properties.
*/
void QDeclarativeGeoLocation::setLocation(const QGeoLocation &src)
{
    if (m_address && m_address->parent() == this) {
        m_address->setAddress(src.address());
    } else if (!m_address || m_address->parent() != this) {
        m_address.setValue(new QDeclarativeGeoAddress(src.address(), this));
        m_address.notify();
    }

    setCoordinate(src.coordinate());
    setBoundingShape(src.boundingShape());
    setExtendedAttributes(src.extendedAttributes());
}

QGeoLocation QDeclarativeGeoLocation::location() const
{
    QGeoLocation retValue;
    retValue.setAddress(m_address ? m_address->address() : QGeoAddress());
    retValue.setCoordinate(m_coordinate);
    retValue.setBoundingShape(m_boundingShape);
    retValue.setExtendedAttributes(m_extendedAttributes);
    return retValue;
}

/*!
    \qmlproperty Address QtPositioning::Location::address

    This property holds the address of the location which can be use to retrieve address details of the location.
*/
void QDeclarativeGeoLocation::setAddress(QDeclarativeGeoAddress *address)
{
    m_address.removeBindingUnlessInWrapper();
    if (m_address == address)
        return;

    // implicitly deleting m_address.value() will force the QML bindings to
    // be reevaluated by the QML engine. So we defer the deletion of the old
    // address until a new value is assigned to the m_address.
    QDeclarativeGeoAddress *oldAddress = nullptr;
    if (m_address && m_address->parent() == this)
        oldAddress = m_address.value();

    m_address.setValueBypassingBindings(address);
    m_address.notify();
    delete oldAddress;
}

QBindable<QDeclarativeGeoAddress *> QDeclarativeGeoLocation::bindableAddress()
{
    return QBindable<QDeclarativeGeoAddress *>(&m_address);
}

QDeclarativeGeoAddress *QDeclarativeGeoLocation::address() const
{
    return m_address;
}

/*!
    \qmlproperty coordinate QtPositioning::Location::coordinate

    This property holds the exact geographical coordinate of the location which can be used to retrieve the latitude, longitude and altitude of the location.

    \note this property's changed() signal is currently emitted only if the
    whole object changes, not if only the contents of the object change.
*/
void QDeclarativeGeoLocation::setCoordinate(const QGeoCoordinate coordinate)
{
    m_coordinate = coordinate;
}

QBindable<QGeoCoordinate> QDeclarativeGeoLocation::bindableCoordinate()
{
    return QBindable<QGeoCoordinate>(&m_coordinate);
}

QGeoCoordinate QDeclarativeGeoLocation::coordinate() const
{
    return m_coordinate;
}

/*!
    \since QtPositioning 6.2

    \qmlproperty geoshape QtPositioning::Location::boundingShape

    This property holds the recommended region to use when displaying the location.
    For example, a building's location may have a region centered around the building,
    but the region is large enough to show it's immediate surrounding geographical
    context.

    \note This property's changed() signal is currently emitted only if the
    whole object changes, not if only the contents of the object change.

    \note This property was introduced in Qt6 instead of boundingBox property.
    It returns a \l geoshape instead of a \l georectangle.
    Use \l QGeoShape::boundingGeoRectangle() to obtain a bounding
    \l georectangle for the shape.

    If you need to convert the returned shape to a specific type, use the
    \c type property of \l geoshape together with convenience
    methods from \l [QML]{QtPositioning} like
    \l {QtPositioning::shapeToRectangle}{QtPositioning.shapeToRectangle()}.
*/
void QDeclarativeGeoLocation::setBoundingShape(const QGeoShape &boundingShape)
{
    m_boundingShape = boundingShape;
}

QBindable<QGeoShape> QDeclarativeGeoLocation::bindableBoundingShape()
{
    return QBindable<QGeoShape>(&m_boundingShape);
}

QVariantMap QDeclarativeGeoLocation::extendedAttributes() const
{
    return m_extendedAttributes;
}

void QDeclarativeGeoLocation::setExtendedAttributes(const QVariantMap &attributes)
{
    m_extendedAttributes = attributes;
}

QBindable<QVariantMap> QDeclarativeGeoLocation::bindableExtendedAttributes()
{
    return QBindable<QVariantMap>(&m_extendedAttributes);
}

QGeoShape QDeclarativeGeoLocation::boundingShape() const
{
    return m_boundingShape;
}
