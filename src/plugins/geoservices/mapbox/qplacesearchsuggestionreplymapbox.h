/****************************************************************************
**
** Copyright (C) 2017 Mapbox, Inc.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtFoo module of the Qt Toolkit.
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

#ifndef QPLACESEARCHSUGGESTIONREPLYMAPBOX_H
#define QPLACESEARCHSUGGESTIONREPLYMAPBOX_H

#include <QtLocation/QPlaceSearchSuggestionReply>
#include <QNetworkReply>

QT_BEGIN_NAMESPACE

class QNetworkReply;
class QPlaceManagerEngineMapbox;
class QPlaceResult;

class QPlaceSearchSuggestionReplyMapbox : public QPlaceSearchSuggestionReply
{
    Q_OBJECT

public:
    QPlaceSearchSuggestionReplyMapbox(QNetworkReply *reply, QPlaceManagerEngineMapbox *parent);
    ~QPlaceSearchSuggestionReplyMapbox();

public slots:
    void setError(QPlaceReply::Error errorCode, const QString &errorString);

private slots:
    void onReplyFinished();
    void onNetworkError(QNetworkReply::NetworkError error);
};

QT_END_NAMESPACE

#endif // QPLACESEARCHSUGGESTIONREPLYMAPBOX_H
