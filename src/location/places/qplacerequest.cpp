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

#include "qplacerequest.h"
#include "qplacerequest_p.h"

QT_BEGIN_NAMESPACE

#if !defined(Q_CC_MWERKS)
template<> QT_PREPEND_NAMESPACE(QPlaceRequestPrivate) *QSharedDataPointer<QT_PREPEND_NAMESPACE(QPlaceRequestPrivate)>::clone()
{
    return d->clone();
}
#endif

QPlaceRequestPrivate::QPlaceRequestPrivate(QPlaceRequest::Type type)
:   QSharedData(), offset(0), limit(-1), requestType(type)
{
}

QPlaceRequestPrivate::QPlaceRequestPrivate(const QPlaceRequestPrivate &other)
    : QSharedData()
{
    this->offset = other.offset;
    this->limit = other.limit;
}

QPlaceRequestPrivate::~QPlaceRequestPrivate()
{
}

QPlaceRequestPrivate *QPlaceRequestPrivate::clone() const { return new QPlaceRequestPrivate(*this);}

bool QPlaceRequestPrivate::compare(const QPlaceRequestPrivate *other) const
{
    return (this->offset == other->offset
            && this->limit == other->limit);
}

void QPlaceRequestPrivate::clear()
{
    this->offset =0;
    this->limit = -1;
}

/*!
    \class QPlaceQuery

    \inmodule Location

    \brief The QPlaceQuery class is the base class used for various search queries.

    \ingroup places-main

*/

/*!
    Default constructor. Constructs an new query object.
*/
QPlaceRequest::QPlaceRequest()
:   d_ptr(new QPlaceRequestPrivate(QPlaceRequest::Request))
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceRequest::QPlaceRequest(const QPlaceRequest &other)
    :d_ptr(other.d_ptr)
{
}

/*!
    Destructor.
*/
QPlaceRequest::~QPlaceRequest()
{
}

QPlaceRequest &QPlaceRequest::operator =(const QPlaceRequest &other) {
    d_ptr = other.d_ptr;
    return *this;
}

bool QPlaceRequest::operator==(const QPlaceRequest &other) const
{
    if (!d_ptr)
        return !other.d_ptr;

    if (d_ptr->requestType != other.d_ptr->requestType)
        return false;

    return d_ptr->compare(other.d_ptr);
}

QPlaceRequest::Type QPlaceRequest::type() const
{
    return d_ptr->requestType;
}

/*!
    Returns the offset. By default the offset is set to 0.
    The offset determines the first index which is retrieved, it is generally
    used in conjunction with limit() to facilitate paging.

    Negative offests are treated as an offset of 0;
*/
int QPlaceRequest::offset() const
{
    return d_ptr->offset;
}

/*!
    Sets the \a offset.
*/
void QPlaceRequest::setOffset(int offset)
{
    d_ptr->offset = offset;
}

/*!
    Returns the maximum number of places to be fetched.  The default
    value of this limit is -1, indicating that the default limit of the
    backend should be used.
*/
int QPlaceRequest::limit() const
{
    return d_ptr->limit;
}

/*!
    Sets the maximum number of places to be fetched to \a limit.

    A negative limit indicates that the default limit of the backend should used.
    If the backend maximum limit is less than \a limit, then only the
    backend maximum limit number of places are retrieved.

    (A limit of 0 will retrieve no places).
*/
void QPlaceRequest::setLimit(int limit)
{
    d_ptr->limit = limit;
}

/*!
    Clears the parameters of the search query.
*/
void QPlaceRequest::clear()
{
    d_ptr->clear();
}

QPlaceRequest::QPlaceRequest(QPlaceRequestPrivate *d)
    : d_ptr(d)
{
}

QT_END_NAMESPACE
