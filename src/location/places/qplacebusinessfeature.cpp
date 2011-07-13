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

#include "qplacebusinessfeature.h"
#include "qplacebusinessfeature_p.h"

QT_USE_NAMESPACE

QPlaceBusinessFeaturePrivate::QPlaceBusinessFeaturePrivate() : QSharedData()
{
}

QPlaceBusinessFeaturePrivate::QPlaceBusinessFeaturePrivate(const QPlaceBusinessFeaturePrivate &other)
    : QSharedData()
{
    this->value = other.value;
    this->key = other.key;
    this->label = other.label;
}

QPlaceBusinessFeaturePrivate::~QPlaceBusinessFeaturePrivate()
{
}

bool QPlaceBusinessFeaturePrivate::operator==(const QPlaceBusinessFeaturePrivate &other) const
{
    return (
            this->value == other.value
            && this->key == other.key
            && this->label == other.label
    );
}

/*!
    \class QPlaceBusinessFeature

    \inmodule QPlaces

    \brief The QPlaceBusinessFeature class represents a business services object.

    Each QPlaceBusinessFeature represents a business service object with a number of attributes
    such as key etc.
*/


/*!
    Constructs an new "empty" business feature object.
*/
QPlaceBusinessFeature::QPlaceBusinessFeature()
    : d(new QPlaceBusinessFeaturePrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceBusinessFeature::QPlaceBusinessFeature(const QPlaceBusinessFeature &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceBusinessFeature::~QPlaceBusinessFeature()
{
}

QPlaceBusinessFeature &QPlaceBusinessFeature::operator =(const QPlaceBusinessFeature &other) {
    d = other.d;
    return *this;
}

bool QPlaceBusinessFeature::operator==(const QPlaceBusinessFeature &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns key.
*/
QString QPlaceBusinessFeature::key() const
{
    return d->key;
}

/*!
    Sets key.
*/
void QPlaceBusinessFeature::setKey(const QString &data)
{
    d->key = data;
}

/*!
    Returns label.
*/
QString QPlaceBusinessFeature::label() const
{
    return d->label;
}

/*!
    Sets label.
*/
void QPlaceBusinessFeature::setLabel(const QString &data)
{
    d->label = data;
}

/*!
    Returns value.
*/
QString QPlaceBusinessFeature::value() const
{
    return d->value;
}

/*!
    Sets value.
*/
void QPlaceBusinessFeature::setValue(const QString &data)
{
    d->value = data;
}
