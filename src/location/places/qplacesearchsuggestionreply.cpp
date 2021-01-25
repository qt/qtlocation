/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
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
    \since 5.6

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
