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

#include "qplacemediaobject.h"
#include "qplacemediaobject_p.h"

QT_USE_NAMESPACE

QPlaceMediaObjectPrivate::QPlaceMediaObjectPrivate() : QSharedData()
{
}

QPlaceMediaObjectPrivate::QPlaceMediaObjectPrivate(const QPlaceMediaObjectPrivate &other)
    : QSharedData()
{
    this->url = other.url;
    this->thumbnailUrl = other.thumbnailUrl;
    this->id = other.id;
    this->metaInfo = other.metaInfo;
    this->mimeType = other.mimeType;
    this->supplier = other.supplier;
}

QPlaceMediaObjectPrivate::~QPlaceMediaObjectPrivate()
{
}

bool QPlaceMediaObjectPrivate::operator==(const QPlaceMediaObjectPrivate &other) const
{
    return (
            this->url == other.url
            && this->thumbnailUrl == other.thumbnailUrl
            && this->id == other.id
            && this->metaInfo == other.metaInfo
            && this->mimeType == other.mimeType
            && this->supplier == other.supplier
    );
}

/*!
    \class QPlaceMediaObject

    \inmodule QPlaces

    \brief The QPlaceMediaObject class represents a media object.


    Each QPlaceMediaObject represents a media object with a number of attributes
    such as type, thumbnail, media provider etc. Each QPlaceMediaObject may be associated
    with place.

    Media objects are read-only, e.g. user of API might get list of media objects
    associated to specific place but can not edit its content.

    QPlaceMediaObject is an in memory representation of a media object.
*/

/*!
   \variable QPlaceMediaObject::Image
   The constant used to define the type of media.
*/
Q_DEFINE_LATIN1_CONSTANT(QPlaceMediaObject::Image, "Image");

/*!
    Constructs an new media object.
*/
QPlaceMediaObject::QPlaceMediaObject()
    : d(new QPlaceMediaObjectPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceMediaObject::QPlaceMediaObject(const QPlaceMediaObject &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceMediaObject::~QPlaceMediaObject()
{
}

QPlaceMediaObject &QPlaceMediaObject::operator =(const QPlaceMediaObject &other) {
    d = other.d;
    return *this;
}

bool QPlaceMediaObject::operator==(const QPlaceMediaObject &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns media url.
*/
QUrl QPlaceMediaObject::url() const
{
    return d->url;
}

/*!
    Sets media url.
*/
void QPlaceMediaObject::setUrl(const QUrl &url)
{
    d->url = url;
}

/*!
    Returns media url for thumbnail.
*/
QUrl QPlaceMediaObject::thumbnailUrl() const
{
    return d->thumbnailUrl;
}

/*!
    Sets media url for thumbnail.
*/
void QPlaceMediaObject::setThumbnailUrl(const QUrl &url)
{
    d->thumbnailUrl = url;
}

/*!
    Returns media id.
*/
QString QPlaceMediaObject::id() const
{
    return d->id;
}

/*!
    Sets media id.
*/
void QPlaceMediaObject::setId(const QString &data)
{
    d->id = data;
}

/*!
    Returns media meta info.
*/
QString QPlaceMediaObject::metaInfo() const
{
    return d->metaInfo;
}

/*!
    Sets media meta info.
*/
void QPlaceMediaObject::setMetaInfo(const QString &data)
{
    d->metaInfo = data;
}

/*!
    Returns media mime type.
*/
QString QPlaceMediaObject::mimeType() const
{
    return d->mimeType;
}

/*!
    Sets media mime type.
*/
void QPlaceMediaObject::setMimeType(const QString &data)
{
    d->mimeType = data;
}

/*!
    Returns supplier.
*/
QPlaceSupplier QPlaceMediaObject::supplier() const
{
    return d->supplier;
}

/*!
    Sets supplier.
*/
void QPlaceMediaObject::setSupplier(const QPlaceSupplier &data)
{
    d->supplier = data;
}
