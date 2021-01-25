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

#ifndef QPLACESEARCHREPLYMAPBOX_H
#define QPLACESEARCHREPLYMAPBOX_H

#include <QtLocation/QPlaceSearchReply>
#include <QNetworkReply>

QT_BEGIN_NAMESPACE

class QNetworkReply;
class QPlaceManagerEngineMapbox;
class QPlaceResult;

class QPlaceSearchReplyMapbox : public QPlaceSearchReply
{
    Q_OBJECT

public:
    QPlaceSearchReplyMapbox(const QPlaceSearchRequest &request, QNetworkReply *reply,
                          QPlaceManagerEngineMapbox *parent);
    ~QPlaceSearchReplyMapbox();

public slots:
    void setError(QPlaceReply::Error errorCode, const QString &errorString);

private slots:
    void onReplyFinished();
    void onNetworkError(QNetworkReply::NetworkError error);
};

QT_END_NAMESPACE

#endif // QPLACESEARCHREPLYMAPBOX_H
