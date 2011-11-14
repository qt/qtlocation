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

#include "qplaceimage.h"
#include "qplaceimage_p.h"

QT_USE_NAMESPACE

QPlaceImagePrivate::QPlaceImagePrivate() : QPlaceContentPrivate()
{
}

QPlaceImagePrivate::QPlaceImagePrivate(const QPlaceImagePrivate &other)
    : QPlaceContentPrivate(other)
{
    url = other.url;
    id = other.id;
    mimeType = other.mimeType;
}

QPlaceImagePrivate::~QPlaceImagePrivate()
{
}

bool QPlaceImagePrivate::compare(const QPlaceContentPrivate *other) const
{
    const QPlaceImagePrivate *od = static_cast<const QPlaceImagePrivate *>(other);
    return QPlaceContentPrivate::compare(other)
            && url == od->url && id == od->id && mimeType == od->mimeType;
}

/*!
    \class QPlaceImage
    \inmodule QtLocation
    \ingroup QtLocation-places
    \ingroup QtLocation-places-data
    \since QtLocation 5.0

    \brief The QPlaceImage class represents a reference to an image.

    Each QPlaceImage represents a set of metadata about an image such as it's
    url, id and MIME type.  These are properties in addition to those provided
    by QPlaceContent.

    Note: The Places API only supports images as 'retrieve-only' objects.  Submitting
    images to a provider is not a supported use case.
    \sa QPlaceContent
*/

/*!
    Constructs an new QPlaceImage.
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

/*!
    \fn QPlaceImage::QPlaceImage(const QPlaceContent &other)
    Constructs a copy of \a other if possible, otherwise constructs a default image.
*/

Q_IMPLEMENT_CONTENT_COPY_CTOR(QPlaceImage)

Q_IMPLEMENT_CONTENT_D_FUNC(QPlaceImage)

/*!
    Returns the image's url.
*/
QUrl QPlaceImage::url() const
{
    Q_D(const QPlaceImage);
    return d->url;
}

/*!
    Sets the image's \a url.
*/
void QPlaceImage::setUrl(const QUrl &url)
{
    Q_D(QPlaceImage);
    d->url = url;
}

/*!
    Returns the image's id.
*/
QString QPlaceImage::imageId() const
{
    Q_D(const QPlaceImage);
    return d->id;
}

/*!
    Sets image's \a id.
*/
void QPlaceImage::setImageId(const QString &id)
{
    Q_D(QPlaceImage);
    d->id = id;
}

/*!
    Returns the image's MIME type.
*/
QString QPlaceImage::mimeType() const
{
    Q_D(const QPlaceImage);
    return d->mimeType;
}

/*!
    Sets image's MIME \a type.
*/
void QPlaceImage::setMimeType(const QString &type)
{
    Q_D(QPlaceImage);
    d->mimeType = type;
}
