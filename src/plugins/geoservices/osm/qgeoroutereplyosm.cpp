/****************************************************************************
**
** Copyright (C) 2013 Aaron McCarthy <mccarthy.aaron@gmail.com>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeoroutereplyosm.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtLocation/QGeoRouteSegment>
#include <QtLocation/QGeoManeuver>

QT_BEGIN_NAMESPACE

static QList<QGeoCoordinate> parsePolyline(const QByteArray &data)
{
    QList<QGeoCoordinate> path;

    bool parsingLatitude = true;

    int shift = 0;
    int value = 0;

    QGeoCoordinate coord(0, 0);

    for (int i = 0; i < data.length(); ++i) {
        unsigned char c = data.at(i) - 63;

        value |= (c & 0x1f) << shift;
        shift += 5;

        // another chunk
        if (c & 0x20)
            continue;

        int diff = (value & 1) ? ~(value >> 1) : (value >> 1);

        if (parsingLatitude) {
            coord.setLatitude(coord.latitude() + diff/1e5);
        } else {
            coord.setLongitude(coord.longitude() + diff/1e5);
            path.append(coord);
        }

        parsingLatitude = !parsingLatitude;

        value = 0;
        shift = 0;
    }

    return path;
}

static QGeoManeuver::InstructionDirection osrmInstructionDirection(const QString &instructionCode)
{
    if (instructionCode == QLatin1String("0"))
        return QGeoManeuver::NoDirection;
    else if (instructionCode == QLatin1String("1"))
        return QGeoManeuver::DirectionForward;
    else if (instructionCode == QLatin1String("2"))
        return QGeoManeuver::DirectionBearRight;
    else if (instructionCode == QLatin1String("3"))
        return QGeoManeuver::DirectionRight;
    else if (instructionCode == QLatin1String("4"))
        return QGeoManeuver::DirectionHardRight;
    else if (instructionCode == QLatin1String("5"))
        return QGeoManeuver::DirectionUTurnLeft;
    else if (instructionCode == QLatin1String("6"))
        return QGeoManeuver::DirectionHardLeft;
    else if (instructionCode == QLatin1String("7"))
        return QGeoManeuver::DirectionLeft;
    else if (instructionCode == QLatin1String("8"))
        return QGeoManeuver::DirectionBearLeft;
    else if (instructionCode == QLatin1String("9"))
        return QGeoManeuver::NoDirection;
    else if (instructionCode == QLatin1String("10"))
        return QGeoManeuver::DirectionForward;
    else if (instructionCode == QLatin1String("11"))
        return QGeoManeuver::NoDirection;
    else if (instructionCode == QLatin1String("12"))
        return QGeoManeuver::NoDirection;
    else if (instructionCode == QLatin1String("13"))
        return QGeoManeuver::NoDirection;
    else if (instructionCode == QLatin1String("14"))
        return QGeoManeuver::NoDirection;
    else if (instructionCode == QLatin1String("15"))
        return QGeoManeuver::NoDirection;
    else
        return QGeoManeuver::NoDirection;
}

const QString osrmInstructionText(const QString &instructionCode, const QString &wayname)
{
    if (instructionCode == QLatin1String("0"))
        return QString();
    else if (instructionCode == QLatin1String("1"))
        return QGeoRouteReplyOsm::tr("Go straight.");
    else if (instructionCode == QLatin1String("2"))
        return QGeoRouteReplyOsm::tr("Turn slightly right onto %1.").arg(wayname);
    else if (instructionCode == QLatin1String("3"))
        return QGeoRouteReplyOsm::tr("Turn right onto %1.").arg(wayname);
    else if (instructionCode == QLatin1String("4"))
        return QGeoRouteReplyOsm::tr("Make a sharp right onto %1.").arg(wayname);
    else if (instructionCode == QLatin1String("5"))
        return QGeoRouteReplyOsm::tr("When it is safe to do so, perform a U-turn.");
    else if (instructionCode == QLatin1String("6"))
        return QGeoRouteReplyOsm::tr("Make a sharp left onto %1.").arg(wayname);
    else if (instructionCode == QLatin1String("7"))
        return QGeoRouteReplyOsm::tr("Turn left onto %1.").arg(wayname);
    else if (instructionCode == QLatin1String("8"))
        return QGeoRouteReplyOsm::tr("Turn slightly left onto %1.").arg(wayname);
    else if (instructionCode == QLatin1String("9"))
        return QGeoRouteReplyOsm::tr("Reached waypoint.");
    else if (instructionCode == QLatin1String("10"))
        return QGeoRouteReplyOsm::tr("Head onto %1.").arg(wayname);
    else if (instructionCode == QLatin1String("11"))
        return QGeoRouteReplyOsm::tr("Enter the round about.");
    else if (instructionCode == QLatin1String("11-1"))
        return QGeoRouteReplyOsm::tr("At the round about take the 1st exit.");
    else if (instructionCode == QLatin1String("11-2"))
        return QGeoRouteReplyOsm::tr("At the round about take the 2nd exit.");
    else if (instructionCode == QLatin1String("11-3"))
        return QGeoRouteReplyOsm::tr("At the round about take the 3rd exit.");
    else if (instructionCode == QLatin1String("12"))
        return QGeoRouteReplyOsm::tr("Leave the round about.");
    else if (instructionCode == QLatin1String("13"))
        return QGeoRouteReplyOsm::tr("Stay on the round about.");
    else if (instructionCode == QLatin1String("14"))
        return QGeoRouteReplyOsm::tr("Start at the end of the street.");
    else if (instructionCode == QLatin1String("15"))
        return QGeoRouteReplyOsm::tr("You have reached your destination.");
    else
        return QGeoRouteReplyOsm::tr("Don't know what to say for '%1'").arg(instructionCode);
}

QGeoRouteReplyOsm::QGeoRouteReplyOsm(QNetworkReply *reply, const QGeoRouteRequest &request,
                                     QObject *parent)
:   QGeoRouteReply(request, parent), m_reply(reply)
{
    connect(m_reply, SIGNAL(finished()), this, SLOT(networkReplyFinished()));
    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(networkReplyError(QNetworkReply::NetworkError)));
}

QGeoRouteReplyOsm::~QGeoRouteReplyOsm()
{
    if (m_reply)
        m_reply->deleteLater();
}

void QGeoRouteReplyOsm::abort()
{
    if (!m_reply)
        return;

    m_reply->abort();

    m_reply->deleteLater();
    m_reply = 0;
}

static QGeoRoute constructRoute(const QByteArray &geometry, const QJsonArray &instructions,
                                const QJsonObject &summary)
{
    QGeoRoute route;

    QList<QGeoCoordinate> path = parsePolyline(geometry);

    QGeoRouteSegment firstSegment;
    int firstPosition = -1;

    int segmentPathLengthCount = 0;

    for (int i = instructions.count() - 1; i >= 0; --i) {
        QJsonArray instruction = instructions.at(i).toArray();

        if (instruction.count() != 8) {
            qWarning("Instruction does not contain enough fields.");
            continue;
        }

        const QString instructionCode = instruction.at(0).toString();
        const QString wayname = instruction.at(1).toString();
        double segmentLength = instruction.at(2).toDouble();
        int position = instruction.at(3).toDouble();
        int time = instruction.at(4).toDouble();
        //const QString segmentLengthString = instruction.at(5).toString();
        //const QString direction = instruction.at(6).toString();
        //double azimuth = instruction.at(7).toDouble();

        QGeoRouteSegment segment;
        segment.setDistance(segmentLength);

        QGeoManeuver maneuver;
        maneuver.setDirection(osrmInstructionDirection(instructionCode));
        maneuver.setDistanceToNextInstruction(segmentLength);
        maneuver.setInstructionText(osrmInstructionText(instructionCode, wayname));
        maneuver.setPosition(path.at(position));
        maneuver.setTimeToNextInstruction(time);

        segment.setManeuver(maneuver);

        if (firstPosition == -1)
            segment.setPath(path.mid(position));
        else
            segment.setPath(path.mid(position, firstPosition - position));

        segmentPathLengthCount += segment.path().length();

        segment.setTravelTime(time);

        segment.setNextRouteSegment(firstSegment);

        firstSegment = segment;
        firstPosition = position;
    }

    route.setDistance(summary.value(QStringLiteral("total_distance")).toDouble());
    route.setTravelTime(summary.value(QStringLiteral("total_time")).toDouble());
    route.setFirstRouteSegment(firstSegment);
    route.setPath(path);

    return route;
}

void QGeoRouteReplyOsm::networkReplyFinished()
{
    if (!m_reply)
        return;

    if (m_reply->error() != QNetworkReply::NoError) {
        setError(QGeoRouteReply::CommunicationError, m_reply->errorString());
        m_reply->deleteLater();
        return;
    }

    QJsonDocument document = QJsonDocument::fromJson(m_reply->readAll());

    if (document.isObject()) {
        QJsonObject object = document.object();

        //double version = object.value(QStringLiteral("version")).toDouble();
        //int status = object.value(QStringLiteral("status")).toDouble();
        //QString statusMessage = object.value(QStringLiteral("status_message")).toString();

        QJsonObject routeSummary = object.value(QStringLiteral("route_summary")).toObject();

        QByteArray routeGeometry =
            object.value(QStringLiteral("route_geometry")).toString().toLatin1();

        QJsonArray routeInstructions = object.value(QStringLiteral("route_instructions")).toArray();

        QGeoRoute route = constructRoute(routeGeometry, routeInstructions, routeSummary);

        QList<QGeoRoute> routes;
        routes.append(route);

        QJsonArray alternativeSummaries =
            object.value(QStringLiteral("alternative_summaries")).toArray();
        QJsonArray alternativeGeometries =
            object.value(QStringLiteral("alternative_geometries")).toArray();
        QJsonArray alternativeInstructions =
            object.value(QStringLiteral("alternative_instructions")).toArray();

        if (alternativeSummaries.count() == alternativeGeometries.count() &&
            alternativeSummaries.count() == alternativeInstructions.count()) {
            for (int i = 0; i < alternativeSummaries.count(); ++i) {
                route = constructRoute(alternativeGeometries.at(i).toString().toLatin1(),
                                       alternativeInstructions.at(i).toArray(),
                                       alternativeSummaries.at(i).toObject());
                //routes.append(route);
            }
        }

        setRoutes(routes);

        setFinished(true);
    } else {
        setError(QGeoRouteReply::ParseError, QLatin1String("Couldn't parse json."));
    }

    m_reply->deleteLater();
}

void QGeoRouteReplyOsm::networkReplyError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error)

    if (!m_reply)
        return;

    setError(QGeoRouteReply::CommunicationError, m_reply->errorString());

    m_reply->deleteLater();
    m_reply = 0;
}

QT_END_NAMESPACE
