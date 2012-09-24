/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#include <QtCore>

#ifdef Q_LOCATION_GEOCLUE_DEBUG
#include <QDebug>
#endif

#include "qgeopositioninfosource_geocluemaster_p.h"
#include <gconf/gconf-client.h>

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
static void position_callback (GeocluePosition      *pos,
                   GeocluePositionFields fields,
                   int                   timestamp,
                   double                latitude,
                   double                longitude,
                   double                altitude,
                   GeoclueAccuracy      *accuracy,
                   GError               *error,
                   gpointer              userdata)
{
    Q_UNUSED(pos);
    Q_UNUSED(accuracy);

    if (error)
        g_error_free (error);
    if (!(fields & GEOCLUE_POSITION_FIELDS_LATITUDE &&
                   fields & GEOCLUE_POSITION_FIELDS_LONGITUDE)) {
        static_cast<QGeoPositionInfoSourceGeoclueMaster *>(userdata)->singleUpdateFailed();

    } else {
        static_cast<QGeoPositionInfoSourceGeoclueMaster *>(userdata)->singleUpdateSucceeded(
                    fields, timestamp, latitude, longitude, altitude, accuracy);
    }
}

QGeoPositionInfoSourceGeoclueMaster::QGeoPositionInfoSourceGeoclueMaster(QObject *parent)
    : QGeoPositionInfoSource(parent), m_updateInterval(0), m_preferredResources(GEOCLUE_RESOURCE_ALL),
      m_preferredAccuracy(GEOCLUE_ACCURACY_LEVEL_NONE),
      m_client(0), m_pos(0), m_vel(0), m_lastPositionIsFresh(false), m_lastVelocityIsFresh(false),
      m_lastVelocity(0), m_lastPositionFromSatellite(false), m_methods(AllPositioningMethods)
{
    m_requestTimer.setSingleShot(true);
    QObject::connect(&m_requestTimer, SIGNAL(timeout()), this, SLOT(requestUpdateTimeout()));
    QObject::connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(startUpdatesTimeout()));
}

