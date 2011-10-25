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

#include "qplacecontactdetail_p.h"
#include "qplacecontactdetail.h"

QT_USE_NAMESPACE

QPlaceContactDetailPrivate::QPlaceContactDetailPrivate(const QPlaceContactDetailPrivate &other)
    : QSharedData(other),
      label(other.label),
      value(other.value)
{
}

bool QPlaceContactDetailPrivate::operator== (const QPlaceContactDetailPrivate &other) const
{
    return label == other.label
            && value == other.value;
}

/*!
\class QPlaceContactDetail
\brief The QPlaceContactDetail class represents a contact detail such as a phone number or url.
\inmodule QtLocation

\ingroup QtLocation-places

*/

/*!
   \variable QPlaceContactDetail::Phone
   The constant to specify phone contact details
*/
Q_DEFINE_LATIN1_CONSTANT(QPlaceContactDetail::Phone, "Phone");

/*!
   \variable QPlaceContactDetail::Email
   The constant to specify email contact contact details.
*/
Q_DEFINE_LATIN1_CONSTANT(QPlaceContactDetail::Email, "Email");

/*!
   \variable QPlaceContactDetail::Website
   The constant used to specify website contact details.
*/
Q_DEFINE_LATIN1_CONSTANT(QPlaceContactDetail::Website, "Website");

/*!
   \variable QPlaceContactDetail::Fax
   The constant used to specify fax contact details.
*/
Q_DEFINE_LATIN1_CONSTANT(QPlaceContactDetail::Fax, "Fax");

/*!
    Constructs an contact detail.
*/
QPlaceContactDetail::QPlaceContactDetail()
    : d_ptr(new QPlaceContactDetailPrivate)
{
}

/*!
    Destroys the contact detail.
*/
QPlaceContactDetail::~QPlaceContactDetail()
{
}

/*!
    Creates a copy of \a other.
*/
QPlaceContactDetail::QPlaceContactDetail(const QPlaceContactDetail &other)
    :d_ptr(other.d_ptr)
{
}

/*!
    Assigns \a other to this contact detail and returns a reference to this
    contact detail.
*/
QPlaceContactDetail &QPlaceContactDetail::operator=(const QPlaceContactDetail &other)
{
    d_ptr = other.d_ptr;
    return *this;
}

/*!
    Returns true if \a other is equal to this contact detail, otherwise
    returns false.
*/
bool QPlaceContactDetail::operator== (const QPlaceContactDetail &other) const
{
    if (d_ptr == other.d_ptr)
        return true;
    return ( *(d_ptr.constData()) == *(other.d_ptr.constData()));
}

/*!
    Returns true if \a other is not equal to this contact detail,
    otherwise returns false.
*/
bool QPlaceContactDetail::operator!= (const QPlaceContactDetail &other) const
{
    return (!this->operator ==(other));
}

/*!
    Returns a localized label describing the contact detail.
*/
QString QPlaceContactDetail::label() const
{
    return d_ptr->label;
}

/*!
    Sets the \a label of the contact detail.
*/
void QPlaceContactDetail::setLabel(const QString &label)
{
    d_ptr->label = label;
}

/*!
    Returns the value of the contact detail.
*/
QString QPlaceContactDetail::value() const
{
    return d_ptr->value;
}

/*!
    Sets the \a value of the this contact detail.
*/
void QPlaceContactDetail::setValue(const QString &value)
{
    d_ptr->value = value;
}
