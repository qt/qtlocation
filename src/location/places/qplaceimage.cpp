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

#include "qplaceimage.h"
#include "qplaceimage_p.h"

QT_USE_NAMESPACE

QPlaceImagePrivate::QPlaceImagePrivate() : QPlaceContentPrivate()
{
}

QPlaceImagePrivate::QPlaceImagePrivate(const QPlaceImagePrivate &other)
    : QPlaceContentPrivate(other)
{
    this->url = other.url;
    this->thumbnailUrl = other.thumbnailUrl;
    this->id = other.id;
    this->metaInfo = other.metaInfo;
    this->mimeType = other.mimeType;
    this->supplier = other.supplier;
}

QPlaceImagePrivate::~QPlaceImagePrivate()
{
}

bool QPlaceImagePrivate::compare(const QPlaceContentPrivate *other) const
{
    const QPlaceImagePrivate *od = static_cast<const QPlaceImagePrivate *>(other);
    return (this->url == od->url
            && this->thumbnailUrl == od->thumbnailUrl
            && this->id == od->id
            && this->metaInfo == od->metaInfo
            && this->mimeType == od->mimeType
            && this->supplier == od->supplier
    );
}

/*!
    \class QPlaceImage

    \inmodule QPlaces

    \brief The QPlaceImage class represents an image.


    Each QPlaceImage represents an image with a number of attributes
    such as type, thumbnail, media provider etc.

    QPlaceImage is an in memory representation of an image.
*/

/*!
    Constructs an new image object.
*/
QPlaceImage::QPlaceImage()
    : QPlaceContent(new QPlaceImagePrivate)
{
}

/*!
    Destructor.
*/
QPlaceImage::~QPlaceImage()
{
}

Q_IMPLEMENT_CONTENT_COPY_CTOR(QPlaceImage)

Q_IMPLEMENT_CONTENT_D_FUNC(QPlaceImage)

/*!
    Returns the image url.
*/
QUrl QPlaceImage::url() const
{
    Q_D(const QPlaceImage);
    return d->url;
}

/*!
    Sets image url.
*/
void QPlaceImage::setUrl(const QUrl &url)
{
    Q_D(QPlaceImage);
    d->url = url;
}

/*!
    Returns the thumbnail url.
*/
QUrl QPlaceImage::thumbnailUrl() const
{
    Q_D(const QPlaceImage);
    return d->thumbnailUrl;
}

/*!
    Sets the thumbnail \a url.
*/
void QPlaceImage::setThumbnailUrl(const QUrl &url)
{
    Q_D(QPlaceImage);
    d->thumbnailUrl = url;
}

/*!
    Returns image id.
*/
QString QPlaceImage::id() const
{
    Q_D(const QPlaceImage);
    return d->id;
}

/*!
    Sets image id.
*/
void QPlaceImage::setId(const QString &data)
{
    Q_D(QPlaceImage);
    d->id = data;
}

/*!
    Returns image meta info.
*/
QString QPlaceImage::metaInfo() const
{
    Q_D(const QPlaceImage);
    return d->metaInfo;
}

/*!
    Sets image meta info.
*/
void QPlaceImage::setMetaInfo(const QString &data)
{
    Q_D(QPlaceImage);
    d->metaInfo = data;
}

/*!
    Returns image mime type.
*/
QString QPlaceImage::mimeType() const
{
    Q_D(const QPlaceImage);
    return d->mimeType;
}

/*!
    Sets image mime type.
*/
void QPlaceImage::setMimeType(const QString &data)
{
    Q_D(QPlaceImage);
    d->mimeType = data;
}

/*!
    Returns supplier.
*/
QPlaceSupplier QPlaceImage::supplier() const
{
    Q_D(const QPlaceImage);
    return d->supplier;
}

/*!
    Sets supplier.
*/
void QPlaceImage::setSupplier(const QPlaceSupplier &data)
{
    Q_D(QPlaceImage);
    d->supplier = data;
}
