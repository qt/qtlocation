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

#include "qplacequery.h"

QT_BEGIN_NAMESPACE

class QPlaceQueryPrivate : public QSharedData
{
public:
    QPlaceQueryPrivate();
    QPlaceQueryPrivate(const QPlaceQueryPrivate &other);

    ~QPlaceQueryPrivate();

    bool operator==(const QPlaceQueryPrivate &other) const;

    int offset;
    int limit;
};

QT_END_NAMESPACE

QT_USE_NAMESPACE

QPlaceQueryPrivate::QPlaceQueryPrivate()
    : QSharedData(),
      offset(0),
      limit(-1)
{
}

QPlaceQueryPrivate::QPlaceQueryPrivate(const QPlaceQueryPrivate &other)
    : QSharedData()
{
    this->offset = other.offset;
    this->limit = other.limit;
}

QPlaceQueryPrivate::~QPlaceQueryPrivate()
{
}

bool QPlaceQueryPrivate::operator==(const QPlaceQueryPrivate &other) const
{
    return (
            this->offset == other.offset
            && this->limit == other.limit
    );
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
QPlaceQuery::QPlaceQuery()
    : d(new QPlaceQueryPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceQuery::QPlaceQuery(const QPlaceQuery &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceQuery::~QPlaceQuery()
{
}

QPlaceQuery &QPlaceQuery::operator =(const QPlaceQuery &other) {
    d = other.d;
    return *this;
}

bool QPlaceQuery::operator==(const QPlaceQuery &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns the offset. By default the offset is set to 0.
    The offset determines the first index which is retrieved, it is generally
    used in conjunction with limit() to facilitate paging.

    Negative offests are treated as an offset of 0;
*/
int QPlaceQuery::offset() const
{
    return d->offset;
}

/*!
    Sets the \a offset.
*/
void QPlaceQuery::setOffset(int offset)
{
    d->offset = offset;
}

/*!
    Returns the maximum number of places to be fetched.  The default
    value of this limit is -1, indicating that the default limit of the
    backend should be used.
*/
int QPlaceQuery::limit() const
{
    return d->limit;
}

/*!
    Sets the maximum number of places to be fetched to \a limit.

    A negative limit indicates that the default limit of the backend should used.
    If the backend maximum limit is less than \a limit, then only the
    backend maximum limit number of places are retrieved.

    (A limit of 0 will retrieve no places).
*/
void QPlaceQuery::setLimit(int limit)
{
    d->limit = limit;
}

/*!
    Clears the parameters of the search query.
*/
void QPlaceQuery::clear()
{
    d->offset = 0;
    d->limit = -1;
}
