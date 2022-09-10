/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeorouteparser_p.h"
#include "qgeorouteparser_p_p.h"
#include "qgeoroutesegment.h"
#include "qgeomaneuver.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

#include <QtCore/private/qobject_p.h>
#include <QtPositioning/private/qlocationutils_p.h>

QT_BEGIN_NAMESPACE

/*
    Private class implementations
*/

QGeoRouteParserPrivate::QGeoRouteParserPrivate()
    : QObjectPrivate()
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


