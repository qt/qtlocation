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

#include "qplacedetailsreply.h"
#include "qplacereply_p.h"

#include <QtLocation/QPlace>

QT_BEGIN_NAMESPACE
class QPlaceDetailsReplyPrivate : public QPlaceReplyPrivate
{
public:
    QPlace result;
};

QT_END_NAMESPACE

QT_USE_NAMESPACE

/*!
    \class QPlaceDetailsReply
    \inmodule QtLocation
    \ingroup QtLocation-places
    \ingroup QtLocation-places-replies
    \since 5.6

    \brief The QPlaceDetailsReply class manages a place details fetch operation started by an
    instance of QPlaceManager.

    See \l {QML Places API#Fetching Place Details}{Fetching Place Details} for an example on how to use a details reply.
    \sa QPlaceManager
*/

/*!
    Constructs a details reply with a given \a parent.
*/
QPlaceDetailsReply::QPlaceDetailsReply(QObject *parent)
    : QPlaceReply(new QPlaceDetailsReplyPrivate, parent)
{
}

/*!
    Destroys the details reply.
*/
QPlaceDetailsReply::~QPlaceDetailsReply()
{
}

/*!
    Returns the type of reply.
*/
QPlaceReply::Type QPlaceDetailsReply::type() const
{
    return QPlaceReply::DetailsReply;
}

 /*!
    Returns the place that was fetched.
*/
QPlace QPlaceDetailsReply::place() const
{
    Q_D(const QPlaceDetailsReply);
    return d->result;
}

/*!
    Sets the fetched \a place of the reply.
*/
void QPlaceDetailsReply::setPlace(const QPlace &place)
{
    Q_D(QPlaceDetailsReply);
    d->result = place;
}
