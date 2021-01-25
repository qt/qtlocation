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

#include "qplaceidreplyimpl.h"

QT_BEGIN_NAMESPACE

QPlaceIdReplyImpl::QPlaceIdReplyImpl(QPlaceIdReply::OperationType type, QObject *parent)
:   QPlaceIdReply(type, parent)
{
}

QPlaceIdReplyImpl::~QPlaceIdReplyImpl()
{
}

void QPlaceIdReplyImpl::setId(const QString &id)
{
    QPlaceIdReply::setId(id);
}

void QPlaceIdReplyImpl::setError(QPlaceReply::Error error_, const QString &errorString)
{
    if (error_ != QPlaceReply::NoError) {
        QPlaceIdReply::setError(error_, errorString);
        emit error(error_, errorString);
    }

    setFinished(true);
    emit finished();
}

QT_END_NAMESPACE
