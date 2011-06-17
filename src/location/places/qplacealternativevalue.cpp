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

#include "qplacealternativevalue.h"
#include "qplacealternativevalue_p.h"

QTM_USE_NAMESPACE

QPlaceAlternativeValuePrivate::QPlaceAlternativeValuePrivate()
    : QSharedData(),
    semantics(QPlaceAlternativeValue::Unclassified),
    type(QPlaceAlternativeValue::Unknown)
{
}

QPlaceAlternativeValuePrivate::QPlaceAlternativeValuePrivate(const QPlaceAlternativeValuePrivate &other)
    : QSharedData()
{
    this->key = other.key;
    this->language = other.language;
    this->semantics = other.semantics;
    this->type = other.type;
    this->value = other.value;
}

QPlaceAlternativeValuePrivate::~QPlaceAlternativeValuePrivate()
{
}

bool QPlaceAlternativeValuePrivate::operator==(const QPlaceAlternativeValuePrivate &other) const
{
    return (
            this->key == other.key
            && this->language == other.language
            && this->semantics == other.semantics
            && this->type == other.type
            && this->value == other.value
    );
}

/*!
    \class QPlaceAlternativeValue

    \inmodule QPlaces

    \brief The QPlaceAlternativeValue class represents a alternative value object.

    Each QPlaceAlternativeValue represents a alternative value object with a number of attributes
    such as value, language id etc. Each QPlaceAlternativeValue is associated with place.

    Alternative value objects are read-only, e.g. user of API might get alternative value object
    associated to specific place but can not edit its content. User might also create new
    alternative value object and add it to place.

    QPlaceAlternativeValue is an in memory representation of a alternative value object.
*/

/*!
    Constructs an new alternative value object.
*/
QPlaceAlternativeValue::QPlaceAlternativeValue()
    : d(new QPlaceAlternativeValuePrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceAlternativeValue::QPlaceAlternativeValue(const QPlaceAlternativeValue &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceAlternativeValue::~QPlaceAlternativeValue()
{
}

QPlaceAlternativeValue &QPlaceAlternativeValue::operator =(const QPlaceAlternativeValue &other) {
    d = other.d;
    return *this;
}

bool QPlaceAlternativeValue::operator==(const QPlaceAlternativeValue &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns key.
*/
QString QPlaceAlternativeValue::key() const
{
    return d->key;
}

/*!
    Sets key.
*/
void QPlaceAlternativeValue::setKey(const QString &data)
{
    d->key = data;
}

/*!
    Returns language.
*/
QString QPlaceAlternativeValue::language() const
{
    return d->language;
}

/*!
    Sets language.
*/
void QPlaceAlternativeValue::setLanguage(const QString &data)
{
    d->language = data;
}

/*!
    Returns semantics.
*/
QPlaceAlternativeValue::TextSemantics QPlaceAlternativeValue::semantics() const
{
    return d->semantics;
}

/*!
    Sets semantics.
*/
void QPlaceAlternativeValue::setSemantics(const QPlaceAlternativeValue::TextSemantics &data)
{
    d->semantics = data;
}

/*!
    Returns type.
*/
QPlaceAlternativeValue::NameType QPlaceAlternativeValue::type() const
{
    return d->type;
}

/*!
    Sets type.
*/
void QPlaceAlternativeValue::setType(const QPlaceAlternativeValue::NameType &data)
{
    d->type = data;
}

/*!
    Returns value.
*/
QString QPlaceAlternativeValue::value() const
{
    return d->value;
}

/*!
    Sets value.
*/
void QPlaceAlternativeValue::setValue(const QString &data)
{
    d->value = data;
}
