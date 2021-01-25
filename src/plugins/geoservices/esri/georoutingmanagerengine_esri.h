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

#ifndef GEOROUTINGMANAGERENGINEESRI_H
#define GEOROUTINGMANAGERENGINEESRI_H

#include <QGeoServiceProvider>
#include <QGeoRoutingManagerEngine>

QT_BEGIN_NAMESPACE

class QNetworkAccessManager;

class GeoRoutingManagerEngineEsri : public QGeoRoutingManagerEngine
{
    Q_OBJECT

public:
    GeoRoutingManagerEngineEsri(const QVariantMap &parameters, QGeoServiceProvider::Error *error,
                                QString *errorString);
    virtual ~GeoRoutingManagerEngineEsri();

    QGeoRouteReply *calculateRoute(const QGeoRouteRequest &request) override;

private Q_SLOTS:
    void replyFinished();
    void replyError(QGeoRouteReply::Error errorCode, const QString &errorString);

private:
    QString preferedDirectionLangage();
    QString preferedDirectionsLengthUnits();

    QNetworkAccessManager *m_networkManager;
    QByteArray m_userAgent;
    QString m_token;
};

QT_END_NAMESPACE

#endif // GEOROUTINGMANAGERENGINEESRI_H
