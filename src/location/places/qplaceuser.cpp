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

#include "qplaceuser.h"
#include "qplaceuser_p.h"

QT_USE_NAMESPACE

QT_DEFINE_QSDP_SPECIALIZATION_DTOR(QPlaceUserPrivate)

bool QPlaceUserPrivate::operator==(const QPlaceUserPrivate &other) const
{
    return userId == other.userId && name == other.name;
}

/*!
    \class QPlaceUser
    \inmodule QtLocation
    \ingroup QtLocation-places
    \ingroup QtLocation-places-data
    \since 5.6

    \brief The QPlaceUser class represents an individual user.
*/

/*!
    \qmlvaluetype user
    \inqmlmodule QtLocation
    \ingroup qml-QtLocation5-places
    \ingroup qml-QtLocation5-places-data
    \since QtLocation 5.5

    \brief The user type identifies a user who contributed a particular \l Place content item.

    Each \l Place content item has an associated user who contributed the content.  This type
    provides information about that user.

    \sa ImageModel, ReviewModel, EditorialModel

    \section1 Example

    The following example shows how to display information about the user who
    submitted an editorial:

    \snippet declarative/places.qml QtQuick import
    \snippet declarative/maps.qml QtLocation import
    \codeline
    \snippet declarative/places.qml EditorialModel
*/

/*!
    Constructs a new user object.
*/
QPlaceUser::QPlaceUser()
    : d(new QPlaceUserPrivate)
{
}

/*!
    Constructs a copy of \a other.
*/
QPlaceUser::QPlaceUser(const QPlaceUser &other) noexcept = default;

/*!
    Destroys the user object.
*/
QPlaceUser::~QPlaceUser() = default;

/*!
    Assigns \a other to this user and returns a reference to this user.
*/
QPlaceUser &QPlaceUser::operator=(const QPlaceUser &other) noexcept
{
    if (this == &other)
        return *this;

    d = other.d;
    return *this;
}

/*!
    \fn bool QPlaceUser::operator==(const QPlaceUser &lhs, const QPlaceUser &rhs) noexcept

    Returns true if \a lhs is equal to \a rhs, otherwise returns false.
*/

/*!
    \fn bool QPlaceUser::operator!=(const QPlaceUser &lhs, const QPlaceUser &rhs) noexcept

    Returns true if \a lhs is not equal to \a rhs, otherwise returns false.
*/

bool QPlaceUser::isEqual(const QPlaceUser &other) const noexcept
{
    return (*d) == *(other.d);
}

/*!
    \qmlproperty string QtLocation::user::userId

    This property holds the unique identifier of the user.
*/

/*!
    \property QPlaceUser::userId
    \brief the identifier of the user.
*/
QString QPlaceUser::userId() const
{
    return d->userId;
}

void QPlaceUser::setUserId(const QString &identifier)
{
    d->userId = identifier;
}

/*!
    \qmlproperty string QtLocation::User::name

    This property holds the name of a user.
*/

/*!
    \property QPlaceUser::name
    \brief the name of the user.
*/
QString QPlaceUser::name() const
{
    return d->name;
}

void QPlaceUser::setName(const QString &name)
{
    d->name = name;
}

#include "moc_qplaceuser.cpp"
