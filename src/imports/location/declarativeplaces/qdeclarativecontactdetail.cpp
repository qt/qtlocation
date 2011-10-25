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

#include "qdeclarativecontactdetail_p.h"

/*!
    \qmlclass ContactDetail QDeclarativeContactDetail

    \inqmlmodule QtLocation 5

    \ingroup qml-QtLocation5-places

    \since QtLocation 5.0

    \brief The ContactDetail element holds a contact detail such as phone number or website address.
*/
QDeclarativeContactDetail::QDeclarativeContactDetail(QObject *parent)
    : QObject(parent)
{
}

QDeclarativeContactDetail::QDeclarativeContactDetail(const QPlaceContactDetail &src, QObject *parent)
    : QObject(parent), m_contactDetail(src)
{
}

QDeclarativeContactDetail::~QDeclarativeContactDetail()
{
}

void QDeclarativeContactDetail::setContactDetail(const QPlaceContactDetail &src)
{
    QPlaceContactDetail prevContactDetail = m_contactDetail;
    m_contactDetail = src;

    if (m_contactDetail.label() != prevContactDetail.label())
        emit labelChanged();
    if (m_contactDetail.value() != prevContactDetail.value())
        emit valueChanged();
}

QPlaceContactDetail QDeclarativeContactDetail::contactDetail() const
{
    return m_contactDetail;
}

/*!
    \qmlproperty string ContactDetail::label

    This property holds a localized label describing the contact detail.
*/
QString QDeclarativeContactDetail::label() const
{
    return m_contactDetail.label();
}

void QDeclarativeContactDetail::setLabel(const QString &label)
{
    if (m_contactDetail.label()!= label) {
        m_contactDetail.setLabel(label);
        emit labelChanged();
    }
}

/*!
    \qmlproperty string ContactDetail::value

    This property holds the value of the contact detail which
    may be a phone number, email address, website url etc.
*/
QString QDeclarativeContactDetail::value() const
{
    return m_contactDetail.value();
}

void QDeclarativeContactDetail::setValue(const QString &value)
{
    if (m_contactDetail.value()!= value) {
        m_contactDetail.setValue(value);
        emit valueChanged();
    }
}
