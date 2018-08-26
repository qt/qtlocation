/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeoroutexmlparser.h"

#include <QXmlStreamReader>
#include <QStringList>
#include <QString>
#include <QtCore/QThreadPool>
#include <QDebug>

#include <QtPositioning/QGeoRectangle>
#include <QtPositioning/QGeoPath>
#include <QtLocation/QGeoRoute>
#include <QtLocation/private/qgeoroutesegment_p.h>

QT_BEGIN_NAMESPACE

QGeoDynamicSpeedInfoContainer::QGeoDynamicSpeedInfoContainer()
: trafficSpeed(0)
, baseSpeed(0)
, trafficTime(0)
, baseTime(0)
{}

QGeoRouteXmlParser::QGeoRouteXmlParser(const QGeoRouteRequest &request)
        : m_request(request)
{
}

QGeoRouteXmlParser::~QGeoRouteXmlParser()
{
}

void QGeoRouteXmlParser::parse(const QByteArray &data)
{
    m_data = data;
    QThreadPool::globalInstance()->start(this);
}

void QGeoRouteXmlParser::run()
{
    m_reader = new QXmlStreamReader(m_data);

    if (!parseRootElement())
        emit error(m_reader->errorString());
    else
        emit results(m_results);

    delete m_reader;
    m_reader = 0;
}

bool QGeoRouteXmlParser::parseRootElement()
{
    if (!m_reader->readNextStartElement()) {
        m_reader->raiseError("Expected a root element named \"CalculateRoute\" (no root element found).");
        return false;
    }

    if (m_reader->name() == QLatin1String("Error")) {
        QXmlStreamAttributes attributes = m_reader->attributes();
        if (attributes.value(QStringLiteral("type")) == QLatin1String("ApplicationError")
            && attributes.value("subtype") == QLatin1String("NoRouteFound"))
            return true;
    }

    bool updateroute = false;
    if (m_reader->name() != "CalculateRoute" && m_reader->name() != "GetRoute")  {
        m_reader->raiseError(QString("The root element is expected to have the name \"CalculateRoute\" or \"GetRoute\" (root element was named \"%1\").").arg(m_reader->name().toString()));
        return false;
    } else if (m_reader->name() == "GetRoute") {
        updateroute = true;
    }

    if (m_reader->readNextStartElement()) {
        if (m_reader->name() != "Response") {
            m_reader->raiseError(QString("Expected a element named \"Response\" (element was named \"%1\").").arg(m_reader->name().toString()));
            return false;
        }
    }

    while (m_reader->readNextStartElement() && !m_reader->hasError()) {
        if (m_reader->name() == "Route") {
            QGeoRoute route;
            route.setRequest(m_request);
            if (updateroute)
                route.setTravelMode(QGeoRouteRequest::TravelMode(int(m_request.travelModes())));
            if (!parseRoute(&route))
                continue; //route parsing failed move on to the next
            m_results.append(route);
        } else if (m_reader->name() == "Progress") {
            //TODO: updated route progress
            m_reader->skipCurrentElement();
        } else {
            m_reader->skipCurrentElement();
        }
    }

    return !m_reader->hasError();
}

bool QGeoRouteXmlParser::parseRoute(QGeoRoute *route)
{
    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "Route");
    m_maneuvers.clear();
    m_segments.clear();
    m_legs.clear();
    int legIndex = 0;
    m_reader->readNext();
    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == "Route") &&
           !m_reader->hasError()) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement) {
            if (m_reader->name() == "RouteId") {
                route->setRouteId(m_reader->readElementText());
            }
            //else if (m_reader->name() == "Waypoint") {
            //    succeeded = parseWaypoint(route);
            //}
            else if (m_reader->name() == "Mode") {
                if (!parseMode(route))
                    return false;
            } else if (m_reader->name() == "Shape") {
                QString elementName = m_reader->name().toString();
                QList<QGeoCoordinate> path;
                if (!parseGeoPoints(m_reader->readElementText(), &path, elementName))
                    return false;
                route->setPath(path);
            } else if (m_reader->name() == "BoundingBox") {
                QGeoRectangle bounds;
                if (!parseBoundingBox(bounds))
                    return false;
                route->setBounds(bounds);
            } else if (m_reader->name() == "Leg") {
                if (!parseLeg(legIndex++))
                    return false;
            } else if (m_reader->name() == "Summary") {
                if (!parseSummary(route))
                    return false;
            } else {
                m_reader->skipCurrentElement();
            }
        }
        m_reader->readNext();
    }

    if (m_reader->hasError())
        return false;

    return postProcessRoute(route);
}

