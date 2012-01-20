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

#include "qdeclarativebusinessfeature_p.h"

QT_USE_NAMESPACE

/*!
    \qmlclass BusinessFeature

    \brief The BusinessFeature element holds business feature information.
    \inherits QObject

    \ingroup qml-places
*/

QDeclarativeBusinessFeature::QDeclarativeBusinessFeature(QObject* parent)
        : QObject(parent)
{
}

QDeclarativeBusinessFeature::QDeclarativeBusinessFeature(const QPlaceBusinessFeature &value,
        QObject *parent)
        : QObject(parent),
          m_value(value)
{
}

QDeclarativeBusinessFeature::~QDeclarativeBusinessFeature()
{
}

void QDeclarativeBusinessFeature::setBusinessFeature(const QPlaceBusinessFeature &value)
{
    QPlaceBusinessFeature previous = m_value;
    m_value = value;

    if (previous.key() != m_value.key()) {
        emit keyChanged();
    }
    if (previous.label() != m_value.label()) {
        emit labelChanged();
    }
    if (previous.value() != m_value.value()) {
        emit valueChanged();
    }
}

QPlaceBusinessFeature QDeclarativeBusinessFeature::businessFeature() const
{
    return m_value;
}

/*!
    \qmlproperty string BusinessFeature::key

    This property holds key description.
*/

void QDeclarativeBusinessFeature::setKey(const QString &key)
{
    if (m_value.key() != key) {
        m_value.setKey(key);
        emit keyChanged();
    }
}

QString QDeclarativeBusinessFeature::key() const
{
    return m_value.key();
}

/*!
    \qmlproperty string BusinessFeature::label

    This property holds label.
*/

void QDeclarativeBusinessFeature::setLabel(const QString &label)
{
    if (m_value.label() != label) {
        m_value.setLabel(label);
        emit labelChanged();
    }
}

QString QDeclarativeBusinessFeature::label() const
{
    return m_value.label();
}

/*!
    \qmlproperty string BusinessFeature::value

    This property holds value.
*/

void QDeclarativeBusinessFeature::setValue(const QString &value)
{
    if (m_value.value() != value) {
        m_value.setValue(value);
        emit valueChanged();
    }
}

QString QDeclarativeBusinessFeature::value() const
{
    return m_value.value();
}
