/****************************************************************************
**
** Copyright (C) 2016 Aaron McCarthy <mccarthy.aaron@gmail.com>
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

#ifndef QGEOCODEREPLYOSM_H
#define QGEOCODEREPLYOSM_H

#include <QtNetwork/QNetworkReply>
#include <QtLocation/QGeoCodeReply>
#include <QtLocation/private/qgeocodereply_p.h>

QT_BEGIN_NAMESPACE

class QGeoCodeReplyOsm : public QGeoCodeReply
{
    Q_OBJECT

public:
    explicit QGeoCodeReplyOsm(QNetworkReply *reply, bool includeExtraData = false, QObject *parent = 0);
    ~QGeoCodeReplyOsm();

private Q_SLOTS:
    void networkReplyFinished();
    void networkReplyError(QNetworkReply::NetworkError error);

private:
    bool m_includeExtraData = false;
};

class QGeoCodeReplyOsmPrivate : public QGeoCodeReplyPrivate
{
public:
    QGeoCodeReplyOsmPrivate();
    ~QGeoCodeReplyOsmPrivate();
    QVariantMap extraData() const override;

    QVariantMap m_extraData;
};

QT_END_NAMESPACE

#endif // QGEOCODEREPLYOSM_H
