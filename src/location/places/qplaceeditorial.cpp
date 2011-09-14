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

#include "qplaceeditorial.h"
#include "qplaceeditorial_p.h"

QT_USE_NAMESPACE

QPlaceEditorialPrivate::QPlaceEditorialPrivate()
:   QPlaceContentPrivate()
{
}

QPlaceEditorialPrivate::QPlaceEditorialPrivate(const QPlaceEditorialPrivate &other)
:   QPlaceContentPrivate(), content(other.content), contentTitle(other.contentTitle),
    supplier(other.supplier), sourceUrl(other.sourceUrl), language(other.language)
{
}

QPlaceEditorialPrivate::~QPlaceEditorialPrivate()
{
}

bool QPlaceEditorialPrivate::compare(const QPlaceContentPrivate *other) const
{
    const QPlaceEditorialPrivate *od = static_cast<const QPlaceEditorialPrivate *>(other);
    return content == od->content &&
           contentTitle == od->contentTitle &&
           supplier == od->supplier &&
           sourceUrl == od->sourceUrl &&
           language == od->language;
}

/*!
    \class QPlaceEditorial

    \inmodule QPlaces

    \brief The QPlaceEditorial class represents a address object.

    Each QPlaceEditorial represents a description object with a number of attributes
    such as title, value etc. Each QPlaceEditorial is associated with place.

    Editorial objects are read-only, e.g. user of API might get editorial objects associated to
    specific place but can not edit its content. User might also create new editorial.
*/

/*!
    Constructs an new place editorial object.
*/
QPlaceEditorial::QPlaceEditorial()
:   QPlaceContent(new QPlaceEditorialPrivate)
{
}

/*!
    Destructor.
*/
QPlaceEditorial::~QPlaceEditorial()
{
}

Q_IMPLEMENT_CONTENT_COPY_CTOR(QPlaceEditorial)

Q_IMPLEMENT_CONTENT_D_FUNC(QPlaceEditorial)

/*!
    Returns content.
*/
QString QPlaceEditorial::content() const
{
    Q_D(const QPlaceEditorial);
    return d->content;
}

/*!
    Sets content.
*/
void QPlaceEditorial::setContent(const QString &data)
{
    Q_D(QPlaceEditorial);
    d->content = data;
}

/*!
    Returns content title.
*/
QString QPlaceEditorial::title() const
{
    Q_D(const QPlaceEditorial);
    return d->contentTitle;
}

/*!
    Sets content title.
*/
void QPlaceEditorial::setTitle(const QString &data)
{
    Q_D(QPlaceEditorial);
    d->contentTitle = data;
}

/*!
    Returns supplier. Do not remove it.
*/
QPlaceSupplier QPlaceEditorial::supplier() const
{
    Q_D(const QPlaceEditorial);
    return d->supplier;
}

/*!
    Sets supplier.
*/
void QPlaceEditorial::setSupplier(const QPlaceSupplier &data)
{
    Q_D(QPlaceEditorial);
    d->supplier = data;
}

/*!
    Returns source url.
*/
QUrl QPlaceEditorial::sourceUrl() const
{
    Q_D(const QPlaceEditorial);
    return d->sourceUrl;
}

/*!
    Sets source url.
*/
void QPlaceEditorial::setSourceUrl(const QUrl &data)
{
    Q_D(QPlaceEditorial);
    d->sourceUrl = data;
}

/*!
    Returns language.
*/
QString QPlaceEditorial::language() const
{
    Q_D(const QPlaceEditorial);
    return d->language;
}

/*!
    Sets language.
*/
void QPlaceEditorial::setLanguage(const QString &data)
{
    Q_D(QPlaceEditorial);
    d->language = data;
}
