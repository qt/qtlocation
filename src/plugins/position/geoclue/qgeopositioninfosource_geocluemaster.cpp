/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd.
** Contact: Aaron McCarthy <aaron.mccarthy@jollamobile.com>
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtPositioning module of the Qt Toolkit.
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

#include "qgeopositioninfosource_geocluemaster_p.h"

#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QSaveFile>
#include <QtCore/QStandardPaths>
#include <QtCore/QtNumeric>

#ifdef Q_LOCATION_GEOCLUE_DEBUG
#include <QDebug>
#endif

#include <dbus/dbus-glib.h>

QT_BEGIN_NAMESPACE

#define MINIMUM_UPDATE_INTERVAL 1000
#define UPDATE_TIMEOUT_COLD_START 120000

// Callback for position-changed -signal
static void position_changed (GeocluePosition      *position,
                              GeocluePositionFields fields,
                              int                   timestamp,
                              double                latitude,
                              double                longitude,
                              double                altitude,
                              GeoclueAccuracy      *accuracy,
                              gpointer              userdata) // Ptr to this
{
    Q_UNUSED(position);
    if (!(fields & GEOCLUE_POSITION_FIELDS_LATITUDE &&
          fields & GEOCLUE_POSITION_FIELDS_LONGITUDE)) {
        static_cast<QGeoPositionInfoSourceGeoclueMaster *>(userdata)->regularUpdateFailed();
    } else {
        static_cast<QGeoPositionInfoSourceGeoclueMaster *>(userdata)->regularUpdateSucceeded(
                    fields, timestamp, latitude, longitude, altitude, accuracy);
    }
}

// Callback for velocity-changed -signal
static void velocity_changed (GeoclueVelocity *velocity,
                              GeoclueVelocityFields fields,
                              int timestamp,
                              double speed,
                              double direction,
                              double climb,
                              gpointer userdata) // Ptr to this
{
    Q_UNUSED(velocity)
    Q_UNUSED(timestamp)
    Q_UNUSED(direction)
    Q_UNUSED(climb)
    if (!(fields & GEOCLUE_VELOCITY_FIELDS_SPEED)) {
        static_cast<QGeoPositionInfoSourceGeoclueMaster *>(userdata)->velocityUpdateFailed();
        return;
    }
    static_cast<QGeoPositionInfoSourceGeoclueMaster *>(userdata)->velocityUpdateSucceeded(speed);
}

// Callback for single async update
static void position_callback (GeocluePosition *pos, GeocluePositionFields fields, int timestamp,
                               double latitude, double longitude, double altitude,
                               GeoclueAccuracy *accuracy, GError *error, gpointer userdata)
{
    Q_UNUSED(pos);
    Q_UNUSED(accuracy);

    if (error)
        g_error_free(error);

    if (fields & GEOCLUE_POSITION_FIELDS_LATITUDE && fields & GEOCLUE_POSITION_FIELDS_LONGITUDE) {
        static_cast<QGeoPositionInfoSourceGeoclueMaster *>(userdata)->singleUpdateSucceeded(
                    fields, timestamp, latitude, longitude, altitude, accuracy);
    }
}

QGeoPositionInfoSourceGeoclueMaster::QGeoPositionInfoSourceGeoclueMaster(QObject *parent)
:   QGeoPositionInfoSource(parent), QGeoclueMaster(this), m_updateInterval(0), m_pos(0), m_vel(0),
    m_lastPositionIsFresh(false), m_lastVelocityIsFresh(false), m_lastVelocity(0),
    m_lastPositionFromSatellite(false), m_methods(AllPositioningMethods), m_running(false)
{
#ifndef QT_NO_DATASTREAM
    // Load the last known location
    QFile file(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) +
               QStringLiteral("/qtposition-geoclue"));
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream out(&file);
        out >> m_lastPosition;
    }
