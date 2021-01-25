/****************************************************************************
**
** Copyright (C) 2016 Aaron McCarthy <mccarthy.aaron@gmail.com>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtFoo module of the Qt Toolkit.
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

#ifndef QPLACESEARCHREPLYOSM_H
#define QPLACESEARCHREPLYOSM_H

#include <QtLocation/QPlaceSearchReply>
#include <QNetworkReply>

QT_BEGIN_NAMESPACE

class QNetworkReply;
class QPlaceManagerEngineOsm;
class QPlaceResult;

class QPlaceSearchReplyOsm : public QPlaceSearchReply
{
    Q_OBJECT

public:
    QPlaceSearchReplyOsm(const QPlaceSearchRequest &request, QNetworkReply *reply,
                          QPlaceManagerEngineOsm *parent);
    ~QPlaceSearchReplyOsm();

    QString requestUrl;

private slots:
    void setError(QPlaceReply::Error errorCode, const QString &errorString);
    void replyFinished();
    void networkError(QNetworkReply::NetworkError error);

private:
    QPlaceResult parsePlaceResult(const QJsonObject &item) const;
};

QT_END_NAMESPACE

#endif // QPLACESEARCHREPLYOSM_H