bool QGeoRouteXmlParser::parseLeg(int legIndex)
{
    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == QStringLiteral("Leg"));
    QGeoRouteLeg leg;
    leg.setLegIndex(legIndex);
    m_reader->readNext();
    QList<QGeoManeuverContainer> maneuvers;
    QList<QGeoRouteSegmentContainer> links;
    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement
             && m_reader->name() == QStringLiteral("Leg")) &&
           !m_reader->hasError()) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement) {
            if (m_reader->name() == QStringLiteral("Maneuver")) {
                if (!parseManeuver(maneuvers))
                    return false;
            } else if (m_reader->name() == QStringLiteral("Link")) {
                if (!parseLink(links))
                    return false;
            } else if (m_reader->name() == "TravelTime") {
                leg.setTravelTime(qRound(m_reader->readElementText().toDouble()));
            } else if (m_reader->name() == "Length") {
                leg.setDistance(m_reader->readElementText().toDouble());
            } else {
                m_reader->skipCurrentElement();
            }
        }
        m_reader->readNext();
    }

    if (m_reader->hasError())
        return false;

    m_legs << leg;
    m_segments << links;
    m_maneuvers << maneuvers;
    return true;
}

static bool fuzzyCompare(const QGeoCoordinate &a, const QGeoCoordinate& b)
{
    return qFuzzyCompare(a.latitude(), b.latitude()) && qFuzzyCompare(a.longitude(), b.longitude());
}

