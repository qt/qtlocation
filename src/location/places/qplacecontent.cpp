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

#include "qplacecontent.h"
#include "qplacecontent_p.h"

#if !defined(Q_CC_MWERKS)
template<> QT_PREPEND_NAMESPACE(QPlaceContentPrivate) *QSharedDataPointer<QT_PREPEND_NAMESPACE(QPlaceContentPrivate)>::clone()
{
    return d->clone();
}
#endif

QT_USE_NAMESPACE

/* Constructs an empty content object */
QPlaceContent::QPlaceContent()
    :d_ptr(0)
{
}

/*!
    Constructs a new copy of \a other
*/
QPlaceContent::QPlaceContent(const QPlaceContent &other)
    :d_ptr(other.d_ptr)
{
}

/*!
    Assigns the \a other content object to this
*/
QPlaceContent &QPlaceContent::operator=(const QPlaceContent &other)
{
    if (this != &other ) {
        d_ptr = other.d_ptr;
    }
    return *this;
}

/*!
    Destroys the content object
*/
QPlaceContent::~QPlaceContent()
{
}

/*!
    Returns the content type.
*/
QPlaceContent::Type QPlaceContent::type() const
{
    if (!d_ptr)
        return InvalidType;
    return d_ptr->type();
}

/*!
    Returns true if the content object is equivalent to \a other,
    otherwise returns false.
*/
bool QPlaceContent::operator==(const QPlaceContent &other) const
{
    // An invalid content object is only equal to another invalid content object
    if (!d_ptr)
        return !other.d_ptr;

    if (type() != other.type())
        return false;

    return d_ptr->compare(other.d_ptr);
}

bool QPlaceContent::operator!=(const QPlaceContent &other) const
{
    return !(*this == other);
}

QPlaceSupplier QPlaceContent::supplier() const
{
    return d_ptr->supplier;
}

void QPlaceContent::setSupplier(const QPlaceSupplier &supplier)
{
    d_ptr->supplier = supplier;
}

/*!
    \internal
    Constructs a new content object from the given pointer \a d.
*/
QPlaceContent::QPlaceContent(QPlaceContentPrivate *d)
    :d_ptr(d)
{
}
