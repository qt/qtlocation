/****************************************************************************
**
** Copyright (C) 2013-2018 Esri <contracts@esri.com>
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

#ifndef GEOROUTEREPLYESRI_H
#define GEOROUTEREPLYESRI_H

#include <QNetworkReply>
#include <QGeoRouteReply>

QT_BEGIN_NAMESPACE

class GeoRouteReplyEsri : public QGeoRouteReply
{
    Q_OBJECT

public:
    GeoRouteReplyEsri(QNetworkReply *reply, const QGeoRouteRequest &request, QObject *parent = nullptr);
    ~GeoRouteReplyEsri();

private Q_SLOTS:
    void networkReplyFinished();
    void networkReplyError(QNetworkReply::NetworkError error);
};

QT_END_NAMESPACE

#endif // GEOROUTEREPLYESRI_H
