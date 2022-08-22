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

#include "jsonparserhelpers.h"
#include "qplacecontentreplyimpl.h"
#include "qplacecontentrequest.h"
#include "../qplacemanagerengine_nokiav2.h"
#include "../qgeoerror_messages.h"


#include <QCoreApplication>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

QT_BEGIN_NAMESPACE

QPlaceContentReplyImpl::QPlaceContentReplyImpl(const QPlaceContentRequest &request,
                                               QNetworkReply *reply,
                                               QPlaceManagerEngineNokiaV2 *engine)
    :   QPlaceContentReply(engine), m_engine(engine)
{
    Q_ASSERT(engine);
    if (!reply) {
        setError(UnknownError, QStringLiteral("Null reply"));
        return;
    }
    setRequest(request);

    connect(reply, &QNetworkReply::finished,
            this, &QPlaceContentReplyImpl::replyFinished);
    connect(reply, &QNetworkReply::errorOccurred,
            this, &QPlaceContentReplyImpl::replyError);
    connect(this, &QPlaceReply::aborted, reply, &QNetworkReply::abort);
    connect(this, &QObject::destroyed, reply, &QObject::deleteLater);
}

QPlaceContentReplyImpl::~QPlaceContentReplyImpl()
{
}

void QPlaceContentReplyImpl::setError(QPlaceReply::Error error_, const QString &errorString)
{
    QPlaceContentReply::setError(error_, errorString);
    emit errorOccurred(error_, errorString);
    setFinished(true);
    emit finished();
}

void QPlaceContentReplyImpl::replyFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError)
        return;

    QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
    if (!document.isObject()) {
        setError(ParseError, QCoreApplication::translate(NOKIA_PLUGIN_CONTEXT_NAME, PARSE_ERROR));
        return;
    }

    QJsonObject object = document.object();

    QPlaceContent::Collection collection;
    int totalCount;
    QPlaceContentRequest previous;
    QPlaceContentRequest next;

    parseCollection(request().contentType(), object, &collection, &totalCount,
                    &previous, &next, m_engine);

    setTotalCount(totalCount);
    setContent(collection);
    setPreviousPageRequest(previous);
    setNextPageRequest(next);

    setFinished(true);
    emit finished();
}

void QPlaceContentReplyImpl::replyError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (error == QNetworkReply::OperationCanceledError)
        setError(QPlaceReply::CancelError, QStringLiteral("Request cancelled"));
    else
        setError(QPlaceReply::CommunicationError, reply->errorString());
}

QT_END_NAMESPACE
