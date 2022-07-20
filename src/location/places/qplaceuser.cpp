/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
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

QPlaceUserPrivate::QPlaceUserPrivate()
    : QSharedData()
{
}

QPlaceUserPrivate::QPlaceUserPrivate(const QPlaceUserPrivate &other)
    : QSharedData(), userId(other.userId), name(other.name)
{
}

QPlaceUserPrivate::~QPlaceUserPrivate()
{
}

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
    Constructs a new user object.
*/
QPlaceUser::QPlaceUser()
    : d(new QPlaceUserPrivate)
{
}

/*!
    Constructs a copy of \a other.
*/
QPlaceUser::QPlaceUser(const QPlaceUser &other)
    :d(other.d)
{
}

/*!
    Destroys the user object.
*/
QPlaceUser::~QPlaceUser()
{
}

/*!
    Assigns \a other to this user and returns a reference to this user.
*/
QPlaceUser &QPlaceUser::operator=(const QPlaceUser &other)
{
    if (this == &other)
        return *this;

    d = other.d;
    return *this;
}

/*!
    \fn bool QPlaceUser::operator!=(const QPlaceUser &other) const

    Returns true if \a other is not equal to this user,
    otherwise returns false.
*/

/*!
    Returns true if this user is equal to \a other.
    Otherwise returns false.
*/
bool QPlaceUser::operator==(const QPlaceUser &other) const
{
    return (*d) == *(other.d);
}

/*!
    Returns the identifier of the user.
*/
QString QPlaceUser::userId() const
{
    return d->userId;
}

/*!
    Sets the \a identifier of the user.
*/
void QPlaceUser::setUserId(const QString &identifier)
{
    d->userId = identifier;
}

/*!
    Returns the name of the user.
*/
QString QPlaceUser::name() const
{
    return d->name;
}

/*!
    Sets the \a name of the user.
*/

void QPlaceUser::setName(const QString &name)
{
    d->name = name;
}