#endif

    m_requestTimer.setSingleShot(true);
    QObject::connect(&m_requestTimer, SIGNAL(timeout()), this, SLOT(requestUpdateTimeout()));
    QObject::connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(startUpdatesTimeout()));

    setPreferredPositioningMethods(AllPositioningMethods);
}

QGeoPositionInfoSourceGeoclueMaster::~QGeoPositionInfoSourceGeoclueMaster()
{
#ifndef QT_NO_DATASTREAM
    if (m_lastPosition.isValid()) {
        QSaveFile file(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) +
                       QStringLiteral("/qtposition-geoclue"));
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            QDataStream out(&file);
            // Only save position and timestamp.
            out << QGeoPositionInfo(m_lastPosition.coordinate(), m_lastPosition.timestamp());
            file.commit();
        }
    }
#endif

    if (m_pos)
        g_object_unref (m_pos);
    if (m_vel)
        g_object_unref(m_vel);
}

void QGeoPositionInfoSourceGeoclueMaster::velocityUpdateFailed()
{
#ifdef Q_LOCATION_GEOCLUE_DEBUG
        qDebug() << "QGeoPositionInfoSourceGeoclueMaster velocity update failed.";
#endif
    // Set the velocitydata non-fresh.
    m_lastVelocityIsFresh = false;
}

void QGeoPositionInfoSourceGeoclueMaster::velocityUpdateSucceeded(double speed)
{
#ifdef Q_LOCATION_GEOCLUE_DEBUG
        qDebug() << "QGeoPositionInfoSourceGeoclueMaster velocity update succeeded, speed: " << speed;
#endif
    // Store the velocity and mark it as fresh. Simple but hopefully adequate.
    m_lastVelocity = speed * 0.514444; // convert knots to m/s
    m_lastVelocityIsFresh = true;
}

void QGeoPositionInfoSourceGeoclueMaster::singleUpdateSucceeded(GeocluePositionFields fields,
                                                                int                   timestamp,
                                                                double                latitude,
                                                                double                longitude,
                                                                double                altitude,
                                                                GeoclueAccuracy      *accuracy)
{
    QGeoPositionInfo info = geoclueToPositionInfo(fields, timestamp, latitude, longitude, altitude, accuracy);
    m_lastPosition = info;
    if (m_requestTimer.isActive())
        m_requestTimer.stop();
    if (m_lastVelocityIsFresh)
        info.setAttribute(QGeoPositionInfo::GroundSpeed, m_lastVelocity); // assume groundspeed
#ifdef Q_LOCATION_GEOCLUE_DEBUG
        qDebug() << "QGeoPositionInfoSourceGeoclueMaster single update succeeded: ";
        qDebug() << "Lat, lon, alt, speed:" << info.coordinate().latitude() << info.coordinate().longitude() << info.coordinate().altitude() << info.attribute(QGeoPositionInfo::GroundSpeed);
#endif
    emit positionUpdated(info);

    // Only stop positioning if regular updates not active.
    if (!m_running) {
        cleanupPositionSource();
        releaseMasterClient();
    }
}

void QGeoPositionInfoSourceGeoclueMaster::regularUpdateFailed()
{
#ifdef Q_LOCATION_GEOCLUE_DEBUG
        qDebug() << "QGeoPositionInfoSourceGeoclueMaster regular update failed.";
#endif
    // Emit timeout and keep on listening in case error condition clears.
    // Currently this is emitted each time an error occurs, and thereby it assumes
    // that there does not come many erroneous updates from position source.
    // This assumption may be invalid.
    m_lastVelocityIsFresh = false;
    m_lastPositionIsFresh = false;
    if (m_updateTimer.isActive())
        emit updateTimeout();
}

