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

#include <QtCore/QUrl>

#if !defined(Q_CC_MWERKS)
template<> QT_PREPEND_NAMESPACE(QPlaceContentPrivate) *QSharedDataPointer<QT_PREPEND_NAMESPACE(QPlaceContentPrivate)>::clone()
{
    return d->clone();
}
#endif

QT_USE_NAMESPACE

inline QPlaceContentPrivate *QPlaceContent::d_func()
{
    return static_cast<QPlaceContentPrivate *>(d_ptr.data());
}

inline const QPlaceContentPrivate *QPlaceContent::d_func() const
{
    return static_cast<const QPlaceContentPrivate *>(d_ptr.constData());
}

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

/*!
    Returns the supplier of the content.
*/
QPlaceSupplier QPlaceContent::supplier() const
{
    Q_D(const QPlaceContent);

    return d->supplier;
}

/*!
    Sets the supplier of the content to \a supplier.
*/
void QPlaceContent::setSupplier(const QPlaceSupplier &supplier)
{
    Q_D(QPlaceContent);

    d->supplier = supplier;
}

/*!
    Returns the source url of the content.
*/
QUrl QPlaceContent::sourceUrl() const
{
    Q_D(const QPlaceContent);

    return d->sourceUrl;
}

/*!
    Sets source url of the content to \a url.
*/
void QPlaceContent::setSourceUrl(const QUrl &url)
{
    Q_D(QPlaceContent);
    d->sourceUrl = url;
}

/*!
    Returns user id of the user who contributed this content.
*/
QString QPlaceContent::userId() const
{
    Q_D(const QPlaceContent);
    return d->userId;
}

/*!
    Sets user id to \a id.
*/
void QPlaceContent::setUserId(const QString &id)
{
    Q_D(QPlaceContent);
    d->userId = id;
}

/*!
    Returns user name of the user who contributed this content.
*/
QString QPlaceContent::userName() const
{
    Q_D(const QPlaceContent);
    return d->userName;
}

/*!
    Sets user name to \a name.
*/
void QPlaceContent::setUserName(const QString &name)
{
    Q_D(QPlaceContent);
    d->userName = name;
}


/*!
    \internal
    Constructs a new content object from the given pointer \a d.
*/
QPlaceContent::QPlaceContent(QPlaceContentPrivate *d)
    :d_ptr(d)
{
}
