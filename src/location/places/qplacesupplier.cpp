/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
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

#include "qplacesupplier.h"
#include "qplacesupplier_p.h"

QT_USE_NAMESPACE

QT_DEFINE_QSDP_SPECIALIZATION_DTOR(QPlaceSupplierPrivate)

bool QPlaceSupplierPrivate::operator==(const QPlaceSupplierPrivate &other) const
{
    return (
            this->name == other.name
            && this->supplierId == other.supplierId
            && this->url == other.url
            && this->icon == other.icon
    );
}

bool QPlaceSupplierPrivate::isEmpty() const
{
    return (name.isEmpty()
            && supplierId.isEmpty()
            && url.isEmpty()
            && icon.isEmpty()
            );
}

/*!
    \class QPlaceSupplier
    \inmodule QtLocation
    \ingroup QtLocation-places
    \ingroup QtLocation-places-data
    \since 5.6

    \brief The QPlaceSupplier class represents a supplier of a place or content associated
    with a place.

    Each instance represents a set of data about a supplier, which can include
    supplier's name, url and icon.  The supplier is typically a business or organization.

    Note: The Places API only supports suppliers as 'retrieve-only' objects.  Submitting
    suppliers to a provider is not a supported use case.
*/

/*!
    \qmlvaluetype supplier
    \inqmlmodule QtLocation
    \ingroup qml-QtLocation5-places
    \ingroup qml-QtLocation5-places-data
    \since QtLocation 5.5

    \brief Holds data regarding the supplier of a place, a place's image, review, or editorial.

    Each instance represents a set of data about a supplier, which can include
    supplier's name, url and icon.  The supplier is typically a business or organization.

    Note: The Places API only supports suppliers as 'retrieve-only' objects.  Submitting
    suppliers to a provider is not a supported use case.

    \sa ImageModel, ReviewModel, EditorialModel

    \section1 Example

    The following example shows how to create and display a supplier in QML:

    \snippet declarative/places.qml QtQuick import
    \snippet declarative/maps.qml QtLocation import
    \codeline
    \snippet declarative/places.qml Supplier
*/

/*!
    Constructs a new supplier object.
*/
QPlaceSupplier::QPlaceSupplier()
    : d(new QPlaceSupplierPrivate)
{
}

/*!
    Constructs a copy of \a other.
*/
QPlaceSupplier::QPlaceSupplier(const QPlaceSupplier &other) noexcept = default;

/*!
    Destroys the supplier object.
*/
QPlaceSupplier::~QPlaceSupplier() = default;

/*!
    Assigns \a other to this supplier and returns a reference to this
    supplier.
*/
QPlaceSupplier &QPlaceSupplier::operator=(const QPlaceSupplier &other) noexcept
{
    if (this == &other)
        return *this;

    d = other.d;
    return *this;
}

/*!
    \fn bool QPlaceSupplier::operator==(const QPlaceSupplier &lhs, const QPlaceSupplier &rhs) noexcept

    Returns true if \a lhs is equal to \a rhs, otherwise returns false.
*/

/*!
    \fn bool QPlaceSupplier::operator!=(const QPlaceSupplier &lhs, const QPlaceSupplier &rhs) noexcept

    Returns true if \a lhs is not equal to \a rhs, otherwise returns false.
*/

bool QPlaceSupplier::isEqual(const QPlaceSupplier &other) const noexcept
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    \qmlproperty string Supplier::name

    This property holds the name of the supplier which can be displayed
    to the user.

    The name can potentially be localized.  The language is dependent on the
    entity that sets it, typically this is the \l Plugin.  The \l {Plugin::locales}
    property defines what language is used.
*/

/*!
    \property QPlaceSupplier::name
    \brief the name of the supplier which can be displayed to the user.

    The name can potentially be localized. The language is dependent on the
    entity that sets it, typically this is the QPlaceManager.
    The QPlaceManager::locales() field defines what language is used.
*/
QString QPlaceSupplier::name() const
{
    return d->name;
}

/*!
    Sets the \a name of the supplier.
*/
void QPlaceSupplier::setName(const QString &name)
{
    d->name = name;
}

/*!
    \qmlproperty string Supplier::supplierId

    This property holds the identifier of the supplier.  The identifier is unique
    to the Plugin backend which provided the supplier and is generally
    not suitable for displaying to the user.
*/

/*!
    \property QPlaceSupplier::supplierId
    \brief the identifier of the supplier.

    The identifier is unique to the manager backend which provided the supplier
    and is generally not suitable for displaying to the user.
*/
QString QPlaceSupplier::supplierId() const
{
    return d->supplierId;
}

/*!
    Sets the \a identifier of the supplier.
*/
void QPlaceSupplier::setSupplierId(const QString &identifier)
{
    d->supplierId = identifier;
}

/*!
    \qmlproperty url Supplier::url

    This property holds the URL of the supplier's website.
*/

/*!
    \property QPlaceSupplier::url
    \brief the URL of the supplier's website.
*/
QUrl QPlaceSupplier::url() const
{
    return d->url;
}

/*!
    Sets the \a url of the supplier's website.
*/
void QPlaceSupplier::setUrl(const QUrl &url)
{
    d->url = url;
}

/*!
    \qmlproperty PlaceIcon Supplier::icon

    This property holds the icon of the supplier.
*/

/*!
    \property QPlaceSupplier::icon
    \brief the icon of the supplier.
*/
QPlaceIcon QPlaceSupplier::icon() const
{
    return d->icon;
}

/*!
    Sets the \a icon of the supplier.
*/
void QPlaceSupplier::setIcon(const QPlaceIcon &icon)
{
    d->icon = icon;
}

/*!
    Returns true if all fields of the place supplier are 0; otherwise returns false.
*/
bool QPlaceSupplier::isEmpty() const
{
    return d->isEmpty();
}

#include "moc_qplacesupplier.cpp"
