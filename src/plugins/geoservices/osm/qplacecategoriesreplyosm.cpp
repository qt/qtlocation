// Copyright (C) 2016 Aaron McCarthy <mccarthy.aaron@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#include "qplacecategoriesreplyosm.h"

QT_BEGIN_NAMESPACE

QPlaceCategoriesReplyOsm::QPlaceCategoriesReplyOsm(QObject *parent)
:   QPlaceReply(parent)
{
}

QPlaceCategoriesReplyOsm::~QPlaceCategoriesReplyOsm()
{
}

void QPlaceCategoriesReplyOsm::emitFinished()
{
    setFinished(true);
    emit finished();
}

void QPlaceCategoriesReplyOsm::setError(QPlaceReply::Error errorCode, const QString &errorString)
{
    QPlaceReply::setError(errorCode, errorString);
    emit errorOccurred(errorCode, errorString);
}

QT_END_NAMESPACE
