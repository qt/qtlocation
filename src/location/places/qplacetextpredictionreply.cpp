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

#include "qplacetextpredictionreply.h"
#include "qplacereply_p.h"

QT_BEGIN_NAMESPACE

class QPlaceTextPredictionReplyPrivate : public QPlaceReplyPrivate
{
public:
    QPlaceTextPredictionReplyPrivate(){}
    QStringList textPredictions;
};

QT_END_NAMESPACE

QT_USE_NAMESPACE

/*!
    \class QPlaceTextPredictionReply
    \inmodule QtLocation
    \ingroup QtLocation-places
    \since QtLocation 5.0

    \brief The QPlaceTextPredictionReply class manages a text prediction operation started by an
    instance of QPlaceManager.
*/

/*!
    Constructs a text prediction reply with a given \a parent.
*/
QPlaceTextPredictionReply::QPlaceTextPredictionReply(QObject *parent)
    : QPlaceReply(new QPlaceTextPredictionReplyPrivate, parent)
{
}

/*!
    Destroys the reply.
*/
QPlaceTextPredictionReply::~QPlaceTextPredictionReply()
{
}

/*!
    Returns the text predictions.
*/
QStringList QPlaceTextPredictionReply::textPredictions() const
{
    Q_D(const QPlaceTextPredictionReply);
    return d->textPredictions;
}

/*!
   Returns type of reply.
*/
QPlaceReply::Type QPlaceTextPredictionReply::type() const
{
    return QPlaceReply::TextPredictionReply;
}

/*!
    Sets the text \a predictions.
*/
void QPlaceTextPredictionReply::setTextPredictions(const QStringList &predictions)
{
    Q_D(QPlaceTextPredictionReply);
    d->textPredictions = predictions;
}
