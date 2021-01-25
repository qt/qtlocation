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

#ifndef UNSUPPORTED_REPLIES_H
#define UNSUPPORTED_REPLIES_H

#include <QtLocation/QPlaceIdReply>

QT_BEGIN_NAMESPACE

class QPlaceIdReplyImpl : public QPlaceIdReply
{
    Q_OBJECT

public:
    QPlaceIdReplyImpl(QPlaceIdReply::OperationType type, QObject *parent = 0);
    ~QPlaceIdReplyImpl();

    void setId(const QString &id);

private slots:
    void setError(QPlaceReply::Error error_, const QString &errorString);
};

QT_END_NAMESPACE

#endif