void QGeoPositionInfoSourceGeoclueMaster::regularUpdateSucceeded(GeocluePositionFields fields,
                                                                 int                   timestamp,
                                                                 double                latitude,
                                                                 double                longitude,
                                                                 double                altitude,
                                                                 GeoclueAccuracy      *accuracy)
{
    m_lastPosition = geoclueToPositionInfo(fields, timestamp, latitude, longitude, altitude, accuracy);
    m_lastPositionIsFresh = true;
    if (m_lastVelocityIsFresh) {
        m_lastPosition.setAttribute(QGeoPositionInfo::GroundSpeed, m_lastVelocity); // assume groundspeed
        m_lastVelocityIsFresh = false;
    }
    // If a non-intervalled startUpdates has been issued, send an update.
    if (!m_updateTimer.isActive()) {
        m_lastPositionIsFresh = false;
        emit positionUpdated(m_lastPosition);
    }
#ifdef Q_LOCATION_GEOCLUE_DEBUG
        qDebug() << "QGeoPositionInfoSourceGeoclueMaster regular update succeeded: ";
        qDebug() << "Lat, lon, alt, speed:" << m_lastPosition.coordinate().latitude() << m_lastPosition.coordinate().longitude() << m_lastPosition.coordinate().altitude() << m_lastPosition.attribute(QGeoPositionInfo::GroundSpeed);
#endif
}

bool QGeoPositionInfoSourceGeoclueMaster::init()
{
    g_type_init ();

    return true;
}

void QGeoPositionInfoSourceGeoclueMaster::cleanupPositionSource()
{
    if (m_pos) {
        g_object_unref(m_pos);
        m_pos = 0;
    }
    if (m_vel) {
        g_object_unref(m_vel);
        m_vel = 0;
    }
}

void QGeoPositionInfoSourceGeoclueMaster::setUpdateInterval(int msec)
{
    msec = (((msec > 0) && (msec < minimumUpdateInterval())) || msec < 0)? minimumUpdateInterval() : msec;
    QGeoPositionInfoSource::setUpdateInterval(msec);
    m_updateInterval = msec;
    // If update timer is running, set the new interval
    if (m_updateTimer.isActive()) {
        m_updateTimer.setInterval(msec);
    }
}

void QGeoPositionInfoSourceGeoclueMaster::setPreferredPositioningMethods(PositioningMethods methods)
{
    PositioningMethods previousPreferredPositioningMethods = preferredPositioningMethods();
    QGeoPositionInfoSource::setPreferredPositioningMethods(methods);
    if (previousPreferredPositioningMethods == preferredPositioningMethods())
        return;

#ifdef Q_LOCATION_GEOCLUE_DEBUG
    qDebug() << "QGeoPositionInfoSourceGeoclueMaster requested to set methods to" << methods
             << ", and set them to:" << preferredPositioningMethods();
#endif

    m_lastPositionIsFresh = false;
    m_lastVelocityIsFresh = false;

    // Don't start Geoclue provider until necessary. Don't currently have a master client, no need
    // no recreate one.
    if (!hasMasterClient())
        return;

    // Free potential previous sources, because new requirements can't be set for the client
    // (creating a position object after changing requirements seems to fail).
    cleanupPositionSource();
    releaseMasterClient();

    // Restart Geoclue provider with new requirements.
    configurePositionSource();
}

QGeoPositionInfo QGeoPositionInfoSourceGeoclueMaster::lastKnownPosition(bool fromSatellitePositioningMethodsOnly) const
{
    if (fromSatellitePositioningMethodsOnly) {
        if (m_lastPositionFromSatellite)
            return m_lastPosition;
        else
            return QGeoPositionInfo();
    }
    return m_lastPosition;
}

QGeoPositionInfoSourceGeoclueMaster::PositioningMethods QGeoPositionInfoSourceGeoclueMaster::supportedPositioningMethods() const
{
    // There is no really knowing which methods the GeoClue master supports.
    return AllPositioningMethods;
}

