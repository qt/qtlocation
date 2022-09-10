/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qplacecontentrequest_p.h"
#include "qplacecontentrequest.h"
#include "qgeocoordinate.h"

QT_BEGIN_NAMESPACE

QT_DEFINE_QSDP_SPECIALIZATION_DTOR(QPlaceContentRequestPrivate)

bool QPlaceContentRequestPrivate::operator==(const QPlaceContentRequestPrivate &other) const
{
    return contentType == other.contentType
            && limit == other.limit;
}

void QPlaceContentRequestPrivate::clear()
{
    contentType = QPlaceContent::NoType;
    limit = -1;
}

/*!
    \class QPlaceContentRequest
    \inmodule QtLocation
    \ingroup QtLocation-places
    \ingroup QtLocation-places-requests
    \since 5.6

    \brief The QPlaceContentRequest class represents the parameters of a content request.

    The QPlaceContentRequest class is used in conjunction with a QPlaceManager to
    retrieve rich content like images and reviews in a paginated fashion.
    The following code would request a set of 5 images from the 10th index:

    \snippet places/requesthandler.h Content request
    \dots
    \dots
    \snippet places/requesthandler.h Content handler

    \sa QPlaceContentReply
*/

/*!
    Constructs a new request object.
*/
QPlaceContentRequest::QPlaceContentRequest()
    : d_ptr(new QPlaceContentRequestPrivate())
{
}

/*!
    Constructs a copy of \a other.
*/
QPlaceContentRequest::QPlaceContentRequest(const QPlaceContentRequest &other) noexcept = default;

/*!
    Destroys the request object
*/
QPlaceContentRequest::~QPlaceContentRequest() = default;

/*!
    Assigns \a other to this content request and returns a reference
    to this content request.
*/
QPlaceContentRequest &QPlaceContentRequest::operator=(const QPlaceContentRequest & other) noexcept
{
    if (this == &other)
        return *this;

    d_ptr = other.d_ptr;
    return *this;
}

/*!
    \fn bool QPlaceContentRequest::operator==(const QPlaceContentRequest &lhs, const QPlaceContentRequest &rhs) noexcept
    Returns true if \a lhs is equal to \a rhs, otherwise returns false.
*/

/*!
    \fn bool QPlaceContentRequest::operator!=(const QPlaceContentRequest &lhs, const QPlaceContentRequest &rhs) noexcept
    Returns true if \a lhs is not equal to \a rhs, otherwise returns false.
*/

bool QPlaceContentRequest::isEqual(const QPlaceContentRequest &other) const noexcept
{
    Q_D(const QPlaceContentRequest);
    return *d == *other.d_func();
}

/*!
    Returns the type of content to be requested, for example reviews or images
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
    Returns the identifier of the place content is to be fetched for.
*/
QString QPlaceContentRequest::placeId() const
{
    Q_D(const QPlaceContentRequest);
    return d->placeId;
}

/*!
    Sets the identifier of the place to fetch content for to \a identifier.
*/
void QPlaceContentRequest::setPlaceId(const QString &identifier)
{
    Q_D(QPlaceContentRequest);
    d->placeId = identifier;
}

/*!
    Returns backend specific additional content context associated with this place content request.
*/
QVariant QPlaceContentRequest::contentContext() const
{
    Q_D(const QPlaceContentRequest);
    return d->contentContext;
}

/*!
    Sets the content context to \a context.

    \note This method is intended to be used by geo service plugins when returning place content
    results.

    The content context is used by backends to store additional content context related to the
    content request. Other relevant fields should also be filled in. For example, if the content
    request is for image content the content type should also be set with \l setContentType(). The
    content context allows additional context to be kept which is not directly accessible via the
    Qt Location API.

    The content context can be of any type storable in a QVariant. The value of the content context
    is not intended to be used directly by applications.
*/
void QPlaceContentRequest::setContentContext(const QVariant &context)
{
    Q_D(QPlaceContentRequest);
    d->contentContext = context;
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
    Clears the content request.
*/
void QPlaceContentRequest::clear()
{
    Q_D(QPlaceContentRequest);
    d->clear();
}

inline QPlaceContentRequestPrivate *QPlaceContentRequest::d_func()
{
    return static_cast<QPlaceContentRequestPrivate *>(d_ptr.data());
}

inline const QPlaceContentRequestPrivate *QPlaceContentRequest::d_func() const
{
    return static_cast<const QPlaceContentRequestPrivate *>(d_ptr.constData());
}

QT_END_NAMESPACE
