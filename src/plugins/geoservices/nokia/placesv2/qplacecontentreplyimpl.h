// Copyright (C) 2015 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

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
