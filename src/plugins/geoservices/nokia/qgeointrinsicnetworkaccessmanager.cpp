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

#include "qgeointrinsicnetworkaccessmanager.h"

#include <QDebug>
#include <QVariant>
#include <QNetworkProxy>
#include <QNetworkAccessManager>

QGeoIntrinsicNetworkAccessManager::QGeoIntrinsicNetworkAccessManager(QObject *parent)
: QGeoNetworkAccessManager(parent)
, m_networkManager(new QNetworkAccessManager(this))
{
}


void QGeoIntrinsicNetworkAccessManager::configure(const QMap<QString, QVariant> &parameters)
{
    QString proxy = parameters.value(QStringLiteral("proxy")).toString();
    if (proxy.isEmpty() && !m_customProxyToken.isEmpty())
        proxy = parameters.value(m_customProxyToken).toString();

    if (!proxy.isEmpty()) {
        if (proxy.toLower() != QStringLiteral("system")) {
            QUrl proxyUrl(proxy);
            if (proxyUrl.isValid()) {
                qDebug() << "Setting proxy to " << proxyUrl.toString();
                m_networkManager->setProxy(
                    QNetworkProxy(QNetworkProxy::HttpProxy,
                    proxyUrl.host(),
                    proxyUrl.port(8080),
                    proxyUrl.userName(),
                    proxyUrl.password()));
            }
        } else if (QNetworkProxy::applicationProxy().type() == QNetworkProxy::NoProxy) {
            QNetworkProxyFactory::setUseSystemConfiguration(true);
            qDebug() << "Setting system proxy.";
        }
    } else {
        qDebug() << "No proxy parameter specified.";
    }
}


void QGeoIntrinsicNetworkAccessManager::setCustomProxyToken(const QString& token)
{
    m_customProxyToken = token;
}

QNetworkReply* QGeoIntrinsicNetworkAccessManager::get(const QNetworkRequest& request)
{
    return m_networkManager->get(request);
}

QNetworkReply *QGeoIntrinsicNetworkAccessManager::post(const QNetworkRequest &request, const QByteArray &data)
{
    return m_networkManager->post(request, data);
}
