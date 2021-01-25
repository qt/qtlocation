/****************************************************************************
**
** Copyright (C) 2017 Mapbox, Inc.
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
**
**
**
****************************************************************************/

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
    emit error(errorCode, errorString);
    finish();
}

QT_END_NAMESPACE
