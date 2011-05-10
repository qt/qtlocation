/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
** This file is part of the Ovi services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file OVI_SERVICES_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Ovi services
** plugin source code.
**
****************************************************************************/

#include "qgeomapreply_nokia.h"
#include <QNetworkAccessManager>
#include <QNetworkCacheMetaData>
#include <QDateTime>

QGeoMapReplyNokia::QGeoMapReplyNokia(QNetworkReply *reply, const QGeoTiledMapRequest &request, QObject *parent)
        : QGeoTiledMapReply(request, parent),
        m_reply(reply)
{
    m_reply->setParent(this);
    connect(m_reply,
            SIGNAL(finished()),
            this,
            SLOT(networkFinished()));

    connect(m_reply,
            SIGNAL(error(QNetworkReply::NetworkError)),
            this,
            SLOT(networkError(QNetworkReply::NetworkError)));

    connect(m_reply,
            SIGNAL(destroyed()),
            this,
            SLOT(replyDestroyed()));
}

QGeoMapReplyNokia::~QGeoMapReplyNokia()
{
}

QNetworkReply* QGeoMapReplyNokia::networkReply() const
{
    return m_reply;
}

void QGeoMapReplyNokia::abort()
{
    if (!m_reply)
        return;

    m_reply->abort();
}

void QGeoMapReplyNokia::replyDestroyed()
{
    m_reply = 0;
}

void QGeoMapReplyNokia::networkFinished()
{
    if (!m_reply)
        return;

    if (m_reply->error() != QNetworkReply::NoError)
        return;

    QVariant fromCache = m_reply->attribute(QNetworkRequest::SourceIsFromCacheAttribute);
    setCached(fromCache.toBool());

    if (!isCached()) {
        QAbstractNetworkCache *cache = m_reply->manager()->cache();
        if (cache) {
            QNetworkCacheMetaData metaData = cache->metaData(m_reply->url());
            QDateTime exp = QDateTime::currentDateTime();
            exp = exp.addDays(14);
            metaData.setExpirationDate(exp);
            cache->updateMetaData(metaData);
        }
    }

    setMapImageData(m_reply->readAll());
    setMapImageFormat("PNG");
    setFinished(true);

    m_reply->deleteLater();
    m_reply = 0;
}

void QGeoMapReplyNokia::networkError(QNetworkReply::NetworkError error)
{
    if (!m_reply)
        return;

    if (error != QNetworkReply::OperationCanceledError)
        setError(QGeoTiledMapReply::CommunicationError, m_reply->errorString());
    setFinished(true);
    m_reply->deleteLater();
    m_reply = 0;
}
