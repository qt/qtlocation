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

#ifndef GEOCODINGMANAGERENGINEESRI_H
#define GEOCODINGMANAGERENGINEESRI_H

#include <QGeoServiceProvider>
#include <QGeoCodingManagerEngine>
#include <QGeoCodeReply>

QT_BEGIN_NAMESPACE

class QNetworkAccessManager;

class GeoCodingManagerEngineEsri : public QGeoCodingManagerEngine
{
    Q_OBJECT

public:
    GeoCodingManagerEngineEsri(const QVariantMap &parameters, QGeoServiceProvider::Error *error,
                               QString *errorString);
    virtual ~GeoCodingManagerEngineEsri();

    QGeoCodeReply *geocode(const QGeoAddress &address, const QGeoShape &bounds) override;
    QGeoCodeReply *geocode(const QString &address, int limit, int offset,
                           const QGeoShape &bounds) override;
    QGeoCodeReply *reverseGeocode(const QGeoCoordinate &coordinate,
                                  const QGeoShape &bounds) override;

private Q_SLOTS:
    void replyFinished();
    void replyError(QGeoCodeReply::Error errorCode, const QString &errorString);

private:
    QNetworkAccessManager *m_networkManager;
    QByteArray m_userAgent;
};

QT_END_NAMESPACE

#endif // GEOCODINGMANAGERENGINEESRI_H
