/****************************************************************************
**
** Copyright (C) 2013-2018 Esri <contracts@esri.com>
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
    emit error(errorCode, errorString);
}

QT_END_NAMESPACE
