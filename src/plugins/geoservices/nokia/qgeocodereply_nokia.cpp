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

#include "qgeocodereply_nokia.h"
#include "qgeocodejsonparser.h"
#include "qgeoerror_messages.h"

#include <QtPositioning/QGeoShape>
#include <QtCore/QCoreApplication>

Q_DECLARE_METATYPE(QList<QGeoLocation>)

QT_BEGIN_NAMESPACE

// manualBoundsRequired will be true if the parser has to manually
// check if a given result lies within the viewport bounds,
// and false if the bounds information was able to be supplied
// to the server in the request (so it should not return any
// out-of-bounds results).
QGeoCodeReplyNokia::QGeoCodeReplyNokia(QNetworkReply *reply, int limit, int offset,
                                       const QGeoShape &viewport, bool manualBoundsRequired,
                                       QObject *parent)
:   QGeoCodeReply(parent), m_parsing(false), m_manualBoundsRequired(manualBoundsRequired)
{
    if (!reply) {
        setError(UnknownError, QStringLiteral("Null reply"));
        return;
    }
    qRegisterMetaType<QList<QGeoLocation> >();

    connect(reply, SIGNAL(finished()), this, SLOT(networkFinished()));
    connect(reply, SIGNAL(errorOccurred(QNetworkReply::NetworkError)),
            this, SLOT(networkError(QNetworkReply::NetworkError)));
    connect(this, &QGeoCodeReply::aborted, reply, &QNetworkReply::abort);
    connect(this, &QGeoCodeReply::aborted, [this](){ m_parsing = false; });
    connect(this, &QObject::destroyed, reply, &QObject::deleteLater);


    setLimit(limit);
    setOffset(offset);
    setViewport(viewport);
}

QGeoCodeReplyNokia::~QGeoCodeReplyNokia()
{
}

void QGeoCodeReplyNokia::networkFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError)
        return;

    QGeoCodeJsonParser *parser = new QGeoCodeJsonParser; // QRunnable, autoDelete = true.
    if (m_manualBoundsRequired)
        parser->setBounds(viewport());
    connect(parser, SIGNAL(results(QList<QGeoLocation>)),
            this, SLOT(appendResults(QList<QGeoLocation>)));
    connect(parser, SIGNAL(error(QString)), this, SLOT(parseError(QString)));

    m_parsing = true;
    parser->parse(reply->readAll());
}

void QGeoCodeReplyNokia::networkError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error);

    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    setError(QGeoCodeReply::CommunicationError, reply->errorString());
}

void QGeoCodeReplyNokia::appendResults(const QList<QGeoLocation> &locations)
{
    if (!m_parsing)
        return;

    m_parsing = false;
    setLocations(locations);
    setFinished(true);
}

void QGeoCodeReplyNokia::parseError(const QString &errorString)
{
    Q_UNUSED(errorString);

    setError(QGeoCodeReply::ParseError,
             QCoreApplication::translate(NOKIA_PLUGIN_CONTEXT_NAME, RESPONSE_NOT_RECOGNIZABLE));
}

QT_END_NAMESPACE
