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

#ifndef QGEOCODINGMANAGERENGINE_H
#define QGEOCODINGMANAGERENGINE_H

#include <QtCore/QObject>
#include <QtLocation/qlocationglobal.h>
#include <QtLocation/QGeoCodeReply>

QT_BEGIN_NAMESPACE

class QGeoAddress;
class QGeoShape;
class QGeoCodingManagerEnginePrivate;

class Q_LOCATION_EXPORT QGeoCodingManagerEngine : public QObject
{
    Q_OBJECT
public:
    explicit QGeoCodingManagerEngine(const QVariantMap &parameters, QObject *parent = nullptr);
    virtual ~QGeoCodingManagerEngine();

    QString managerName() const;
    int managerVersion() const;

    virtual QGeoCodeReply *geocode(const QGeoAddress &address, const QGeoShape &bounds);
    virtual QGeoCodeReply *geocode(const QString &address,
                                   int limit,
                                   int offset,
                                   const QGeoShape &bounds);
    virtual QGeoCodeReply *reverseGeocode(const QGeoCoordinate &coordinate,
                                          const QGeoShape &bounds);


    void setLocale(const QLocale &locale);
    QLocale locale() const;

Q_SIGNALS:
    void finished(QGeoCodeReply *reply);
    void error(QGeoCodeReply *reply, QGeoCodeReply::Error error, QString errorString = QString());

private:
    void setManagerName(const QString &managerName);
    void setManagerVersion(int managerVersion);

    QGeoCodingManagerEnginePrivate *d_ptr;
    Q_DISABLE_COPY(QGeoCodingManagerEngine)

    friend class QGeoServiceProvider;
    friend class QGeoServiceProviderPrivate;
};

QT_END_NAMESPACE

#endif
