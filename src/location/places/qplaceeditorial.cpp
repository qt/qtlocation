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

#include "qplaceeditorial.h"
#include "qplacecontent_p.h"

QT_USE_NAMESPACE

/*!
    \class QPlaceEditorial
    \inmodule QtLocation
    \ingroup QtLocation-places
    \ingroup QtLocation-places-data
    \since 5.6

    \brief The QPlaceEditorial class represents a publisher's article describing a place.

    Each QPlaceEditorial has a title, text and language; in addition to those properties
    inherited from QPlaceContent.

    Note: The Places API only supports editorials as 'retrieve-only' objects.  Submitting editorials
    to a provider is not a supported use case.

    \sa QPlaceContent
*/

#if QT_DEPRECATED_SINCE(6, 0)
/*!
    Constructs a new editorial object.
*/
QPlaceEditorial::QPlaceEditorial()
    : QPlaceContent(QPlaceContent::EditorialType)
{
}

/*!
    \fn QPlaceEditorial::QPlaceEditorial(const QPlaceContent &other)
    Constructs a copy of \a other if possible, otherwise constructs a default
    editorial object.
*/
Q_IMPLEMENT_CONTENT_COPY_CTOR(QPlaceEditorial, QPlaceContent::EditorialType)

#endif // QT_DEPRECATED_SINCE(6, 0)
