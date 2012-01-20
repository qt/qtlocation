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

#include "qplacecontentrequest_p.h"
#include "qplacecontentrequest.h"
#include "qgeocoordinate.h"

QT_BEGIN_NAMESPACE

QPlaceContentRequestPrivate::QPlaceContentRequestPrivate()
    : QSharedData(), contentType(QPlaceContent::NoType),
      limit(-1), offset(0)
{
}

QPlaceContentRequestPrivate::QPlaceContentRequestPrivate(const QPlaceContentRequestPrivate &other)
    : QSharedData(other), contentType(other.contentType),
      limit(other.limit), offset(other.offset)
{
}

QPlaceContentRequestPrivate::~QPlaceContentRequestPrivate()
{
}

bool QPlaceContentRequestPrivate::operator==(const QPlaceContentRequestPrivate &other) const
{
    return contentType == other.contentType
            && limit == other.limit
            && offset == other.offset;
}

void QPlaceContentRequestPrivate::clear()
{
    contentType = QPlaceContent::NoType;
    limit = -1;
    offset = 0;
}

/*!
    \class QPlaceContentRequest
    \inmodule QtLocation
    \ingroup QtLocation-places
    \ingroup QtLocation-places-requests
    \since QtLocation 5.0

    \brief The QPlaceContentRequest class represents the parameters of a content request.

    The QPlaceContentRequest class is used in conjunction with a QPlaceManager to
    retrieve rich content like images and reviews in a paginated fashion.
    The following code would request a set of 5 images from the 10th index:

    \snippet snippets/places/requesthandler.h Content request
    \dots
    \dots
    \snippet snippets/places/requesthandler.h Content handler

    \sa QPlaceContentReply
*/

/*!
    Constructs an new request object.
*/
QPlaceContentRequest::QPlaceContentRequest()
    : d_ptr(new QPlaceContentRequestPrivate())
{
}

/*!
    Constructs a copy of \a other.
*/
QPlaceContentRequest::QPlaceContentRequest(const QPlaceContentRequest &other)
    : d_ptr(other.d_ptr)
{
}

/*!
    Destroys the request object
*/
QPlaceContentRequest::~QPlaceContentRequest()
{
}

/*!
    Assigns \a other to this content request and returns a reference
    to this content request.
*/
QPlaceContentRequest &QPlaceContentRequest::operator= (const QPlaceContentRequest & other)
{
    d_ptr = other.d_ptr;
    return *this;
}

/*!
    Returns true if \a other is equal to this content request,
    otherwise returns false.
*/
bool QPlaceContentRequest::operator== (const QPlaceContentRequest &other) const
{
    Q_D(const QPlaceContentRequest);
    return *d == *other.d_func();
}

/*!
    Returns true if \a other is not equal to this content request,
    otherwise returns false.
*/
bool QPlaceContentRequest::operator!= (const QPlaceContentRequest &other) const
{
    Q_D(const QPlaceContentRequest);
    return !(*d == *other.d_func());
}

/*!
    Returns the type of content to be requested, eg reviews or images
*/
QPlaceContent::Type QPlaceContentRequest::contentType() const
{
    Q_D(const QPlaceContentRequest);
    return d->contentType;
}

/*!
    Sets the \a type of content to be requested.
*/
void QPlaceContentRequest::setContentType(QPlaceContent::Type type)
{
    Q_D(QPlaceContentRequest);
    d->contentType = type;
}

/*!
    Returns the maximum number of content items to retrieve.

    A negative value for limit means that it is undefined.  It is left up to the backend
    provider to choose an appropriate number of items to return.

    The default limit is -1.
*/
int QPlaceContentRequest::limit() const
{
    Q_D(const QPlaceContentRequest);
    return d->limit;
}

/*!
    Set the maximum number of content items to retrieve to
    \a limit.
*/
void QPlaceContentRequest::setLimit(int limit)
{
    Q_D(QPlaceContentRequest);
    d->limit = limit;
}

/*!
    Returns the offset index of the first item that is to be retrieved.

    The default offset is 0.
*/
int QPlaceContentRequest::offset() const
{
    Q_D(const QPlaceContentRequest);
    return d->offset;
}

/*!
    Sets the starting index of the first item to be retrieved
    to \a offset.
*/
void QPlaceContentRequest::setOffset(int offset)
{
    Q_D(QPlaceContentRequest);
    d->offset = offset;
}

/*!
    Clears the content request.
*/
void QPlaceContentRequest::clear()
{
    Q_D(QPlaceContentRequest);
    d->clear();
}

inline QPlaceContentRequestPrivate* QPlaceContentRequest::d_func()
{
    return static_cast<QPlaceContentRequestPrivate *>(d_ptr.data());
}

inline const QPlaceContentRequestPrivate* QPlaceContentRequest::d_func() const
{
    return static_cast<const QPlaceContentRequestPrivate *>(d_ptr.constData());
}

QT_END_NAMESPACE
