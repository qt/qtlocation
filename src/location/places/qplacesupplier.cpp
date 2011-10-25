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
    \since QtLocation 5.0

    \brief The QPlaceSupplier class represents a supplier object.

    Each QPlaceSupplier represents a supplier object with a number of attributes
    such as name, icon etc. Each QPlaceSupplier is associated with place, media, review or description.
*/

/*!
    Default constructor. Constructs an new supplier object.
*/
QPlaceSupplier::QPlaceSupplier()
    : d(new QPlaceSupplierPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceSupplier::QPlaceSupplier(const QPlaceSupplier &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceSupplier::~QPlaceSupplier()
{
}

QPlaceSupplier &QPlaceSupplier::operator =(const QPlaceSupplier &other) {
    d = other.d;
    return *this;
}

bool QPlaceSupplier::operator==(const QPlaceSupplier &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns name.
*/
QString QPlaceSupplier::name() const
{
    return d->name;
}

/*!
    Sets name.
*/
void QPlaceSupplier::setName(const QString &data)
{
    d->name = data;
}

/*!
    Returns supplier id.
*/
QString QPlaceSupplier::supplierId() const
{
    return d->supplierId;
}

/*!
    Sets supplier id.
*/
void QPlaceSupplier::setSupplierId(const QString &data)
{
    d->supplierId = data;
}

/*!
    Returns the URL of the supplier.
*/
QUrl QPlaceSupplier::url() const
{
    return d->url;
}

/*!
    Sets \a url of the supplier.
*/
void QPlaceSupplier::setUrl(const QUrl &url)
{
    d->url = url;
}

/*!
    Returns the supplier icon
*/
QPlaceIcon QPlaceSupplier::icon() const
{
    return d->icon;
}

/*!
    Sets the supplier icon
*/
void QPlaceSupplier::setIcon(const QPlaceIcon &icon)
{
    d->icon = icon;
}
