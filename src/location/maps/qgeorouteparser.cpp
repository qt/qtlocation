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

#include "qgeorouteparser_p.h"
#include "qgeorouteparser_p_p.h"
#include "qgeoroutesegment.h"
#include "qgeomaneuver.h"

#include <QtCore/private/qobject_p.h>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtPositioning/private/qlocationutils_p.h>

QT_BEGIN_NAMESPACE

/*
    Private class implementations
*/

QGeoRouteParserPrivate::QGeoRouteParserPrivate() : QObjectPrivate(), trafficSide(QGeoRouteParser::RightHandTraffic)
{
}

QGeoRouteParserPrivate::~QGeoRouteParserPrivate()
{
}

/*
    Public class implementations
*/

QGeoRouteParser::~QGeoRouteParser()
{

}

QGeoRouteParser::QGeoRouteParser(QGeoRouteParserPrivate &dd, QObject *parent) : QObject(dd, parent)
{

}

QGeoRouteReply::Error QGeoRouteParser::parseReply(QList<QGeoRoute> &routes, QString &errorString, const QByteArray &reply) const
{
    Q_D(const QGeoRouteParser);
    return d->parseReply(routes, errorString, reply);
}

QUrl QGeoRouteParser::requestUrl(const QGeoRouteRequest &request, const QString &prefix) const
{
    Q_D(const QGeoRouteParser);
    return d->requestUrl(request, prefix);
}

QGeoRouteParser::TrafficSide QGeoRouteParser::trafficSide() const
{
    Q_D(const QGeoRouteParser);
    return d->trafficSide;
}

void QGeoRouteParser::setTrafficSide(QGeoRouteParser::TrafficSide trafficSide)
{
    Q_D(QGeoRouteParser);
    if (d->trafficSide == trafficSide) return;
    d->trafficSide = trafficSide;
    Q_EMIT trafficSideChanged(trafficSide);
}

QT_END_NAMESPACE


