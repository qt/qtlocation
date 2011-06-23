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

#include "qplacedescription.h"
#include "qplacedescription_p.h"

QTM_USE_NAMESPACE

QPlaceDescriptionPrivate::QPlaceDescriptionPrivate() : QSharedData()
{
}

QPlaceDescriptionPrivate::QPlaceDescriptionPrivate(const QPlaceDescriptionPrivate &other)
    : QSharedData()
{
    this->content = other.content;
    this->contentTitle = other.contentTitle;
    this->contentType = other.contentType;
    this->supplier = other.supplier;
    this->sourceUrl = other.sourceUrl;
    this->language = other.language;
}

QPlaceDescriptionPrivate::~QPlaceDescriptionPrivate()
{
}

bool QPlaceDescriptionPrivate::operator==(const QPlaceDescriptionPrivate &other) const
{
    return (
            this->content == other.content
            && this->contentTitle == other.contentTitle
            && this->contentType == other.contentType
            && this->supplier == other.supplier
            && this->sourceUrl == other.sourceUrl
            && this->language == other.language
    );
}

/*!
    \class QPlaceDescription

    \inmodule QPlaces

    \brief The QDescription class represents a address object.

    Each QPlaceDescription represents a description object with a number of attributes
    such as title, value etc. Each QPlaceDescription is associated with place.

    Description objects are read-only, e.g. user of API might get description objects
    associated to specific place but can not edit its content. User might also create new description.

*/

/*!
    Constructs an new place description object.
*/
QPlaceDescription::QPlaceDescription()
    : d(new QPlaceDescriptionPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceDescription::QPlaceDescription(const QPlaceDescription &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceDescription::~QPlaceDescription()
{
}

QPlaceDescription &QPlaceDescription::operator =(const QPlaceDescription &other) {
    d = other.d;
    return *this;
}

bool QPlaceDescription::operator==(const QPlaceDescription &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns content.
*/
QString QPlaceDescription::content() const
{
    return d->content;
}

/*!
    Sets content.
*/
void QPlaceDescription::setContent(const QString &data)
{
    d->content = data;
}

/*!
    Returns content title.
*/
QString QPlaceDescription::contentTitle() const
{
    return d->contentTitle;
}

/*!
    Sets content title.
*/
void QPlaceDescription::setContentTitle(const QString &data)
{
    d->contentTitle = data;
}

/*!
    Returns content type.
*/
QString QPlaceDescription::contentType() const
{
    return d->contentType;
}

/*!
    Sets content type.
*/
void QPlaceDescription::setContentType(const QString &data)
{
    d->contentType = data;
}

/*!
    Returns supplier. Do not remove it.
*/
QPlaceSupplier QPlaceDescription::supplier() const
{
    return d->supplier;
}

/*!
    Sets supplier.
*/
void QPlaceDescription::setSupplier(const QPlaceSupplier &data)
{
    d->supplier = data;
}

/*!
    Returns source url.
*/
QString QPlaceDescription::sourceUrl() const
{
    return d->sourceUrl;
}

/*!
    Sets source url.
*/
void QPlaceDescription::setSourceUrl(const QString &data)
{
    d->sourceUrl = data;
}

/*!
    Returns language.
*/
QString QPlaceDescription::language() const
{
    return d->language;
}

/*!
    Sets language.
*/
void QPlaceDescription::setLanguage(const QString &data)
{
    d->language = data;
}