void QGeoPositionInfoSourceGeoclueMaster::startUpdates()
{
    if (m_running) {
#ifdef Q_LOCATION_GEOCLUE_DEBUG
      qDebug() << "QGeoPositionInfoSourceGeoclueMaster already running";
#endif
        return;
    }

    m_running = true;

    // Start Geoclue provider.
    if (!hasMasterClient())
        configurePositionSource();

    if (m_updateInterval > 0) {
#ifdef Q_LOCATION_GEOCLUE_DEBUG
        qDebug() << "QGeoPositionInfoSourceGeoclueMaster startUpdates with interval: " << m_updateInterval;
#endif
        m_updateTimer.start(m_updateInterval);
    }

    // Emit last known position on start.
    if (m_lastPosition.isValid()) {
        QMetaObject::invokeMethod(this, "positionUpdated", Qt::QueuedConnection,
                                  Q_ARG(QGeoPositionInfo, m_lastPosition));
    }

    // m_pos and m_vel are likely to be invalid until Geoclue master selects a position provider.
    if (!m_pos)
        return;

    g_signal_connect (G_OBJECT (m_pos), "position-changed",
                      G_CALLBACK (position_changed),this);
    if (m_vel) {
        g_signal_connect (G_OBJECT (m_vel), "velocity-changed",
                          G_CALLBACK (velocity_changed),this);
    }
}

int QGeoPositionInfoSourceGeoclueMaster::minimumUpdateInterval() const {
    return MINIMUM_UPDATE_INTERVAL;
}

void QGeoPositionInfoSourceGeoclueMaster::stopUpdates()
{
    if (!m_running)
        return;

    if (m_updateTimer.isActive())
        m_updateTimer.stop();
    if (m_pos)
        g_signal_handlers_disconnect_by_func(G_OBJECT(m_pos), (void *)position_changed, this);
    if (m_vel)
        g_signal_handlers_disconnect_by_func(G_OBJECT(m_vel), (void *)velocity_changed, this);

    m_running = false;

    // Only stop positioning if single update not requested.
    if (!m_requestTimer.isActive()) {
        cleanupPositionSource();
        releaseMasterClient();
    }
}

void QGeoPositionInfoSourceGeoclueMaster::requestUpdate(int timeout)
{
    if (timeout < minimumUpdateInterval() && timeout != 0) {
        emit updateTimeout();
        return;
    }
    if (m_requestTimer.isActive()) {
#ifdef Q_LOCATION_GEOCLUE_DEBUG
      qDebug() << "QGeoPositionInfoSourceGeoclueMaster request timer was active, ignoring startUpdates.";
#endif
        return;
    }

    if (!hasMasterClient())
        configurePositionSource();

    // Create better logic for timeout value (specs leave it impl dependant).
    // Especially if there are active updates ongoing, there is no point of waiting
    // for whole cold start time.
    m_requestTimer.start(timeout ? timeout : UPDATE_TIMEOUT_COLD_START);

    if (m_pos)
        geoclue_position_get_position_async(m_pos, position_callback, this);
}

void QGeoPositionInfoSourceGeoclueMaster::requestUpdateTimeout()
{
#ifdef Q_LOCATION_GEOCLUE_DEBUG
    qDebug() << "QGeoPositionInfoSourceGeoclueMaster requestUpdate timeout occurred.";
#endif
    // If we end up here, there has not been valid position update.
    emit updateTimeout();

    // Only stop positioning if regular updates not active.
    if (!m_running) {
        cleanupPositionSource();
        releaseMasterClient();
    }
}

void QGeoPositionInfoSourceGeoclueMaster::startUpdatesTimeout()
{
#ifdef Q_LOCATION_GEOCLUE_DEBUG
    qDebug() << "QGeoPositionInfoSourceGeoclueMaster startUpdates timeout occurred.";
#endif
    // Check if there are position updates since last positionUpdated().
    // Do not however send timeout, that's reserved for signaling errors.
    if (m_lastPositionIsFresh) {
        emit positionUpdated(m_lastPosition);
        m_lastPositionIsFresh = false;
        m_lastVelocityIsFresh = false;
    }
}

