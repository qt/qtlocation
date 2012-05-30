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
** This file is part of the Nokia services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file NOKIA_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Nokia services
** plugin source code.
**
****************************************************************************/

#include "qplacesearchsuggestionreplyimpl.h"
#include "../qgeoerror_messages.h"

#include <QCoreApplication>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

QT_BEGIN_NAMESPACE

QPlaceSearchSuggestionReplyImpl::QPlaceSearchSuggestionReplyImpl(QNetworkReply *reply,
                                                                 QObject *parent)
:   QPlaceSearchSuggestionReply(parent), m_reply(reply)
{
    if (!m_reply)
        return;

    m_reply->setParent(this);
    connect(m_reply, SIGNAL(finished()), this, SLOT(replyFinished()));
    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(replyError(QNetworkReply::NetworkError)));
}

QPlaceSearchSuggestionReplyImpl::~QPlaceSearchSuggestionReplyImpl()
{
}

void QPlaceSearchSuggestionReplyImpl::abort()
{
    if (m_reply)
        m_reply->abort();
}

void QPlaceSearchSuggestionReplyImpl::setError(QPlaceReply::Error error_,
                                               const QString &errorString)
{
    QPlaceReply::setError(error_, errorString);
    emit error(error_, errorString);
    setFinished(true);
    emit finished();
}

void QPlaceSearchSuggestionReplyImpl::replyFinished()
{
    QJsonDocument document = QJsonDocument::fromJson(m_reply->readAll());
    if (!document.isObject()) {
        setError(ParseError, QCoreApplication::translate(NOKIA_PLUGIN_CONTEXT_NAME, PARSE_ERROR));
        emit error(error(), errorString());
        return;
    }

    QJsonObject object = document.object();

    QJsonArray suggestions = object.value(QLatin1String("suggestions")).toArray();

    QStringList s;
    for (int i = 0; i < suggestions.count(); ++i) {
        QJsonValue v = suggestions.at(i);
        if (v.isString())
            s.append(v.toString());
    }

    setSuggestions(s);

    m_reply->deleteLater();
    m_reply = 0;

    setFinished(true);
    emit finished();
}

void QPlaceSearchSuggestionReplyImpl::replyError(QNetworkReply::NetworkError error)
{
    switch (error) {
    case QNetworkReply::OperationCanceledError:
        setError(CancelError, "Request canceled.");
        break;
    default:
        setError(CommunicationError, "Network error.");
    }
}

QT_END_NAMESPACE
