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

#include "qplacecontentrequest_p.h"
#include "qplacecontentrequest.h"
#include "qgeocoordinate.h"

QT_BEGIN_NAMESPACE

QPlaceContentRequestPrivate::QPlaceContentRequestPrivate()
:   QPlaceRequestPrivate(QPlaceRequest::ContentRequest), contentType(QPlaceContent::InvalidType)
{
}

QPlaceContentRequestPrivate::~QPlaceContentRequestPrivate()
{
}

QPlaceContentRequestPrivate::QPlaceContentRequestPrivate(const QPlaceContentRequestPrivate &other)
    : QPlaceRequestPrivate(other)
{
    this->contentType = other.contentType;
}

bool QPlaceContentRequestPrivate::compare(const QPlaceRequestPrivate *other) const
{
    const QPlaceContentRequestPrivate *od = static_cast<const QPlaceContentRequestPrivate *>(other);
    return contentType == od->contentType && QPlaceRequestPrivate::compare(other);
}

void QPlaceContentRequestPrivate::clear()
{
    QPlaceRequestPrivate::clear();
    this->contentType = QPlaceContent::InvalidType;
}

/*!
    \class QPlaceContentRequest
    \inmodule QtLocation
    \ingroup QtLocation-places
    \since QtLocation 5.0

    \brief The QPlaceContentRequest class represents the query parameters of a content request.

    The QPlaceContentRequest class represents a query parameters object, it currently
    specifies the type of content to be retrived.
*/

/*!
    Default constructor. Constructs an new request object.
*/
QPlaceContentRequest::QPlaceContentRequest()
    : QPlaceRequest(new QPlaceContentRequestPrivate)
{
}

Q_IMPLEMENT_COPY_CTOR(QPlaceContentRequest, QPlaceRequest)

/*!
    Destructor.
*/
QPlaceContentRequest::~QPlaceContentRequest()
{
}

Q_IMPLEMENT_D_FUNC(QPlaceContentRequest)


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

QT_END_NAMESPACE
