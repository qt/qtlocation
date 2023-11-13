// Copyright (C) 2017 Mapbox, Inc.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#include "qplacecategoriesreplymapbox.h"

QT_BEGIN_NAMESPACE

QPlaceCategoriesReplyMapbox::QPlaceCategoriesReplyMapbox(QObject *parent)
:   QPlaceReply(parent)
{
}

QPlaceCategoriesReplyMapbox::~QPlaceCategoriesReplyMapbox()
{
}

void QPlaceCategoriesReplyMapbox::finish()
{
    setFinished(true);
    emit finished();
}

void QPlaceCategoriesReplyMapbox::setError(QPlaceReply::Error errorCode, const QString &errorString)
{
    QPlaceReply::setError(errorCode, errorString);
    emit errorOccurred(errorCode, errorString);
    finish();
}

QT_END_NAMESPACE
