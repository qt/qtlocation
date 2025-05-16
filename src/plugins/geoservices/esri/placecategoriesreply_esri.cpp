// Copyright (C) 2013-2018 Esri <contracts@esri.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#include "placecategoriesreply_esri.h"

QT_BEGIN_NAMESPACE

PlaceCategoriesReplyEsri::PlaceCategoriesReplyEsri(QObject *parent) :
    QPlaceReply(parent)
{
}

PlaceCategoriesReplyEsri::~PlaceCategoriesReplyEsri()
{
}

void PlaceCategoriesReplyEsri::emitFinished()
{
    setFinished(true);
    emit finished();
}

void PlaceCategoriesReplyEsri::setError(QPlaceReply::Error errorCode, const QString &errorString)
{
    QPlaceReply::setError(errorCode, errorString);
    emit errorOccurred(errorCode, errorString);
}

QT_END_NAMESPACE
