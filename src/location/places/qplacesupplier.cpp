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

#include "qplacesupplier.h"
#include "qplacesupplier_p.h"

QT_USE_NAMESPACE

QPlaceSupplierPrivate::QPlaceSupplierPrivate() : QSharedData()
{
}

QPlaceSupplierPrivate::QPlaceSupplierPrivate(const QPlaceSupplierPrivate &other)
    : QSharedData()
{
    this->name = other.name;
    this->supplierId = other.supplierId;
    this->url = other.url;
    this->icon = other.icon;
}

QPlaceSupplierPrivate::~QPlaceSupplierPrivate()
{
}

bool QPlaceSupplierPrivate::operator==(const QPlaceSupplierPrivate &other) const
{
    return (
            this->name == other.name
            && this->supplierId == other.supplierId
            && this->url == other.url
            && this->icon == other.icon
    );
}

/*!
    \class QPlaceSupplier
    \inmodule QtLocation
    \ingroup QtLocation-places
    \ingroup QtLocation-places-data
    \since QtLocation 5.0

    \brief The QPlaceSupplier class represents a supplier of a place or content associated
    with a place.

    Each instance represents a set of data about a supplier, which can include
    supplier's name, url and icon.  The supplier is typically a business or organization.

    Note: The Places API only supports suppliers as 'retrieve-only' objects.  Submitting
    suppliers to a provider is not a supported use case.
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
QPlaceSupplier::QPlaceSupplier(const QPlaceSupplier &other)
    :d(other.d)
{
}

/*!
    Destroys the supplier object.
*/
QPlaceSupplier::~QPlaceSupplier()
{
}

/*!
    Assigns \a other to this supplier and returns a reference to this
    supplier.
*/
QPlaceSupplier &QPlaceSupplier::operator=(const QPlaceSupplier &other) {
    d = other.d;
    return *this;
}

/*!
    Returns true if this supplier is equal to \a other,
    otherwise returns false.
*/
bool QPlaceSupplier::operator==(const QPlaceSupplier &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    \fn QPlaceSupplier::operator!=(const QPlaceSupplier &other) const

    Returns true if this supplier is not equal to \a other,
    otherwise returns false.
*/

/*!
    Returns the name of the supplier.
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
    Returns the id of the supplier.
*/
QString QPlaceSupplier::supplierId() const
{
    return d->supplierId;
}

/*!
    Sets the \a id of the supplier.
*/
void QPlaceSupplier::setSupplierId(const QString &id)
{
    d->supplierId = id;
}

/*!
    Returns the url of the supplier.
*/
QUrl QPlaceSupplier::url() const
{
    return d->url;
}

/*!
    Sets the \a url of the supplier.
*/
void QPlaceSupplier::setUrl(const QUrl &url)
{
    d->url = url;
}

/*!
    Returns the icon of the supplier.
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