void QGeoPositionInfoSourceGeoclueMaster::positionProviderChanged(const QByteArray &service, const QByteArray &path)
{
    if (m_pos)
        cleanupPositionSource();

    if (service.isEmpty() || path.isEmpty())
        return;

    m_pos = geoclue_position_new(service.constData(), path.constData());
    if (m_pos) {
        if (m_running) {
            g_signal_connect(G_OBJECT(m_pos), "position-changed",
                             G_CALLBACK(position_changed), this);
        }

        // Get the current position immediately.
        geoclue_position_get_position_async(m_pos, position_callback, this);

        m_vel = geoclue_velocity_new(service.constData(), path.constData());
        if (m_vel && m_running) {
            g_signal_connect(G_OBJECT(m_vel), "velocity-changed",
                             G_CALLBACK(velocity_changed), this);
        }
    }
}

bool QGeoPositionInfoSourceGeoclueMaster::configurePositionSource()
{
    switch (preferredPositioningMethods()) {
    case SatellitePositioningMethods:
        return createMasterClient(GEOCLUE_ACCURACY_LEVEL_DETAILED, GEOCLUE_RESOURCE_GPS);
    case NonSatellitePositioningMethods:
        return createMasterClient(GEOCLUE_ACCURACY_LEVEL_NONE, GeoclueResourceFlags(GEOCLUE_RESOURCE_CELL | GEOCLUE_RESOURCE_NETWORK));
    case AllPositioningMethods:
        return createMasterClient(GEOCLUE_ACCURACY_LEVEL_NONE, GEOCLUE_RESOURCE_ALL);
    default:
        qWarning("GeoPositionInfoSourceGeoClueMaster unknown preferred method.");
    }

    return false;
}

// Helper function to convert data into a QGeoPositionInfo
QGeoPositionInfo QGeoPositionInfoSourceGeoclueMaster::geoclueToPositionInfo(
                                               GeocluePositionFields fields,
                                               int                   timestamp,
                                               double                latitude,
                                               double                longitude,
                                               double                altitude,
                                               GeoclueAccuracy *     accuracy)
{
    QGeoCoordinate coordinate(latitude, longitude);
    QDateTime dateTime = QDateTime();
    dateTime.setTime_t(timestamp);
    if (fields & GEOCLUE_POSITION_FIELDS_ALTITUDE) {
        coordinate.setAltitude(altitude);
    }
    QGeoPositionInfo info(coordinate, dateTime);
    if (accuracy) {
        double horizontalAccuracy = qQNaN();
        double verticalAccuracy = qQNaN();
        GeoclueAccuracyLevel accuracyLevel = GEOCLUE_ACCURACY_LEVEL_NONE;
        geoclue_accuracy_get_details(accuracy, &accuracyLevel, &horizontalAccuracy, &verticalAccuracy);
#ifdef Q_LOCATION_GEOCLUE_DEBUG
        qDebug() << "QGeoPositionInfoSourceGeoclueMaster::accuracy horizontal vertical level: " << horizontalAccuracy << verticalAccuracy << accuracyLevel;
#endif
        m_lastPositionFromSatellite = accuracyLevel & GEOCLUE_ACCURACY_LEVEL_DETAILED;

        if (!qIsNaN(horizontalAccuracy))
            info.setAttribute(QGeoPositionInfo::HorizontalAccuracy, horizontalAccuracy);
        if (!qIsNaN(verticalAccuracy))
            info.setAttribute(QGeoPositionInfo::VerticalAccuracy, verticalAccuracy);
    }
    return info;
}


QGeoPositionInfoSource::Error QGeoPositionInfoSourceGeoclueMaster::error() const
{
    return NoError;
}

#include "moc_qgeopositioninfosource_geocluemaster_p.cpp"

QT_END_NAMESPACE
