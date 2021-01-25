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

#ifndef QROUTEXMLPARSER_H
#define QROUTEXMLPARSER_H

#include <QtCore/QObject>
#include <QtCore/QRunnable>
#include <QtCore/QString>
#include <QtCore/QList>

#include <QtLocation/QGeoRouteRequest>
#include <QtLocation/QGeoRouteSegment>
#include <QtLocation/QGeoManeuver>
#include <QtLocation/qgeoroute.h>

QT_BEGIN_NAMESPACE

class QXmlStreamReader;
class QGeoRoute;
class QGeoCoordinate;
class QGeoRectangle;

class QGeoManeuverContainer
{
public:
    QGeoManeuver maneuver;
    QString id;
    QString toLink; // Id of the link this maneuver brings into
    int legIndex = 0;
    int index = 0;
    QList<QGeoCoordinate> path;
    bool first = false;
    bool last = false;
};

class QGeoRouteSegmentContainer
{
public:
    QGeoRouteSegment segment;
    QString id;
    QString maneuverId;

    bool operator ==(const QGeoRouteSegmentContainer &other) const
    {
        return ( segment == other.segment && id == other.id && maneuverId == other.maneuverId );
    }
};

class QGeoDynamicSpeedInfoContainer
{
public:
    QGeoDynamicSpeedInfoContainer();

public:
    double trafficSpeed;
    double baseSpeed;
    int trafficTime;
    int baseTime;
};

class QGeoRouteXmlParser : public QObject, public QRunnable
{
    Q_OBJECT

public:
    QGeoRouteXmlParser(const QGeoRouteRequest &request);
    ~QGeoRouteXmlParser();

    void parse(const QByteArray &data);
    void run();

signals:
    void results(const QList<QGeoRoute> &routes);
    void error(const QString &errorString);

private:
    bool parseRootElement();
    bool parseRoute(QGeoRoute *route);
    //bool parseWaypoint(QGeoRoute *route);
    bool parseCoordinates(QGeoCoordinate &coord);
    bool parseMode(QGeoRoute *route);
    bool parseSummary(QGeoRoute *route);
    bool parseGeoPoints(const QString &strPoints, QList<QGeoCoordinate> *geoPoints, const QString &elementName);
    bool parseLeg(int legIndex);
    bool parseManeuver(QList<QGeoManeuverContainer> &maneuvers);
    bool parseLink(QList<QGeoRouteSegmentContainer> &links);
    bool postProcessRoute(QGeoRoute *route);

    bool parseBoundingBox(QGeoRectangle &bounds);
    bool parseDynamicSpeedInfo(QGeoDynamicSpeedInfoContainer &speedInfo);

    QGeoRouteRequest m_request;
    QByteArray m_data;
    QXmlStreamReader *m_reader;

    QList<QGeoRoute> m_results;
    QList<QGeoRouteLeg> m_legs;
    QList<QList<QGeoManeuverContainer>> m_maneuvers;
    //QList<QList<QGeoRouteSegmentContainer>> m_segments;
};

QT_END_NAMESPACE

#endif