bool QGeoRouteXmlParser::postProcessRoute(QGeoRoute *route)
{
    // The number of links is >> number of maneuvers.
    // Links have to be merged to have one segment per maneuver.
    QList<QGeoRouteSegment> routeSegments;
    QList<QHash<QString, QGeoRouteSegmentContainer>> linkMaps; // one map per leg. links are repeated inside legs, if needed.
    for (int i = 0; i < m_legs.size(); i++)
        linkMaps << QHash<QString, QGeoRouteSegmentContainer>();

    Q_ASSERT(m_maneuvers.size());
    Q_ASSERT(m_maneuvers.size() == m_segments.size());



    // Step 1: find the last maneuver
    QGeoManeuverContainer *lastManeuver = nullptr;
    for (int i = m_maneuvers.size() - 1; i >= 0; i--) {
        if (m_maneuvers[i].size()) {
            lastManeuver = &m_maneuvers[i].last();
            break;
        }
    }

    // Step 2: create a fake link for the very last maneuver, which is expected to have a null "ToLink"
    Q_ASSERT(lastManeuver && lastManeuver->maneuver.isValid());
    if (lastManeuver && lastManeuver->maneuver.isValid() && lastManeuver->toLink.isEmpty()) {
        QList<QGeoCoordinate> path;
        path.append(lastManeuver->maneuver.position());
        path.append(path.first());
        QGeoRouteSegmentContainer endSegment;
        endSegment.segment.setDistance(0);
        endSegment.segment.setTravelTime(0);
        endSegment.segment.setPath(path);
        endSegment.id = "LASTMANEUVER";
        lastManeuver->toLink = endSegment.id;
        m_segments.last().append(endSegment);
    }

    // Step 3: populate the linkMap
    for (int i = 0; i < m_segments.size(); i++) {
        auto &links = m_segments[i];
        for (auto &link: links)
            linkMaps[i][link.id] = link;
    }

    // Step 4: associate links to maneuvers
    QList<QHash<QString, QList<QGeoManeuverContainer>>> maneuverMaps;
    for (int i = 0; i < m_segments.size(); i++)
        maneuverMaps << QHash<QString, QList<QGeoManeuverContainer>>();
    for (int i = 0; i < m_maneuvers.size(); i++) {
        auto &maneuvers = m_maneuvers[i];
        for (int j = 0; j < maneuvers.size(); j++) {
            auto &maneuver = maneuvers[j];
            maneuver.legIndex = i;
            maneuver.first = !j;
            maneuver.last = j == maneuvers.size() - 1;
            maneuver.index = j;
            QVariantMap extendedAttributes;
            extendedAttributes["first"] = maneuver.first;
            extendedAttributes["last"] = maneuver.last;
            extendedAttributes["legIndex"] = i;
            extendedAttributes["id"] = maneuver.id;
            extendedAttributes["toLink"] = maneuver.toLink;
            extendedAttributes["index"] = j;
            maneuver.maneuver.setExtendedAttributes(extendedAttributes);
            if (!maneuver.toLink.isEmpty())
                maneuverMaps[i][maneuver.toLink].append(maneuver);
        }
    }

    // Step 5: inject maneuvers into links.
    // Maneuvers may not have ToLink elements. F.ex. last maneuvers.
    // Create links if needed for these maneuvers.
    for (int i = 0; i < maneuverMaps.size(); i++) {
        const bool lastLeg = i == maneuverMaps.size() - 1;
        auto &maneuverMap = maneuverMaps[i];
        for (const auto &k : maneuverMap.keys()) {
            QList<QGeoManeuverContainer> &maneuvers = maneuverMap[k];
            // maneuvers.size() can be greater than 1.
            // In this case, the assumption here is that the first maneuver in the list is associated
            // with the beginning of the geometry in that link
            // Subsequent maneuvers are to be intended from the maneuver coordinate to
            // the next maneuver coordinate

            QGeoManeuverContainer maneuver = maneuvers.first();
            auto &linkMap = linkMaps[i];
            Q_ASSERT(linkMap.contains(maneuver.toLink) // this is not true for the last maneuver of a leg,
                                                       // except the last maneuver of the last leg
                     || m_segments[maneuver.legIndex + 1].first().id == maneuver.toLink); // which should connect to the first link of the next leg.

            if (linkMap.contains(maneuver.toLink)) {
                if (maneuvers.size() == 1) {
                    linkMap[maneuver.toLink].segment.setManeuver(maneuver.maneuver);
                } else { // Multiple maneuvers along one link
                    // Split the segment, approximate the geometry
                    // ToDo: do proper path splitting
                    QGeoRouteSegmentContainer &segment = linkMap[maneuver.toLink];
                    QList<QGeoRouteSegmentContainer> &leg = m_segments[i];
                    int segmentIndex = leg.indexOf(segment);
                    QList<QGeoCoordinate> path = segment.segment.path();
                    path = { path.first(), maneuver.maneuver.position() };
                    segment.segment.setPath(path);
                    segment.segment.setManeuver(maneuver.maneuver);
                    for (int j = 1; j < maneuvers.size(); j++) {
                        segmentIndex++;
                        QGeoRouteSegmentContainer s;
                        s.segment.setManeuver(maneuvers[j].maneuver);
                        path = { path.last(), maneuvers[j].maneuver.position() };
                        s.segment.setPath(path);
                        s.id = segment.id + "_" + QString::number(j);
                        s.segment.setDistance(maneuvers[j].maneuver.distanceToNextInstruction());
                        s.segment.setTravelTime(maneuvers[j].maneuver.timeToNextInstruction());
                        maneuvers[j].toLink = s.id;
                        maneuverMap[s.id].append(maneuvers[j]);
                        leg.insert(segmentIndex, s);
                    }
                }
            } else {
                if (!maneuver.toLink.isEmpty()) {
                    if (maneuver.last) {
                        Q_ASSERT(!lastLeg);
                        Q_ASSERT(m_segments[maneuver.legIndex + 1].first().id == maneuver.toLink);
                        if (m_segments[maneuver.legIndex + 1].first().id == maneuver.toLink) {
                            // Step 4.1: deal with end-leg maneuvers
                            // verify it's only when one maneuver is last and one is first
                            Q_ASSERT(maneuvers.first().last);
                            maneuver = maneuvers.first();
                            QList<QGeoCoordinate> path;
                            path.append(maneuver.maneuver.position());
                            path.append(path.first());
                            QGeoRouteSegmentContainer endSegment;
                            endSegment.id = "end leg " + QString::number(maneuver.legIndex);
                            endSegment.segment.setDistance(maneuver.maneuver.distanceToNextInstruction());
                            endSegment.segment.setTravelTime(maneuver.maneuver.timeToNextInstruction());
                            endSegment.segment.setPath(path);
                            endSegment.segment.setManeuver(maneuver.maneuver);
                            m_segments[maneuver.legIndex].append(endSegment);
                            maneuverMap[endSegment.id].append(maneuver);
                        }
                    } else {
                        // If not last maneuver, toLink must point to a link within the current leg.
                        // If that's not the case, just ignore this maneuver.
                    }
                } else { // Linkless maneuver
                    Q_ASSERT(!maneuver.last);
                    if (maneuver.last) {
                        // Last maneuvers should connect to next leg!
                    } else {
                        // Subsequent maneuverless links should be checked, here, to see if
                        // one of the link start coordinate match the maneuver coordinate.
                        // if so, associate that maneuverless link with this maneuver.
                        // Otherwise, possibilities are:
                        // a) discard the maneuver
                        // b) find the closest link, find the closest point, split the link
                        // c) introduce a new link
                        // for the moment, use a).

                    }
                }
            }
        }
    }

    // Step 6: collapse links without maneuvers into links with maneuvers.
    QList<QList<QGeoRouteSegment>> legSegments;
    for (int i = 0; i < m_segments.size(); i++) {
        auto &links = m_segments[i];
        QList<QGeoRouteSegment> leg;
        QGeoRouteSegment segment;
        auto &maneuverMap = maneuverMaps[i];
        for (int lid = 0; lid < links.size(); lid++) {
            QGeoRouteSegmentContainer &link = links[lid];

            if (maneuverMap.contains(link.id)) {
                // this link is the start of a segment
                if (segment.isValid())
                    leg.append(segment);

                segment = link.segment;
            } else {
                // This link is not the start of any segment BUT it might be the start of
                // a linkless maneuver.
                // So first, check if the maneuver after segment.maneuver is linkless.
                // If so, check if the start point of this segment matches the maneuver crd.
                // If so, link the maneuver, and inject into this segment.
                if (leg.size()) {
                    bool ok = true;
                    auto &maneuvers = m_maneuvers[i];
                    const int lastManeuverIndex = leg.last().maneuver().extendedAttributes().value("index").toInt(&ok);
                    if (ok && lastManeuverIndex < maneuvers.size() - 1) {
                        QGeoManeuverContainer &nextManeuver = maneuvers[lastManeuverIndex + 1];
                        if (nextManeuver.toLink.isEmpty()
                                && fuzzyCompare(nextManeuver.maneuver.position(), link.segment.path().first())) {
                            if (segment.isValid())
                                leg.append(segment);

                            segment = link.segment;
                            nextManeuver.toLink = link.id;
                            segment.setManeuver(nextManeuver.maneuver);
                        }
                    }
                }

                // this link has no associated maneuvers: collapse it into previous.
                segment.setDistance(segment.distance() + link.segment.distance());
                segment.setTravelTime(segment.travelTime() + link.segment.travelTime());
                QList<QGeoCoordinate> path = segment.path();
                path.append(link.segment.path());
                segment.setPath(path);
            }
        }
        if (segment.isValid()) // Last segment
            leg.append(segment);
        legSegments.append(leg);
    }

    // Step 7: connect all segments.
    QGeoRouteSegment segment;
    QGeoRouteSegment firstSegment;
    for (auto &segments: legSegments) {
        for (int j = 0; j < segments.size(); j++) {
            if (segment.isValid()) {
                segment.setNextRouteSegment(segments[j]);
            } else {
                firstSegment = segments[j];
            }
            segment = segments[j];
            if (j == segments.size() - 1) {
                QGeoRouteSegmentPrivate *sp = QGeoRouteSegmentPrivate::get(segment);
                sp->setLegLastSegment(true);
            }
        }
    }

    if (firstSegment.isValid())
        route->setFirstRouteSegment(firstSegment);

    // Step 8: fill route legs.
    for (int i = 0; i < m_legs.size(); i++) {
        m_legs[i].setTravelMode(route->travelMode());
        m_legs[i].setRequest(route->request());
        m_legs[i].setOverallRoute(*route);
        m_legs[i].setLegIndex(i);

        m_legs[i].setFirstRouteSegment(legSegments[i].first());

        // handle path
        QList<QGeoCoordinate> path;
        QGeoRouteSegment s = m_legs[i].firstRouteSegment();
        while (s.isValid()) {
            path.append(s.path());
            if (s.isLegLastSegment())
                break;
            s = s.nextRouteSegment();
        }
        m_legs[i].setPath(path);
        m_legs[i].setBounds(QGeoPath(path).boundingGeoRectangle());
    }
    route->setRouteLegs(m_legs);
    m_legs.clear();
    m_maneuvers.clear();
    m_segments.clear();
    return true;
}

