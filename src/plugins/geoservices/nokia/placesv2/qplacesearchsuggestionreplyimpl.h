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

#ifndef QPLACESEARCHSUGGESTIONREPLYIMPL_H
#define QPLACESEARCHSUGGESTIONREPLYIMPL_H

#include <QtNetwork/QNetworkReply>
#include <QtLocation/QPlaceSearchSuggestionReply>

QT_BEGIN_NAMESPACE

class QPlaceSearchSuggestionReplyImpl : public QPlaceSearchSuggestionReply
{
    Q_OBJECT

public:
    explicit QPlaceSearchSuggestionReplyImpl(QNetworkReply *reply, QObject *parent = 0);
    ~QPlaceSearchSuggestionReplyImpl();

private slots:
    void setError(QPlaceReply::Error error_, const QString &errorString);
    void replyFinished();
    void replyError(QNetworkReply::NetworkError error);
};

QT_END_NAMESPACE

#endif // QPLACESEARCHSUGGESTIONREPLYIMPL_H
