/****************************************************************************
**
** Copyright (C) 2018 Denis Shienkov <denis.shienkov@gmail.com>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtPositioning module of the Qt Toolkit.
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

#include "qgeopositioninfosource_geoclue2_p.h"

#include <QtCore/QLoggingCategory>
#include <QtCore/QSaveFile>
#include <QtCore/QScopedPointer>
#include <QtCore/QTimer>
#include <QtDBus/QDBusPendingCallWatcher>

// Auto-generated D-Bus files.
#include <client_interface.h>
#include <location_interface.h>
#include <manager_interface.h>

Q_DECLARE_LOGGING_CATEGORY(lcPositioningGeoclue2)

QT_BEGIN_NAMESPACE

namespace {

// NOTE: Copied from the /usr/include/libgeoclue-2.0/gclue-client.h
enum GClueAccuracyLevel {
    GCLUE_ACCURACY_LEVEL_NONE = 0,
    GCLUE_ACCURACY_LEVEL_COUNTRY = 1,
    GCLUE_ACCURACY_LEVEL_CITY = 4,
    GCLUE_ACCURACY_LEVEL_NEIGHBORHOOD = 5,
    GCLUE_ACCURACY_LEVEL_STREET = 6,
    GCLUE_ACCURACY_LEVEL_EXACT = 8
};

const char GEOCLUE2_SERVICE_NAME[] = "org.freedesktop.GeoClue2";
const int MINIMUM_UPDATE_INTERVAL = 1000;
const int UPDATE_TIMEOUT_COLD_START = 120000;

static QString lastPositionFilePath()
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
            + QStringLiteral("/qtposition-geoclue2");
}

} // namespace

QGeoPositionInfoSourceGeoclue2::QGeoPositionInfoSourceGeoclue2(QObject *parent)
    : QGeoPositionInfoSource(parent)
    , m_requestTimer(new QTimer(this))
{
    qDBusRegisterMetaType<Timestamp>();

    restoreLastPosition();

    m_requestTimer->setSingleShot(true);
    connect(m_requestTimer, &QTimer::timeout,
            this, &QGeoPositionInfoSourceGeoclue2::requestUpdateTimeout);

    const auto flags = QDBusServiceWatcher::WatchForRegistration;
    const auto serviceWatcher = new QDBusServiceWatcher(
                QLatin1String(GEOCLUE2_SERVICE_NAME),
                QDBusConnection::systemBus(),
                flags,
                this);
    connect(serviceWatcher, &QDBusServiceWatcher::serviceRegistered,
            this, &QGeoPositionInfoSourceGeoclue2::createClient);

    if (const auto iface = QDBusConnection::systemBus().interface()) {
        if (iface->isServiceRegistered(QLatin1String(GEOCLUE2_SERVICE_NAME)))
            createClient(QLatin1String(GEOCLUE2_SERVICE_NAME));
        else
            iface->startService(QLatin1String(GEOCLUE2_SERVICE_NAME));
    } else {
        qCCritical(lcPositioningGeoclue2) << "D-Bus connection interface is not exists";
        setError(AccessError);
    }
}

QGeoPositionInfoSourceGeoclue2::~QGeoPositionInfoSourceGeoclue2()
{
    saveLastPosition();
}

void QGeoPositionInfoSourceGeoclue2::setUpdateInterval(int msec)
{
    QGeoPositionInfoSource::setUpdateInterval(msec);
    configureClient();
}

QGeoPositionInfo QGeoPositionInfoSourceGeoclue2::lastKnownPosition(bool fromSatellitePositioningMethodsOnly) const
{
    if (fromSatellitePositioningMethodsOnly && !m_lastPositionFromSatellite)
        return QGeoPositionInfo();
    return m_lastPosition;
}

QGeoPositionInfoSourceGeoclue2::PositioningMethods QGeoPositionInfoSourceGeoclue2::supportedPositioningMethods() const
{
    if (m_manager) {
        return m_manager->availableAccuracyLevel() == GCLUE_ACCURACY_LEVEL_EXACT
            ? AllPositioningMethods : NonSatellitePositioningMethods;
    }
    return NoPositioningMethods;
}

void QGeoPositionInfoSourceGeoclue2::setPreferredPositioningMethods(PositioningMethods methods)
{
    QGeoPositionInfoSource::setPreferredPositioningMethods(methods);
    configureClient();
}

int QGeoPositionInfoSourceGeoclue2::minimumUpdateInterval() const
{
    return MINIMUM_UPDATE_INTERVAL;
}

QGeoPositionInfoSource::Error QGeoPositionInfoSourceGeoclue2::error() const
{
    return m_error;
}

void QGeoPositionInfoSourceGeoclue2::startUpdates()
{
    if (m_running) {
        qCWarning(lcPositioningGeoclue2) << "Already running";
        return;
    }

    qCDebug(lcPositioningGeoclue2) << "Starting updates";
    m_running = true;

    if (m_lastPosition.isValid()) {
        QMetaObject::invokeMethod(this, "positionUpdated", Qt::QueuedConnection,
                                  Q_ARG(QGeoPositionInfo, m_lastPosition));
    }
}

void QGeoPositionInfoSourceGeoclue2::stopUpdates()
{
    if (!m_running) {
        qCWarning(lcPositioningGeoclue2) << "Already stopped";
        return;
    }

    qCDebug(lcPositioningGeoclue2) << "Stopping updates";
    m_running = false;

    // Only stop positioning if single update not requested.
    if (!m_requestTimer->isActive() && m_client)
        stopClient();
}

void QGeoPositionInfoSourceGeoclue2::requestUpdate(int timeout)
{
    if (timeout < minimumUpdateInterval() && timeout != 0) {
        emit updateTimeout();
        return;
    }

    if (m_requestTimer->isActive()) {
        qCDebug(lcPositioningGeoclue2) << "Request timer was active, ignoring startUpdates";
        return;
    }

    m_requestTimer->start(timeout ? timeout : UPDATE_TIMEOUT_COLD_START);
    if (!m_running)
        startClient();
}

void QGeoPositionInfoSourceGeoclue2::setError(QGeoPositionInfoSource::Error error)
{
    m_error = error;
    emit QGeoPositionInfoSource::error(m_error);
}

void QGeoPositionInfoSourceGeoclue2::restoreLastPosition()
{
#if !defined(QT_NO_DATASTREAM)
    const auto filePath = lastPositionFilePath();
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream out(&file);
        out >> m_lastPosition;
    }
#endif
}

void QGeoPositionInfoSourceGeoclue2::saveLastPosition()
{
#if !defined(QT_NO_DATASTREAM) && QT_CONFIG(temporaryfile)
    if (!m_lastPosition.isValid())
        return;

    const auto filePath = lastPositionFilePath();
    QSaveFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QDataStream out(&file);
        // Only save position and timestamp.
        out << QGeoPositionInfo(m_lastPosition.coordinate(), m_lastPosition.timestamp());
        file.commit();
    }
#endif
}

void QGeoPositionInfoSourceGeoclue2::createClient(const QString &service)
{
    if (service != QLatin1String(GEOCLUE2_SERVICE_NAME)) {
        qCCritical(lcPositioningGeoclue2) << "Registered unexpected service:"
                                          << service << ", expected:"
                                          << GEOCLUE2_SERVICE_NAME;
        setError(UnknownSourceError);
        return;
    }

    if (!m_manager) {
        m_manager = new OrgFreedesktopGeoClue2ManagerInterface(
                    QLatin1String(GEOCLUE2_SERVICE_NAME),
                    QStringLiteral("/org/freedesktop/GeoClue2/Manager"),
                    QDBusConnection::systemBus(),
                    this);
    }

    if (!m_manager->isValid()) {
        const auto error = m_manager->lastError();
        qCCritical(lcPositioningGeoclue2) << "Unable to create the manager object:"
                                          << error.name() << error.message();
        setError(AccessError);
        delete m_manager;
        m_manager = nullptr;
        return;
    }

    const QDBusPendingReply<QDBusObjectPath> reply = m_manager->GetClient();
    const auto watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            [this](QDBusPendingCallWatcher *watcher) {
        const QScopedPointer<QDBusPendingCallWatcher, QScopedPointerDeleteLater>
                scopedWatcher(watcher);
        const QDBusPendingReply<QDBusObjectPath> reply = *scopedWatcher;
        if (reply.isError()) {
            const auto error = reply.error();
            qCWarning(lcPositioningGeoclue2) << "Unable to obtain the client patch:"
                                             << error.name() + error.message();
            setError(AccessError);
        } else {
            const QString clientPath = reply.value().path();
            qCDebug(lcPositioningGeoclue2) << "Client path is:"
                                           << clientPath;
            delete m_client;
            m_client = new OrgFreedesktopGeoClue2ClientInterface(
                        QLatin1String(GEOCLUE2_SERVICE_NAME),
                        clientPath,
                        QDBusConnection::systemBus(),
                        this);
            if (!m_client->isValid()) {
                const auto error = m_client->lastError();
                qCCritical(lcPositioningGeoclue2) << "Unable to create the client object:"
                                                  << error.name() << error.message();
                setError(AccessError);
                delete m_client;
                m_client = nullptr;
            } else {
                connect(m_client, &OrgFreedesktopGeoClue2ClientInterface::LocationUpdated,
                        this, &QGeoPositionInfoSourceGeoclue2::handleNewLocation);

                // only start the client if someone asked for it already
                if (configureClient() && (m_running || m_requestTimer->isActive()))
                    startClient();
            }
        }
    });
}

void QGeoPositionInfoSourceGeoclue2::startClient()
{
    if (!m_client) {
        qCWarning(lcPositioningGeoclue2) << "Unable to start the client "
                                            "due to it is not created yet";
        return;
    }

    const QDBusPendingReply<> reply = m_client->Start();
    const auto watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            [this](QDBusPendingCallWatcher *watcher) {
        const QScopedPointer<QDBusPendingCallWatcher, QScopedPointerDeleteLater>
                scopedWatcher(watcher);
        const QDBusPendingReply<> reply = *scopedWatcher;
        if (reply.isError()) {
            const auto error = reply.error();
            qCCritical(lcPositioningGeoclue2) << "Unable to start the client:"
                                              << error.name() << error.message();
            setError(AccessError);
        } else {
            qCDebug(lcPositioningGeoclue2) << "Client successfully started";

            const QDBusObjectPath location = m_client->location();
            const QString path = location.path();
            if (path.isEmpty() || path == QLatin1String("/"))
                return;

            handleNewLocation({}, location);
        }
    });
}

void QGeoPositionInfoSourceGeoclue2::stopClient()
{
    if (!m_client) {
        qCWarning(lcPositioningGeoclue2) << "Unable to stop the client "
                                            "due to it is not created yet";
        return;
    }

    const QDBusPendingReply<> reply = m_client->Stop();
    const auto watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            [this](QDBusPendingCallWatcher *watcher) {
        const QScopedPointer<QDBusPendingCallWatcher, QScopedPointerDeleteLater>
                scopedWatcher(watcher);
        const QDBusPendingReply<> reply = *scopedWatcher;
        if (reply.isError()) {
            const auto error = reply.error();
            qCCritical(lcPositioningGeoclue2) << "Unable to stop the client:"
                                              << error.name() << error.message();
            setError(AccessError);
        } else {
            qCDebug(lcPositioningGeoclue2) << "Client successfully stopped";
        }
    });
}

bool QGeoPositionInfoSourceGeoclue2::configureClient()
{
    if (!m_client) {
        qCWarning(lcPositioningGeoclue2) << "Unable to configure the client "
                                            "due to it is not created yet";
        return false;
    }

    auto desktopId = QString::fromUtf8(qgetenv("QT_GEOCLUE_APP_DESKTOP_ID"));
    if (desktopId.isEmpty())
        desktopId = QCoreApplication::applicationName();
    if (desktopId.isEmpty()) {
        qCCritical(lcPositioningGeoclue2) << "Unable to configure the client "
                                             "due to the application desktop id "
                                             "is not set via QT_GEOCLUE_APP_DESKTOP_ID "
                                             "envirorment variable or QCoreApplication::applicationName";
        setError(AccessError);
        return false;
    }

    m_client->setDesktopId(desktopId);

    const auto msecs = updateInterval();
    const auto secs = msecs / 1000;
    m_client->setTimeThreshold(secs);

    const auto methods = preferredPositioningMethods();
    switch (methods) {
    case SatellitePositioningMethods:
        m_client->setRequestedAccuracyLevel(GCLUE_ACCURACY_LEVEL_EXACT);
        break;
    case NonSatellitePositioningMethods:
        m_client->setRequestedAccuracyLevel(GCLUE_ACCURACY_LEVEL_STREET);
        break;
    case AllPositioningMethods:
        m_client->setRequestedAccuracyLevel(GCLUE_ACCURACY_LEVEL_EXACT);
        break;
    default:
        m_client->setRequestedAccuracyLevel(GCLUE_ACCURACY_LEVEL_NONE);
        break;
    }

    return true;
}

void QGeoPositionInfoSourceGeoclue2::requestUpdateTimeout()
{
    qCDebug(lcPositioningGeoclue2) << "Request update timeout occurred";

    emit updateTimeout();

    if (!m_running)
        stopClient();
}

void QGeoPositionInfoSourceGeoclue2::handleNewLocation(const QDBusObjectPath &oldLocation,
                                                       const QDBusObjectPath &newLocation)
{
    if (m_requestTimer->isActive())
        m_requestTimer->stop();

    const auto oldPath = oldLocation.path();
    const auto newPath = newLocation.path();
    qCDebug(lcPositioningGeoclue2) << "Old location object path:" << oldPath;
    qCDebug(lcPositioningGeoclue2) << "New location object path:" << newPath;

    OrgFreedesktopGeoClue2LocationInterface location(
                QLatin1String(GEOCLUE2_SERVICE_NAME),
                newPath,
                QDBusConnection::systemBus(),
                this);
    if (!location.isValid()) {
        const auto error = location.lastError();
        qCCritical(lcPositioningGeoclue2) << "Unable to create the location object:"
                                          << error.name() << error.message();
    } else {
        QGeoCoordinate coordinate(location.latitude(),
                                  location.longitude());
        if (const auto altitude = location.altitude() > std::numeric_limits<double>::min())
            coordinate.setAltitude(altitude);

        const Timestamp ts = location.timestamp();
        if (ts.m_seconds == 0 && ts.m_microseconds == 0) {
            const auto dt = QDateTime::currentDateTime();
            m_lastPosition = QGeoPositionInfo(coordinate, dt);
        } else {
            auto dt = QDateTime::fromSecsSinceEpoch(ts.m_seconds);
            dt = dt.addMSecs(ts.m_microseconds / 1000);
            m_lastPosition = QGeoPositionInfo(coordinate, dt);
        }

        const auto accuracy = location.accuracy();
        // We assume that an accuracy as 0.0 means that it comes from a sattelite.
        m_lastPositionFromSatellite = qFuzzyCompare(accuracy, 0.0);

        m_lastPosition.setAttribute(QGeoPositionInfo::HorizontalAccuracy, accuracy);
        if (const auto speed = location.speed() >= 0.0)
            m_lastPosition.setAttribute(QGeoPositionInfo::GroundSpeed, speed);
        if (const auto heading = location.heading() >= 0.0)
            m_lastPosition.setAttribute(QGeoPositionInfo::Direction, heading);

        emit positionUpdated(m_lastPosition);
        qCDebug(lcPositioningGeoclue2) << "New position:" << m_lastPosition;
    }

    if (!m_running)
        stopClient();
}

QT_END_NAMESPACE
