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
#ifndef QGEO_MOBILE_COUNTRY_TRACKER_H
#define QGEO_MOBILE_COUNTRY_TRACKER_H

#include <QObject>
#include <QPointer>

QT_BEGIN_NAMESPACE

class QNetworkInfo;

class QGeoUriProvider : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QGeoUriProvider)

public:
    QGeoUriProvider(QObject *parent,
                    const QVariantMap &parameters,
                    const QString &hostParameterName,
                    const QString &internationalHost,
                    const QString &localizedHost = QString());

    QString getCurrentHost() const;

private Q_SLOTS:
    void mobileCountryCodeChanged(int interfaceId, const QString& mcc);

private:
    bool isInternationalNetwork() const;
    void setCurrentHost(const QString &host);

    const QString m_internationalHost;
    const QString m_localizedHost;
    QString m_currentHost;
    QChar m_firstSubdomain;
    unsigned char m_maxSubdomains;
};

QT_END_NAMESPACE

#endif // QGEO_MOBILE_COUNTRY_TRACKER_H
