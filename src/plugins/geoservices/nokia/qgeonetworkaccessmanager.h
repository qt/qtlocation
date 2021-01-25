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

#ifndef QGEONETWORKACCESSMANAGER_H
#define QGEONETWORKACCESSMANAGER_H

#include <QObject>

QT_BEGIN_NAMESPACE

class QNetworkReply;
class QNetworkRequest;
class QByteArray;

class QGeoNetworkAccessManager : public QObject
{
    Q_OBJECT
public:
    virtual ~QGeoNetworkAccessManager() {}
    virtual QNetworkReply *get(const QNetworkRequest &request) = 0;
    virtual QNetworkReply *post(const QNetworkRequest &request, const QByteArray &data) = 0;

protected:
    QGeoNetworkAccessManager(QObject *parent) : QObject(parent) {}
};

QT_END_NAMESPACE

#endif // QGEONETWORKACCESSMANAGER_H
