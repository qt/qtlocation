/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: http://www.qt-project.org/
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

#include "qdeclarativeplaceuser_p.h"

QT_USE_NAMESPACE

/*!
    \qmlclass User QDeclarativePlaceUser
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-places
    \since QtLocation 5.0

    \brief The User element identifies a user who contributed a particular \l Place content item.

    Each \l Place content item has an associated user who contributed the content.  This element
    provides information about that user.

    \sa ImageModel, ReviewModel, EditorialModel
*/

QDeclarativePlaceUser::QDeclarativePlaceUser(QObject* parent)
        : QObject(parent) {}

QDeclarativePlaceUser::QDeclarativePlaceUser(const QPlaceUser &user,
        QObject *parent)
        : QObject(parent),
        m_user(user) {}

QDeclarativePlaceUser::~QDeclarativePlaceUser() {}

/*!
    \qmlproperty QPlaceUser User::user

    This property is used to provide an interface between C++ and QML code.  First a pointer to a
    User object must be obtained from C++, then use the \l {QObject::property()}{property()} and
    \l {QObject::setProperty()}{setProperty()} functions to get and set the \c user property.

    The following gets the QPlaceUser representing this object from C++:

    \snippet snippets/cpp/cppqml.cpp User get

    The following sets the properties of this object based on a QPlaceUser object from C++:

    \snippet snippets/cpp/cppqml.cpp User set
*/
void QDeclarativePlaceUser::setUser(const QPlaceUser &user)
{
    QPlaceUser previousUser = m_user;
    m_user = user;

    if (m_user.userId() != previousUser.userId())
        emit userIdChanged();

    if (m_user.name() != previousUser.name())
        emit nameChanged();
}

QPlaceUser QDeclarativePlaceUser::user() const
{
    return m_user;
}

/*!
    \qmlproperty string User::userId

    This property holds the id of the user.
*/

void QDeclarativePlaceUser::setUserId(const QString &id)
{
    if (m_user.userId() == id)
        return;

    m_user.setUserId(id);
    emit userIdChanged();
}

QString QDeclarativePlaceUser::userId() const
{
    return m_user.userId();
}

/*!
    \qmlproperty string User::name

    This property holds the name of a user.
*/
void QDeclarativePlaceUser::setName(const QString &name)
{
    if (m_user.name() == name)
        return;

    m_user.setName(name);
    emit nameChanged();
}

QString QDeclarativePlaceUser::name() const
{
    return m_user.name();
}

