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

#include "qdeclarativeplaceattribute_p.h"

/*!
    \qmlclass ExtendedAttributes QDeclarativePropertyMap
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-places
    \since QtLocation 5.0

    \brief The ExtendedAttributes elements holds addition data about a Place.

    \sa PlaceAttribute
*/

/*!
    \qmlclass PlaceAttribute QDeclarativePlaceAttribute
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-places
    \since QtLocation 5.0

    \brief The PlaceAttribute element holds generic place attribute information.
*/
QDeclarativePlaceAttribute::QDeclarativePlaceAttribute(QObject *parent)
    : QObject(parent)
{
}

QDeclarativePlaceAttribute::QDeclarativePlaceAttribute(const QPlaceAttribute &src, QObject *parent)
    : QObject(parent),m_attribute(src)
{
}

QDeclarativePlaceAttribute::~QDeclarativePlaceAttribute()
{
}

void QDeclarativePlaceAttribute::setAttribute(const QPlaceAttribute &src)
{
    QPlaceAttribute prevAttribute = m_attribute;
    m_attribute = src;

    if (m_attribute.label() != prevAttribute.label())
        emit labelChanged();
    if (m_attribute.text() != prevAttribute.text())
        emit textChanged();
}

QPlaceAttribute QDeclarativePlaceAttribute::attribute() const
{
    return m_attribute;
}

/*!
    \qmlproperty string PlaceAttribute::label

    This property holds the attribute label which is a user visible string
    describing the attribute.
*/
void QDeclarativePlaceAttribute::setLabel(const QString &label)
{
    if (m_attribute.label()!= label) {
        m_attribute.setLabel(label);
        emit labelChanged();
    }
}

QString QDeclarativePlaceAttribute::label() const
{
    return m_attribute.label();
}

/*!
    \qmlproperty string PlaceAttribute::text

    This property holds the attribute text.
*/
void QDeclarativePlaceAttribute::setText(const QString &text)
{
    if (m_attribute.text() != text) {
        m_attribute.setText(text);
        emit  textChanged();
    }
}

QString QDeclarativePlaceAttribute::text() const
{
    return m_attribute.text();
}