/*
bool QGeoRouteXmlParser::parseWaypoint(QGeoRoute *route)
{
    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "Waypoint");
    m_reader->readNext();
    QList<QGeoCoordinate> path(route->pathSummary());

    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == "Waypoint")) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement) {
            if (m_reader->name() == "MappedPosition") {
                QGeoCoordinate coordinates;
                if(!parseCoordinates(coordinates))
                    return false;
                path.append(coordinates);
            }
            else {
                m_reader->skipCurrentElement();
            }
        }
        m_reader->readNext();
    }
    route->setPathSummary(path);
    return true;
}
*/

bool QGeoRouteXmlParser::parseMode(QGeoRoute *route)
{
    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "Mode");
    m_reader->readNext();

    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == "Mode") &&
           !m_reader->hasError()) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement) {
            if (m_reader->name() == "TransportModes") {
                QString value = m_reader->readElementText();
                if (value == "car")
                    route->setTravelMode(QGeoRouteRequest::CarTravel);
                else if (value == "pedestrian")
                    route->setTravelMode(QGeoRouteRequest::PedestrianTravel);
                else if (value == "publicTransport")
                    route->setTravelMode(QGeoRouteRequest::PublicTransitTravel);
                else if (value == "bicycle")
                    route->setTravelMode(QGeoRouteRequest::BicycleTravel);
                else if (value == "truck")
                    route->setTravelMode(QGeoRouteRequest::TruckTravel);
                else {
                    // unsupported mode
                    m_reader->raiseError(QString("Unsupported travel mode '\"%1\"'").arg(value));
                    return false;
                }
            } else {
                m_reader->skipCurrentElement();
            }
        }
        m_reader->readNext();
    }
    return !m_reader->hasError();
}

