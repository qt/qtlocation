/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
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

#include "qgeoroutereply_nokia.h"
#include "qgeoroutexmlparser.h"
#include "qgeoerror_messages.h"

#include <qgeorouterequest.h>

#include <QtCore/QCoreApplication>

Q_DECLARE_METATYPE(QList<QGeoRoute>)

QT_BEGIN_NAMESPACE

QGeoRouteReplyNokia::QGeoRouteReplyNokia(const QGeoRouteRequest &request,
                                         const QList<QNetworkReply *> &replies,
                                         QObject *parent)
:   QGeoRouteReply(request, parent), m_parsers(0)
{
    qRegisterMetaType<QList<QGeoRoute> >();

    bool failure = false;
    for (QNetworkReply *reply : replies) {
        if (!reply) {
            failure = true;
            continue;
        }
        connect(reply, &QNetworkReply::finished,
                this, &QGeoRouteReplyNokia::networkFinished);
        connect(reply, &QNetworkReply::errorOccurred,
                this, &QGeoRouteReplyNokia::networkError);
        connect(this, &QGeoRouteReply::aborted, reply, &QNetworkReply::abort);
        connect(this, &QObject::destroyed, reply, &QObject::deleteLater);
    }
    if (failure)
        setError(UnknownError, QStringLiteral("Null reply"));
    else
        connect(this, &QGeoRouteReply::aborted, [this](){ m_parsers = 0; });
}

QGeoRouteReplyNokia::~QGeoRouteReplyNokia()
{
}

void QGeoRouteReplyNokia::networkFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError
        && reply->error() != QNetworkReply::UnknownContentError) {
        return;
    }

    QGeoRouteXmlParser *parser = new QGeoRouteXmlParser(request());
    connect(parser, &QGeoRouteXmlParser::results,
            this, &QGeoRouteReplyNokia::appendResults);
    connect(parser, &QGeoRouteXmlParser::errorOccurred,
            this, &QGeoRouteReplyNokia::parserError);

    ++m_parsers;
    parser->parse(reply->readAll());
}

void QGeoRouteReplyNokia::networkError(QNetworkReply::NetworkError error)
{
    if (error == QNetworkReply::UnknownContentError)
        return;
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    setError(QGeoRouteReply::CommunicationError, reply->errorString());
    if (error != QNetworkReply::OperationCanceledError) // Any error not caused by abort()
        emit aborted(); // aborts all unfinished replies and sets m_parsers to 0
}

void QGeoRouteReplyNokia::appendResults(const QList<QGeoRoute> &routes)
{
    if (!m_parsers)
        return;

    --m_parsers;
    addRoutes(routes);

    if (!m_parsers)
        setFinished(true);
}

void QGeoRouteReplyNokia::parserError(const QString &errorString)
{
    Q_UNUSED(errorString);
    emit aborted();
    setError(QGeoRouteReply::ParseError,
             QCoreApplication::translate(NOKIA_PLUGIN_CONTEXT_NAME, RESPONSE_NOT_RECOGNIZABLE));
}

QT_END_NAMESPACE
