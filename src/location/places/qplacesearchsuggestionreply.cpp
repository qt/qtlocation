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

#include "qplacesearchsuggestionreply.h"
#include "qplacereply_p.h"

QT_BEGIN_NAMESPACE

class QPlaceSearchSuggestionReplyPrivate : public QPlaceReplyPrivate
{
public:
    QPlaceSearchSuggestionReplyPrivate(){}
    QStringList suggestions;
};

QT_END_NAMESPACE

QT_USE_NAMESPACE

/*!
    \class QPlaceSearchSuggestionReply
    \inmodule QtLocation
    \ingroup QtLocation-places
    \ingroup QtLocation-places-replies
    \since Qt Location 5.0

    \brief The QPlaceSearchSuggestionReply class manages a search suggestion operation started by an
    instance of QPlaceManager.

    On successful completion of the operation, the reply will contain a list of search term
    suggestions.
    See \l {Search Suggestions} for an example on how to use a search suggestion reply.

    \sa QPlaceManager
*/

/*!
    Constructs a search suggestion reply with a given \a parent.
*/
QPlaceSearchSuggestionReply::QPlaceSearchSuggestionReply(QObject *parent)
    : QPlaceReply(new QPlaceSearchSuggestionReplyPrivate, parent)
{
}

/*!
    Destroys the reply.
*/
QPlaceSearchSuggestionReply::~QPlaceSearchSuggestionReply()
{
}

/*!
    Returns the search term suggestions.
*/
QStringList QPlaceSearchSuggestionReply::suggestions() const
{
    Q_D(const QPlaceSearchSuggestionReply);
    return d->suggestions;
}

/*!
   Returns type of reply.
*/
QPlaceReply::Type QPlaceSearchSuggestionReply::type() const
{
    return QPlaceReply::SearchSuggestionReply;
}

/*!
    Sets the search term \a suggestions.
*/
void QPlaceSearchSuggestionReply::setSuggestions(const QStringList &suggestions)
{
    Q_D(QPlaceSearchSuggestionReply);
    d->suggestions = suggestions;
}
