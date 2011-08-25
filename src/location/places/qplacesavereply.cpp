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

#include "qplacesavereply.h"
#include "qplacereply_p.h"

QT_BEGIN_NAMESPACE
class QPlaceSaveReplyPrivate : public QPlaceReplyPrivate
{
public:
    QPlaceSaveReplyPrivate() {}
    ~QPlaceSaveReplyPrivate() {}
    QString placeId;
};

QT_END_NAMESPACE

QT_USE_NAMESPACE

/*!
    \class QPlaceSaveReply

    \brief The QPlaceSaveReply class manages a place save operation started by an
    instance of QPlaceManager.

    \inmodule QtPlaces

    \ingroup places-main
*/

/*!
    Constructs a save reply with a given \a parent.
*/
QPlaceSaveReply::QPlaceSaveReply(QObject *parent)
    : QPlaceReply(new QPlaceSaveReplyPrivate, parent)
{
}

/*!
    Destroys the save reply.
*/
QPlaceSaveReply::~QPlaceSaveReply()
{
}

/*!
    Returns the type of reply.
*/
QPlaceReply::Type QPlaceSaveReply::type() const
{
    return QPlaceReply::SaveReply;
}

 /*!
    Returns the id of the saved place.  One should ensure that the reply
    is finished before calling this function, otherwise an
    empty string is returned.
*/
QString QPlaceSaveReply::placeId() const
{
    Q_D(const QPlaceSaveReply);
    return d->placeId;
}

/*!
    Sets the saved \a placeId
*/
void QPlaceSaveReply::setPlaceId(const QString &placeId)
{
    Q_D(QPlaceSaveReply);
    d->placeId = placeId;
}
