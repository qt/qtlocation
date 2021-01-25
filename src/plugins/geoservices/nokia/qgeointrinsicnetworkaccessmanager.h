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

#ifndef QGEOINTRINSICNETWORKACCESSMANAGER_H
#define QGEOINTRINSICNETWORKACCESSMANAGER_H

#include "qgeonetworkaccessmanager.h"

#include <QMap>

QT_BEGIN_NAMESPACE

class QNetworkAccessManager;

class QGeoIntrinsicNetworkAccessManager : public QGeoNetworkAccessManager
{
public:
    explicit QGeoIntrinsicNetworkAccessManager(QObject *parent = 0);
    QGeoIntrinsicNetworkAccessManager(const QVariantMap &parameters, const QString &token = QString(), QObject *parent = 0);

    virtual QNetworkReply *get(const QNetworkRequest &request);
    virtual QNetworkReply *post(const QNetworkRequest &request, const QByteArray &data);

private:
    void configure(const QVariantMap &parameters);

    const QString m_customProxyToken;
    QNetworkAccessManager *m_networkManager;
};

QT_END_NAMESPACE

#endif // QGEOINTRINSICNETWORKACCESSMANAGER_H
