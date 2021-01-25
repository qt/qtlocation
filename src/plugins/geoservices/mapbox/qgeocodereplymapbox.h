/****************************************************************************
**
** Copyright (C) 2017 Mapbox, Inc.
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
**
**
**
****************************************************************************/

#ifndef QGEOCODEREPLYMAPBOX_H
#define QGEOCODEREPLYMAPBOX_H

#include <QtNetwork/QNetworkReply>
#include <QtLocation/QGeoCodeReply>

QT_BEGIN_NAMESPACE

class QGeoCodeReplyMapbox : public QGeoCodeReply
{
    Q_OBJECT

public:
    explicit QGeoCodeReplyMapbox(QNetworkReply *reply, QObject *parent = 0);
    ~QGeoCodeReplyMapbox();

private Q_SLOTS:
    void onNetworkReplyFinished();
    void onNetworkReplyError(QNetworkReply::NetworkError error);
};

QT_END_NAMESPACE

#endif // QGEOCODEREPLYMAPBOX_H
