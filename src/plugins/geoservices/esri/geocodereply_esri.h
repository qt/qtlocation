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

#ifndef GEOCODEREPLYESRI_H
#define GEOCODEREPLYESRI_H

#include <QNetworkReply>
#include <QGeoCodeReply>

QT_BEGIN_NAMESPACE

class GeoCodeReplyEsri : public QGeoCodeReply
{
    Q_OBJECT

public:
    enum OperationType
    {
        Geocode,
        ReverseGeocode
    };

public:
    GeoCodeReplyEsri(QNetworkReply *reply, OperationType operationType, QObject *parent = nullptr);
    ~GeoCodeReplyEsri();

    inline OperationType operationType() const;

private Q_SLOTS:
    void networkReplyFinished();
    void networkReplyError(QNetworkReply::NetworkError error);

    QGeoLocation parseAddress(const QJsonObject &object);
    QGeoLocation parseCandidate(const QJsonObject &candidate);

private:
    OperationType m_operationType;
};

inline GeoCodeReplyEsri::OperationType GeoCodeReplyEsri::operationType() const
{
    return m_operationType;
}

QT_END_NAMESPACE

#endif // GEOCODEREPLYESRI_H
