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

#ifndef QPLACESEARCHREPLYHERE_H
#define QPLACESEARCHREPLYHERE_H

#include <QtNetwork/QNetworkReply>
#include <QtLocation/QPlaceSearchReply>

QT_BEGIN_NAMESPACE

class QPlaceManagerEngineNokiaV2;
class QPlaceResult;
class QPlaceProposedSearchResult;

class QPlaceSearchReplyHere : public QPlaceSearchReply
{
    Q_OBJECT

public:
    explicit QPlaceSearchReplyHere(const QPlaceSearchRequest &request,
                                   QNetworkReply *reply,
                                   QPlaceManagerEngineNokiaV2 *parent);
    ~QPlaceSearchReplyHere();

private slots:
    void setError(QPlaceReply::Error error_, const QString &errorString);
    void replyFinished();
    void replyError(QNetworkReply::NetworkError error);

private:
    QPlaceResult parsePlaceResult(const QJsonObject &item) const;
    QPlaceProposedSearchResult parseSearchResult(const QJsonObject &item) const;

    QPlaceManagerEngineNokiaV2 *m_engine;
};

QT_END_NAMESPACE

#endif // QPLACESEARCHREPLYHERE_H
