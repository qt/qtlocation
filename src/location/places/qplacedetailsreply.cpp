/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qplacedetailsreply.h"
#include "qplacereply_p.h"

QT_BEGIN_NAMESPACE
class QPlaceDetailsReplyPrivate : public QPlaceReplyPrivate
{
public:
    QPlaceDetailsReplyPrivate() {}
    ~QPlaceDetailsReplyPrivate() {}
    QPlace result;
};

QT_END_NAMESPACE

QT_USE_NAMESPACE

/*!
    \class QPlaceDetailsReply
    \inmodule QtLocation
    \ingroup QtLocation-places
    \ingroup QtLocation-places-replies
    \since QtLocation 5.0

    \brief The QPlaceDetailsReply class manages a place details fetch operation started by an
    instance of QPlaceManager.

    See \l {Fetching Place Details} for an example on how to use a details reply.
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