bool QGeoRouteXmlParser::parseSummary(QGeoRoute *route)
{
    Q_ASSERT(route);
    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "Summary");
    m_reader->readNext();

    double baseTime = -1, trafficTime = -1;

    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == "Summary") &&
           !m_reader->hasError()) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement) {
            if (m_reader->name() == "Distance") {
                route->setDistance(m_reader->readElementText().toDouble());
            } else if (m_reader->name() == "TrafficTime") {
                trafficTime = m_reader->readElementText().toDouble();
            } else if (m_reader->name() == "BaseTime") {
                baseTime = m_reader->readElementText().toDouble();
            } else {
                m_reader->skipCurrentElement();
            }
        }
        m_reader->readNext();
    }

    if (m_reader->hasError())
        return false;

    if (trafficTime >= 0)
        route->setTravelTime(trafficTime);
    else if (baseTime >= 0)
        route->setTravelTime(baseTime);

    return true;
}

bool QGeoRouteXmlParser::parseCoordinates(QGeoCoordinate &coord)
{
    QString currentElement = m_reader->name().toString();
    m_reader->readNext();

    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == currentElement)  &&
           !m_reader->hasError()) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement) {
            QString name = m_reader->name().toString();
            QString value = m_reader->readElementText();
            if (name == "Latitude")
                coord.setLatitude(value.toDouble());
            else if (name == "Longitude")
                coord.setLongitude(value.toDouble());
        }
        m_reader->readNext();
    }

    return !m_reader->hasError();
}

