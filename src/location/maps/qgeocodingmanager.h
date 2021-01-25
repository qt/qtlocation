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

#ifndef QGEOCODINGMANAGER_H
#define QGEOCODINGMANAGER_H

#include <QtLocation/QGeoCodeReply>
#include <QtPositioning/QGeoRectangle>

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QMap>

QT_BEGIN_NAMESPACE

class QLocale;

class QGeoCodingManagerEngine;
class QGeoCodingManagerPrivate;

class Q_LOCATION_EXPORT QGeoCodingManager : public QObject
{
    Q_OBJECT
public:
    ~QGeoCodingManager();

    QString managerName() const;
    int managerVersion() const;

    QGeoCodeReply *geocode(const QGeoAddress &address,
                           const QGeoShape &bounds = QGeoShape());
    QGeoCodeReply *geocode(const QString &searchString,
                            int limit = -1,
                            int offset = 0,
                           const QGeoShape &bounds = QGeoShape());

    QGeoCodeReply *reverseGeocode(const QGeoCoordinate &coordinate,
                                  const QGeoShape &bounds = QGeoShape());

    void setLocale(const QLocale &locale);
    QLocale locale() const;

Q_SIGNALS:
    void finished(QGeoCodeReply *reply);
    void error(QGeoCodeReply *reply, QGeoCodeReply::Error error, QString errorString = QString());

private:
    explicit QGeoCodingManager(QGeoCodingManagerEngine *engine, QObject *parent = nullptr);

    QGeoCodingManagerPrivate *d_ptr;
    Q_DISABLE_COPY(QGeoCodingManager)

    friend class QGeoServiceProvider;
    friend class QGeoServiceProviderPrivate;
};

QT_END_NAMESPACE

#endif
