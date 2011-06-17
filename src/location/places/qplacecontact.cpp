/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#include "qplacecontact.h"
#include "qplacecontact_p.h"

QTM_USE_NAMESPACE

QPlaceContactPrivate::QPlaceContactPrivate() : QSharedData()
{
    this->type = QPlaceContact::Undefined;
}

QPlaceContactPrivate::QPlaceContactPrivate(const QPlaceContactPrivate &other)
    : QSharedData()
{
    this->description = other.description;
    this->type = other.type;
    this->value = other.value;
}

QPlaceContactPrivate::~QPlaceContactPrivate()
{
}

bool QPlaceContactPrivate::operator==(const QPlaceContactPrivate &other) const
{
    return (
            this->description == other.description
            && this->type == other.type
            && this->value == other.value
    );
}

/*!
    \class QPlaceContact

    \inmodule QPlaces

    \brief The QPlaceContact class represents a contact object.

    Each QPlaceContact represents a contact object with a number of attributes
    such as type, data etc. Each QPlaceContact is associated with place.

    Contact objects are read-only, e.g. user of API might get contact object
    associated to specific place but can not edit its content. User might also create new
    contact object and add it to place.

    QPlaceContact is an in memory representation of a contact object.
*/

/*!
    Constructs an new contact object.
*/
QPlaceContact::QPlaceContact()
    : d(new QPlaceContactPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceContact::QPlaceContact(const QPlaceContact &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceContact::~QPlaceContact()
{
}

QPlaceContact &QPlaceContact::operator =(const QPlaceContact &other) {
    d = other.d;
    return *this;
}

bool QPlaceContact::operator==(const QPlaceContact &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns description.
*/
QString QPlaceContact::description() const
{
    return d->description;
}

/*!
    Sets description.
*/
void QPlaceContact::setDescription(const QString &data)
{
    d->description = data;
}

/*!
    Returns type.
*/
QPlaceContact::ContactType QPlaceContact::type() const
{
    return d->type;
}

/*!
    Sets type.
*/
void QPlaceContact::setType(const QPlaceContact::ContactType &data)
{
    d->type = data;
}

/*!
    Returns value.
*/
QString QPlaceContact::value() const
{
    return d->value;
}

/*!
    Sets value.
*/
void QPlaceContact::setValue(const QString &data)
{
    d->value = data;
}
