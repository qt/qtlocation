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

#ifndef QGEOROUTEREPLY_NOKIA_H
#define QGEOROUTEREPLY_NOKIA_H

#include <qgeoroutereply.h>
#include <QNetworkReply>

QT_BEGIN_NAMESPACE

class QGeoRouteXmlParser;

class QGeoRouteReplyNokia : public QGeoRouteReply
{
    Q_OBJECT
public:
    QGeoRouteReplyNokia(const QGeoRouteRequest &request, const QList<QNetworkReply*> &replies, QObject *parent = 0);
    ~QGeoRouteReplyNokia();

private Q_SLOTS:
    void networkFinished();
    void networkError(QNetworkReply::NetworkError error);
    void appendResults(const QList<QGeoRoute> &routes);
    void parserError(const QString &errorString);

private:
    int m_parsers;
};

QT_END_NAMESPACE

#endif
