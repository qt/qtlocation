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

#include "qplacecategoriesreplyhere.h"

QT_BEGIN_NAMESPACE

QPlaceCategoriesReplyHere::QPlaceCategoriesReplyHere(QObject *parent)
:   QPlaceReply(parent)
{
}

QPlaceCategoriesReplyHere::~QPlaceCategoriesReplyHere()
{
}

void QPlaceCategoriesReplyHere::emitFinished()
{
    setFinished(true);
    emit finished();
}

void QPlaceCategoriesReplyHere::setError(QPlaceReply::Error error_, const QString &errorString)
{
    QPlaceReply::setError(error_, errorString);
    emit error(error_, errorString);
}

QT_END_NAMESPACE
