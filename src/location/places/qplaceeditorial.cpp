/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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
:   QPlaceContentPrivate(), text(other.text), contentTitle(other.contentTitle),
    language(other.language)
{
}

QPlaceEditorialPrivate::~QPlaceEditorialPrivate()
{
}

bool QPlaceEditorialPrivate::compare(const QPlaceContentPrivate *other) const
{
    const QPlaceEditorialPrivate *od = static_cast<const QPlaceEditorialPrivate *>(other);
    return QPlaceContentPrivate::compare(other)
           && text == od->text
           && contentTitle == od->contentTitle
           && language == od->language;
}

/*!
    \class QPlaceEditorial
    \inmodule QtLocation
    \ingroup QtLocation-places
    \ingroup QtLocation-places-data
    \since QtLocation 5.0

    \brief The QPlaceEditorial class represents a publisher's article describing a place.

    Each QPlaceEditorial has a title, text and language; in addition to those properties
    inherited from QPlaceContent.

    Note: The Places API only supports editorials as 'retrieve-only' objects.  Submitting editorials
    to a provider is not a supported use case.

    \sa QPlaceContent
*/

/*!
    Constructs an new editorial object.
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

/*!
    \fn QPlaceEditorial::QPlaceEditorial(const QPlaceContent &other)
    Constructs a copy of \a other if possible, otherwise constructs a default editorial object.
*/
Q_IMPLEMENT_CONTENT_COPY_CTOR(QPlaceEditorial)

Q_IMPLEMENT_CONTENT_D_FUNC(QPlaceEditorial)

/*!
    Returns a textual description of the place.

    Depending upon the provider, the
    editorial text could be either rich(HTML based) text or plain text.
*/
QString QPlaceEditorial::text() const
{
    Q_D(const QPlaceEditorial);
    return d->text;
}

/*!
    Sets the \a text of the editorial.
*/
void QPlaceEditorial::setText(const QString &text)
{
    Q_D(QPlaceEditorial);
    d->text = text;
}

/*!
    Returns the title of the editorial.
*/
QString QPlaceEditorial::title() const
{
    Q_D(const QPlaceEditorial);
    return d->contentTitle;
}

/*!
    Sets the \a title of the editorial.
*/
void QPlaceEditorial::setTitle(const QString &title)
{
    Q_D(QPlaceEditorial);
    d->contentTitle = title;
}

/*!
    Returns the language of the editorial.   Typically this would be a language code
    in the 2 letter ISO 639-1 format.
*/
QString QPlaceEditorial::language() const
{
    Q_D(const QPlaceEditorial);
    return d->language;
}

/*!
    Sets the \a language of the editorial. Typically this would be a language code
    in the 2 letter ISO 639-1 format.
*/
void QPlaceEditorial::setLanguage(const QString &language)
{
    Q_D(QPlaceEditorial);
    d->language = language;
}