QGeoPositionInfoSourceGeoclueMaster::~QGeoPositionInfoSourceGeoclueMaster()
{
    if (m_pos)
        g_object_unref (m_pos);
    if (m_vel)
        g_object_unref(m_vel);
    if (m_client)
        g_object_unref (m_client);
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

void QGeoPositionInfoSourceGeoclueMaster::singleUpdateFailed()
{
#ifdef Q_LOCATION_GEOCLUE_DEBUG
        qDebug() << "QGeoPositionInfoSourceGeoclueMaster single update failed (requestUpdate)";
#endif
    if (m_requestTimer.isActive())
        m_requestTimer.stop();
    // Send timeout even if time wasn't up yet, because we are not trying again
    emit updateTimeout();
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

bool QGeoPositionInfoSourceGeoclueMaster::tryGPS()
{
    // Check if the gconf value is set properly
    GConfClient *client;
    gchar *device_name;
    client = gconf_client_get_default();
    device_name = gconf_client_get_string(client, "/apps/geoclue/master/org.freedesktop.Geoclue.GPSDevice", NULL);
    QString deviceName(QString::fromLatin1(device_name));
    g_object_unref(client);
    g_free(device_name);

    if (deviceName.isEmpty()) {
        return false;
    } else {
        // Check if the device exists (does nothing if a bluetooth address)
        if (deviceName.trimmed().at(0) == '/' && QFile::exists(deviceName.trimmed())) {
            return true;
        }
        return false;
    }
}

int QGeoPositionInfoSourceGeoclueMaster::init()
{
    g_type_init ();
    // Check if there is sense to try GPS
    if (tryGPS()) {
        m_preferredResources = GEOCLUE_RESOURCE_GPS;
        m_preferredAccuracy = GEOCLUE_ACCURACY_LEVEL_DETAILED;
        if (configurePositionSource() != -1) {
            return 0;
        } else {
            // If not successful, try to get any resource
            m_preferredResources = GEOCLUE_RESOURCE_ALL;
            m_preferredAccuracy = GEOCLUE_ACCURACY_LEVEL_NONE;
            return configurePositionSource();
        }
    } else {
        return configurePositionSource();
    }
}

int QGeoPositionInfoSourceGeoclueMaster::configurePositionSource()
{
    GeoclueMaster *master(0);
    GError *error = 0;
    // Free potential previous sources, because new requirements can't be set for the client
    // (creating a position object after changing requirements seems to fail).
    if (m_client) {
        g_object_unref (m_client);
        m_client = 0;
    }
    if (m_pos) {
        g_object_unref(m_pos);
        m_pos = 0;
    }
    if (m_vel) {
        g_object_unref(m_vel);
        m_vel = 0;
    }

    master = geoclue_master_get_default ();
    if (!master) {
        qCritical ("QGeoPositionInfoSourceGeoclueMaster error creating GeoclueMaster");
        return -1;
    }

    m_client = geoclue_master_create_client (master, NULL, &error);
    g_object_unref (master);

    if (!m_client) {
        qCritical ("QGeoPositionInfoSourceGeoclueMaster error creating GeoclueMasterClient.");
        if (error) {
            qCritical ("Geoclue error: %s", error->message);
            g_error_free (error);
        }
        return -1;
    }

    if (!geoclue_master_client_set_requirements (m_client,
                                                 m_preferredAccuracy,   // min_accuracy
                                                 0,                     // min_time
                                                 true,                  // require_updates (signals)
                                                 m_preferredResources,
                                                 &error)){
        qCritical ("QGeoPositionInfoSourceGeoclueMaster geoclue set_requirements failed.");
        if (error) {
            qCritical ("Geoclue error: %s", error->message);
            g_error_free (error);
        }
        g_object_unref (m_client);
        m_client = 0;
        return -1;
    }
    m_pos = geoclue_master_client_create_position (m_client, NULL);
    if (!m_pos) {
        qCritical("QGeoPositionInfoSourceGeoclueMaster failed to get a position object");
        g_object_unref (m_client);
        m_client = 0;
        return -1;
    }
    // Succeeding velocity is not mandatory. Master does not provide abstraction
    // for velocity provider, hence request Gypsy provider directly.
    m_vel = geoclue_velocity_new("org.freedesktop.Geoclue.Providers.Gypsy",
                                 "/org/freedesktop/Geoclue/Providers/Gypsy");
#ifdef Q_LOCATION_GEOCLUE_DEBUG
    if (m_vel == NULL)
        qDebug("QGeoPositionInfoSourceGeoclueMaster velocity provider (Gypsy) not available.");
#endif
    return 0;
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
    if (methods == m_methods)
        return;
    m_methods = methods;

    switch (methods) {
    case SatellitePositioningMethods:
        m_preferredResources = GEOCLUE_RESOURCE_GPS;
        m_preferredAccuracy = GEOCLUE_ACCURACY_LEVEL_DETAILED;
        break;
    case NonSatellitePositioningMethods:
        m_preferredResources = (GeoclueResourceFlags)(GEOCLUE_RESOURCE_CELL | GEOCLUE_RESOURCE_NETWORK);
        m_preferredAccuracy = GEOCLUE_ACCURACY_LEVEL_NONE;
        break;
    case AllPositioningMethods:
        m_preferredResources = GEOCLUE_RESOURCE_ALL;
        m_preferredAccuracy = GEOCLUE_ACCURACY_LEVEL_NONE;
        break;
    default:
        qWarning("GeoPositionInfoSourceGeoClueMaster unknown preferred method.");
        return;
    }
    QGeoPositionInfoSource::setPreferredPositioningMethods(methods);
#ifdef Q_LOCATION_GEOCLUE_DEBUG
    qDebug() << "QGeoPositionInfoSourceGeoclueMaster requested to set methods to, and set them to: " << methods << m_preferredResources;
#endif
    m_lastPositionIsFresh = false;
    m_lastVelocityIsFresh = false;
    int status = configurePositionSource();

    // If updates ongoing, connect to the new objects
    if (m_updateTimer.isActive()) {
        if (status != -1) {
            g_signal_connect (G_OBJECT (m_pos), "position-changed",
                              G_CALLBACK (position_changed),this);
            if (m_vel) {
                g_signal_connect (G_OBJECT (m_vel), "velocity-changed",
                                  G_CALLBACK (velocity_changed),this);
            }
        } else {
            // Changing source failed and there was active reques
            m_updateTimer.stop();
            emit updateTimeout();
        }
    }
    // If a request ongoing, ask it from new object
    if (m_requestTimer.isActive()) {
        if ( status != -1) {
            geoclue_position_get_position_async (m_pos,
                                                 (GeocluePositionCallback)position_callback,
                                                 this);
        } else {
            // Changing source failed and there was active reques
            m_requestTimer.stop();
            emit updateTimeout();
        }
    }
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
    if (m_updateTimer.isActive()) {
#ifdef Q_LOCATION_GEOCLUE_DEBUG
      qDebug() << "QGeoPositionInfoSourceGeoclueMaster timer was active, ignoring startUpdates: " << m_updateInterval;
#endif
        return;
    }
    if (!m_pos) {
        // May happen if source has been changed unsuccesfully
        emit updateTimeout();
        return;
    }
    if (m_updateInterval > 0) {
#ifdef Q_LOCATION_GEOCLUE_DEBUG
        qDebug() << "QGeoPositionInfoSourceGeoclueMaster startUpdates with interval: " << m_updateInterval;
#endif
        m_updateTimer.start(m_updateInterval);
    }
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
    if (m_updateTimer.isActive())
        m_updateTimer.stop();
    if (m_pos) {
        g_signal_handlers_disconnect_by_func(G_OBJECT(m_pos), (void *)position_changed, this);
    } if (m_vel) {
        g_signal_handlers_disconnect_by_func(G_OBJECT(m_vel), (void *)velocity_changed, this);
    }
}

void QGeoPositionInfoSourceGeoclueMaster::requestUpdate(int timeout)
{
    if ((timeout < minimumUpdateInterval() && timeout != 0) || !m_pos) {
        emit updateTimeout();
        return;
    }
    if (m_requestTimer.isActive()) {
#ifdef Q_LOCATION_GEOCLUE_DEBUG
      qDebug() << "QGeoPositionInfoSourceGeoclueMaster request timer was active, ignoring startUpdates.";
#endif
        return;
    }
    // Create better logic for timeout value (specs leave it impl dependant).
    // Especially if there are active updates ongoing, there is no point of waiting
    // for whole cold start time.
    if (timeout == 0)
        m_requestTimer.start(UPDATE_TIMEOUT_COLD_START);
    else
        m_requestTimer.start(timeout);
    geoclue_position_get_position_async (m_pos, (GeocluePositionCallback)position_callback,this);
}

void QGeoPositionInfoSourceGeoclueMaster::requestUpdateTimeout()
{
#ifdef Q_LOCATION_GEOCLUE_DEBUG
    qDebug() << "QGeoPositionInfoSourceGeoclueMaster requestUpdate timeout occurred.";
#endif
    // If we end up here, there has not been valid position update.
    emit updateTimeout();
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
        double horizontalAccuracy;
        double verticalAccuracy;
        GeoclueAccuracyLevel accuracyLevel;
        geoclue_accuracy_get_details(accuracy, &accuracyLevel, &horizontalAccuracy, &verticalAccuracy);
#ifdef Q_LOCATION_GEOCLUE_DEBUG
        qDebug() << "QGeoPositionInfoSourceGeoclueMaster::accuracy horizontal vertical level: " << horizontalAccuracy << verticalAccuracy << accuracyLevel;
#endif
        if (accuracyLevel & GEOCLUE_ACCURACY_LEVEL_DETAILED)
            m_lastPositionFromSatellite = true;
        else
            m_lastPositionFromSatellite = false;
        if (accuracyLevel != GEOCLUE_ACCURACY_LEVEL_NONE) {
            info.setAttribute(QGeoPositionInfo::HorizontalAccuracy, horizontalAccuracy);
            info.setAttribute(QGeoPositionInfo::VerticalAccuracy, verticalAccuracy);
        }
    }
    return info;
}


QGeoPositionInfoSource::Error QGeoPositionInfoSourceGeoclueMaster::error() const
{
    return UnknownSourceError;
}

#include "moc_qgeopositioninfosource_geocluemaster_p.cpp"

QT_END_NAMESPACE
