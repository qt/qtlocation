/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
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

#include "qgeoroutereply_nokia.h"
#include "qgeoroutexmlparser.h"

#include <qgeorouterequest.h>

QT_BEGIN_NAMESPACE

QGeoRouteReplyNokia::QGeoRouteReplyNokia(const QGeoRouteRequest &request, const QList<QNetworkReply*> &replies, QObject *parent)
        : QGeoRouteReply(request, parent),
        m_replies(replies)
{
    foreach (QNetworkReply *reply, m_replies) {
        connect(reply,
                SIGNAL(finished()),
                this,
                SLOT(networkFinished()));

        connect(reply,
                SIGNAL(error(QNetworkReply::NetworkError)),
                this,
                SLOT(networkError(QNetworkReply::NetworkError)));
    }
}

QGeoRouteReplyNokia::~QGeoRouteReplyNokia()
{
    abort();
}

void QGeoRouteReplyNokia::abort()
{
    if (m_replies.isEmpty())
        return;

    foreach (QNetworkReply *reply, m_replies) {
        reply->abort();
        reply->deleteLater();
    }
    m_replies.clear();
}

void QGeoRouteReplyNokia::networkFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply)
        return;

    if (reply->error() != QNetworkReply::NoError) {
        // Removed because this is already done in networkError, which previously caused _two_ errors to be raised for every error.
        //setError(QGeoRouteReply::CommunicationError, m_reply->errorString());
        //m_reply->deleteLater();
        //m_reply = 0;
        return;
    }

    QGeoRouteXmlParser parser(request());
    if (parser.parse(reply)) {
        addRoutes(parser.results());
        reply->deleteLater();
        m_replies.removeOne(reply);
        if (m_replies.isEmpty())
            setFinished(true);
    } else {
        // add a qWarning with the actual parser.errorString()
        setError(QGeoRouteReply::ParseError, "The response from the service was not in a recognisable format.");
        abort();
    }
}

void QGeoRouteReplyNokia::networkError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error)

    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply)
        return;

    bool resolvedError = false;
    if (QNetworkReply::UnknownContentError == error) {
        QGeoRouteXmlParser parser(request());
        if (parser.parse(reply)) {
            addRoutes(parser.results());
            reply->deleteLater();
            m_replies.removeOne(reply);
            resolvedError = true;
            if (m_replies.isEmpty())
                setFinished(true);
        }
    }

    if (!resolvedError) {
        setError(QGeoRouteReply::CommunicationError, reply->errorString());
        abort();
    }
}

QT_END_NAMESPACE