bool QGeoRouteXmlParser::parseManeuver(QList<QGeoManeuverContainer> &maneuvers)
{
    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "Maneuver");

    if (!m_reader->attributes().hasAttribute("id")) {
        m_reader->raiseError("The element \"Maneuver\" did not have the required attribute \"id\".");
        return false;
    }
    QGeoManeuverContainer maneuverContainter;
    maneuverContainter.id = m_reader->attributes().value("id").toString();

    m_reader->readNext();
    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == "Maneuver") &&
           !m_reader->hasError()) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement) {
            if (m_reader->name() == "Position") {
                QGeoCoordinate coordinates;
                if (parseCoordinates(coordinates))
                    maneuverContainter.maneuver.setPosition(coordinates);
            } else if (m_reader->name() == "Instruction") {
                maneuverContainter.maneuver.setInstructionText(m_reader->readElementText());
            } else if (m_reader->name() == "ToLink") {
                maneuverContainter.toLink = m_reader->readElementText();
            } else if (m_reader->name() == "TravelTime") {
                maneuverContainter.maneuver.setTimeToNextInstruction(qRound(m_reader->readElementText().toDouble()));
            } else if (m_reader->name() == "Length") {
                maneuverContainter.maneuver.setDistanceToNextInstruction(m_reader->readElementText().toDouble());
            } else if (m_reader->name() == "Direction") {
                QString value = m_reader->readElementText();
                if (value == "forward")
                    maneuverContainter.maneuver.setDirection(QGeoManeuver::DirectionForward);
                else if (value == "bearRight")
                    maneuverContainter.maneuver.setDirection(QGeoManeuver::DirectionBearRight);
                else if (value == "lightRight")
                    maneuverContainter.maneuver.setDirection(QGeoManeuver::DirectionLightRight);
                else if (value == "right")
                    maneuverContainter.maneuver.setDirection(QGeoManeuver::DirectionRight);
                else if (value == "hardRight")
                    maneuverContainter.maneuver.setDirection(QGeoManeuver::DirectionHardRight);
                else if (value == "uTurnRight")
                    maneuverContainter.maneuver.setDirection(QGeoManeuver::DirectionUTurnRight);
                else if (value == "uTurnLeft")
                    maneuverContainter.maneuver.setDirection(QGeoManeuver::DirectionUTurnLeft);
                else if (value == "hardLeft")
                    maneuverContainter.maneuver.setDirection(QGeoManeuver::DirectionHardLeft);
                else if (value == "left")
                    maneuverContainter.maneuver.setDirection(QGeoManeuver::DirectionLeft);
                else if (value == "lightLeft")
                    maneuverContainter.maneuver.setDirection(QGeoManeuver::DirectionLightLeft);
                else if (value == "bearLeft")
                    maneuverContainter.maneuver.setDirection(QGeoManeuver::DirectionBearLeft);
                else
                    maneuverContainter.maneuver.setDirection(QGeoManeuver::NoDirection);
            } else {
                m_reader->skipCurrentElement();
            }
        }
        m_reader->readNext();
    }

    if (m_reader->hasError())
        return false;

    maneuvers.append(maneuverContainter);
    return true;
}

bool QGeoRouteXmlParser::parseLink(QList<QGeoRouteSegmentContainer> &links)
{
    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == QStringLiteral("Link"));
    m_reader->readNext();

    QGeoRouteSegmentContainer segmentContainer;

    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == QStringLiteral("Link")) &&
           !m_reader->hasError()) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement) {
            if (m_reader->name() == QStringLiteral("LinkId")) {
                segmentContainer.id = m_reader->readElementText();
            } else if (m_reader->name() == QStringLiteral("Shape")) {
                QString elementName = m_reader->name().toString();
                QList<QGeoCoordinate> path;
                parseGeoPoints(m_reader->readElementText(), &path, elementName);
                segmentContainer.segment.setPath(path);
            } else if (m_reader->name() == QStringLiteral("Length")) {
                segmentContainer.segment.setDistance(m_reader->readElementText().toDouble());
            } else if (m_reader->name() == QStringLiteral("Maneuver")) {
                segmentContainer.maneuverId = m_reader->readElementText();
            } else if (m_reader->name() == QStringLiteral("DynamicSpeedInfo")) {
                QGeoDynamicSpeedInfoContainer speedInfo;
                if (!parseDynamicSpeedInfo(speedInfo))
                    return false;
                const double time = speedInfo.trafficTime >= 0 ? speedInfo.trafficTime : speedInfo.baseTime;
                if (time >= 0)
                    segmentContainer.segment.setTravelTime(time);
            } else {
                m_reader->skipCurrentElement();
            }
        }
        m_reader->readNext();
    }

    if (m_reader->hasError())
        return false;
    links.append(segmentContainer);
    return true;
}

