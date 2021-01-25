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

#ifndef QPLACECONTENTREPLYIMPL_H
#define QPLACECONTENTREPLYIMPL_H

#include <QtNetwork/QNetworkReply>
#include <QtLocation/QPlaceContentReply>

QT_BEGIN_NAMESPACE

class QPlaceManager;
class QPlaceManagerEngineNokiaV2;

class QPlaceContentReplyImpl : public QPlaceContentReply
{
    Q_OBJECT

public:
    QPlaceContentReplyImpl(const QPlaceContentRequest &request, QNetworkReply *reply,
                           QPlaceManagerEngineNokiaV2 *engine);
    ~QPlaceContentReplyImpl();

private slots:
    void setError(QPlaceReply::Error error_, const QString &errorString);
    void replyFinished();
    void replyError(QNetworkReply::NetworkError error);

private:
    QPlaceManagerEngineNokiaV2 *m_engine;
};

QT_END_NAMESPACE

#endif
