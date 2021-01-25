/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
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
    \since 5.6

    \brief The QPlaceImage class represents a reference to an image.

    Each QPlaceImage represents a set of metadata about an image such as it's
    url, identifier and MIME type.  These are properties in addition to those provided
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
    Returns the image's identifier.
*/
QString QPlaceImage::imageId() const
{
    Q_D(const QPlaceImage);
    return d->id;
}

/*!
    Sets image's \a identifier.
*/
void QPlaceImage::setImageId(const QString &identifier)
{
    Q_D(QPlaceImage);
    d->id = identifier;
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
