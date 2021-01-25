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

#ifndef QGEOCODINGMANAGER_NOKIA_H
#define QGEOCODINGMANAGER_NOKIA_H

#include "qgeoserviceproviderplugin_nokia.h"

#include <qgeoserviceprovider.h>
#include <qgeocodingmanagerengine.h>

#include <QLocale>

QT_BEGIN_NAMESPACE

class QGeoNetworkAccessManager;
class QGeoUriProvider;

class QGeoCodingManagerEngineNokia : public QGeoCodingManagerEngine
{
    Q_OBJECT
public:
    QGeoCodingManagerEngineNokia(QGeoNetworkAccessManager *networkManager,
                                 const QVariantMap &parameters,
                                 QGeoServiceProvider::Error *error,
                                 QString *errorString);
    ~QGeoCodingManagerEngineNokia();

    QGeoCodeReply *geocode(const QGeoAddress &address,
                             const QGeoShape &bounds);
    QGeoCodeReply *reverseGeocode(const QGeoCoordinate &coordinate,
                                    const QGeoShape &bounds);

    QGeoCodeReply *geocode(const QString &searchString,
                            int limit,
                            int offset,
                            const QGeoShape &bounds);

private Q_SLOTS:
    void placesFinished();
    void placesError(QGeoCodeReply::Error error, const QString &errorString);

private:
    static QString trimDouble(double degree, int decimalDigits = 10);
    QGeoCodeReply *geocode(QString requestString, const QGeoShape &bounds, bool manualBoundsRequired = true, int limit = -1, int offset = 0);
    QString languageToMarc(QLocale::Language language);
    QString getAuthenticationString() const;

    QGeoNetworkAccessManager *m_networkManager;
    QGeoUriProvider *m_uriProvider;
    QGeoUriProvider *m_reverseGeocodingUriProvider;
    QString m_token;
    QString m_applicationId;
};

QT_END_NAMESPACE

#endif