bool QGeoRouteXmlParser::parseGeoPoints(const QString &strPoints, QList<QGeoCoordinate> *geoPoints, const QString &elementName)
{
    QStringList rawPoints = strPoints.split(' ');

    for (int i = 0; i < rawPoints.length(); ++i) {
        QStringList coords = rawPoints[i].split(',');

        if (coords.length() != 2) {
            m_reader->raiseError(QString("Each of the space separated values of \"%1\" is expected to be a comma separated pair of coordinates (value was \"%2\")").arg(elementName).arg(rawPoints[i]));
            return false;
        }

        bool ok = false;
        QString latString = coords[0];
        double lat = latString.toDouble(&ok);

        if (!ok) {
            m_reader->raiseError(QString("The latitude portions of \"%1\" are expected to have a value convertable to a double (value was \"%2\")").arg(elementName).arg(latString));
            return false;
        }

        QString lngString = coords[1];
        double lng = lngString.toDouble(&ok);

        if (!ok) {
            m_reader->raiseError(QString("The longitude portions of \"%1\" are expected to have a value convertable to a double (value was \"%2\")").arg(elementName).arg(lngString));
            return false;
        }

        QGeoCoordinate geoPoint(lat, lng);
        geoPoints->append(geoPoint);
    }

    return true;
}

bool QGeoRouteXmlParser::parseBoundingBox(QGeoRectangle &bounds)
{
    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "BoundingBox");

    QGeoCoordinate tl;
    QGeoCoordinate br;

    m_reader->readNext();
    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == "BoundingBox") &&
           !m_reader->hasError()) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement) {
            if (m_reader->name() == "TopLeft") {
                QGeoCoordinate coordinates;
                if (parseCoordinates(coordinates))
                    tl = coordinates;
            } else if (m_reader->name() == "BottomRight") {
                QGeoCoordinate coordinates;
                if (parseCoordinates(coordinates))
                    br = coordinates;
            } else {
                m_reader->skipCurrentElement();
            }
        }
        m_reader->readNext();
    }

    if (m_reader->hasError())
        return false;

    if (tl.isValid() && br.isValid()) {
        bounds = QGeoRectangle(tl, br);
        return true;
    }

    return false;
}

bool QGeoRouteXmlParser::parseDynamicSpeedInfo(QGeoDynamicSpeedInfoContainer &speedInfo)
{
    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == QStringLiteral("DynamicSpeedInfo"));

    m_reader->readNext();
    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == QStringLiteral("DynamicSpeedInfo")) &&
           !m_reader->hasError()) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement) {
            if (m_reader->name() == QStringLiteral("TrafficSpeed")) {
                speedInfo.trafficSpeed = m_reader->readElementText().toDouble();
            } else if (m_reader->name() == QStringLiteral("TrafficTime")) {
                speedInfo.trafficTime = qRound(m_reader->readElementText().toDouble());
            } else if (m_reader->name() == QStringLiteral("BaseSpeed")) {
                speedInfo.baseSpeed = m_reader->readElementText().toDouble();
            } else if (m_reader->name() == QStringLiteral("BaseTime")) {
                speedInfo.baseTime = qRound(m_reader->readElementText().toDouble());
            } else {
                m_reader->skipCurrentElement();
            }
        }
        m_reader->readNext();
    }

    return !m_reader->hasError();
}

QT_END_NAMESPACE
