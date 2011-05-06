/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "liblocationwrapper_p.h"

#include <QDateTime>

using namespace std;

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(LiblocationWrapper, LocationEngine)

LiblocationWrapper *LiblocationWrapper::instance()
{
    return LocationEngine();
}

LiblocationWrapper::LiblocationWrapper()
    : file(NULL),
    locationControl(NULL),
    locationDevice(NULL),
    errorHandlerId(0),
    posChangedId(0),
    origUpdateInterval(0),
    startcounter(0),
    validLastUpdate(false),
    validLastSatUpdate(false),
    locationState(LiblocationWrapper::Undefined) {}

LiblocationWrapper::~LiblocationWrapper()
{
    if (locationDevice)
        g_object_unref(locationDevice);
    if (locationControl)
        g_object_unref(locationControl);
}

bool LiblocationWrapper::inited()
{
    int retval = false;
    if (!(locationState & LiblocationWrapper::Inited)) {
        g_type_init();

        locationControl = location_gpsd_control_get_default();

        if (locationControl) {
            g_object_set(G_OBJECT(locationControl),
                         "preferred-method", LOCATION_METHOD_USER_SELECTED,
                         "preferred-interval", LOCATION_INTERVAL_1S,
                         NULL);
            locationDevice =
                (LocationGPSDevice*)g_object_new(LOCATION_TYPE_GPS_DEVICE,
                                                 NULL);

            if (locationDevice) {
                errorHandlerId =
                    g_signal_connect(G_OBJECT(locationControl), "error-verbose",
                                     G_CALLBACK(&locationError),
                                     static_cast<void*>(this));
                posChangedId =
                    g_signal_connect(G_OBJECT(locationDevice), "changed",
                                     G_CALLBACK(&locationChanged),
                                     static_cast<void*>(this));
                locationState = LiblocationWrapper::Inited;
                retval = true;
                startcounter = 0;
            }
        }
    } else {
        retval = true;
    }
    return retval;
}

void LiblocationWrapper::locationError(LocationGPSDevice *device,
                                       gint errorCode, gpointer data)
{
    Q_UNUSED(device);
    QString locationError;

    switch (errorCode) {
        case LOCATION_ERROR_USER_REJECTED_DIALOG:
            locationError = "User didn't enable requested methods";
            break;
        case LOCATION_ERROR_USER_REJECTED_SETTINGS:
            locationError = "User changed settings, which disabled location.";
            break;
        case LOCATION_ERROR_BT_GPS_NOT_AVAILABLE:
            locationError = "Problems with BT GPS";
            break;
        case LOCATION_ERROR_METHOD_NOT_ALLOWED_IN_OFFLINE_MODE:
            locationError = "Requested method is not allowed in offline mode";
            break;
        case LOCATION_ERROR_SYSTEM:
            locationError = "System error.";
            break;
        default:
            locationError = "Unknown error.";
    }

    qDebug() << "Location error:" << locationError;

    LiblocationWrapper *object;
    object = (LiblocationWrapper *)data;
    emit object->error();
}

void LiblocationWrapper::locationChanged(LocationGPSDevice *device,
        gpointer data)
{
    QGeoPositionInfo posInfo;
    QGeoCoordinate coordinate;
    QGeoSatelliteInfo satInfo;
    int satellitesInUseCount = 0;
    LiblocationWrapper *object;

    if (!data || !device) {
        return;
    }

    object = (LiblocationWrapper *)data;

    if (device) {
        if (device->fix) {
            if (device->fix->fields & LOCATION_GPS_DEVICE_TIME_SET) {
                posInfo.setTimestamp(QDateTime::fromTime_t(device->fix->time));
            }

            if (device->fix->fields & LOCATION_GPS_DEVICE_LATLONG_SET) {
                coordinate.setLatitude(device->fix->latitude);
                coordinate.setLongitude(device->fix->longitude);
                posInfo.setAttribute(QGeoPositionInfo::HorizontalAccuracy,
                                     device->fix->eph / 100.0);
                posInfo.setAttribute(QGeoPositionInfo::VerticalAccuracy,
                                     device->fix->epv);
            }

            if (device->fix->fields & LOCATION_GPS_DEVICE_ALTITUDE_SET) {
                coordinate.setAltitude(device->fix->altitude);
            }

            if (device->fix->fields & LOCATION_GPS_DEVICE_SPEED_SET) {
                posInfo.setAttribute(QGeoPositionInfo::GroundSpeed,
                                     device->fix->speed / 3.6);
            }

            if (device->fix->fields & LOCATION_GPS_DEVICE_CLIMB_SET) {
                posInfo.setAttribute(QGeoPositionInfo::VerticalSpeed,
                                     device->fix->climb);
            }

            if (device->fix->fields & LOCATION_GPS_DEVICE_TRACK_SET) {
                posInfo.setAttribute(QGeoPositionInfo::Direction,
                                     device->fix->track);
            }
        }

        if (device->satellites_in_view) {
            QList<QGeoSatelliteInfo> satsInView;
            QList<QGeoSatelliteInfo> satsInUse;
            unsigned int i;
            for (i = 0;i < device->satellites->len;i++) {
                LocationGPSDeviceSatellite *satData =
                    (LocationGPSDeviceSatellite *)g_ptr_array_index(device->satellites,
                            i);
                satInfo.setSignalStrength(satData->signal_strength);
                satInfo.setPrnNumber(satData->prn);
                satInfo.setAttribute(QGeoSatelliteInfo::Elevation,
                                     satData->elevation);
                satInfo.setAttribute(QGeoSatelliteInfo::Azimuth,
                                     satData->azimuth);

                satsInView.append(satInfo);
                if (satData->in_use) {
                    satellitesInUseCount++;
                    satsInUse.append(satInfo);
                }
            }

            if (!satsInView.isEmpty())
                object->satellitesInViewUpdated(satsInView);

            if (!satsInUse.isEmpty())
                object->satellitesInUseUpdated(satsInUse);
        }
    }

    posInfo.setCoordinate(coordinate);

    emit object->positionUpdated(posInfo);
}

QGeoPositionInfo LiblocationWrapper::lastKnownPosition(bool fromSatellitePositioningMethodsOnly) const
{
    QGeoPositionInfo posInfo;
    QGeoCoordinate coordinate;
    double time;
    double latitude;
    double longitude;
    double altitude;
    double speed;
    double track;
    double climb;

    GConfItem lastKnownPositionTime("/system/nokia/location/lastknown/time");
    GConfItem lastKnownPositionLatitude("/system/nokia/location/lastknown/latitude");
    GConfItem lastKnownPositionLongitude("/system/nokia/location/lastknown/longitude");
    GConfItem lastKnownPositionAltitude("/system/nokia/location/lastknown/altitude");
    GConfItem lastKnownPositionSpeed("/system/nokia/location/lastknown/speed");
    GConfItem lastKnownPositionTrack("/system/nokia/location/lastknown/track");
    GConfItem lastKnownPositionClimb("/system/nokia/location/lastknown/climb");

    if (validLastSatUpdate)
        return lastSatUpdate;

    if (!fromSatellitePositioningMethodsOnly)
        if (validLastUpdate)
            return lastUpdate;

    time = lastKnownPositionTime.value().toDouble();
    latitude = lastKnownPositionLatitude.value().toDouble();
    longitude = lastKnownPositionLongitude.value().toDouble();
    altitude = lastKnownPositionAltitude.value().toDouble();
    speed = lastKnownPositionSpeed.value().toDouble();
    track = lastKnownPositionTrack.value().toDouble();
    climb = lastKnownPositionClimb.value().toDouble();

    if (longitude && latitude) {
        coordinate.setLongitude(longitude);
        coordinate.setLatitude(latitude);
        if (altitude) {
            coordinate.setAltitude(altitude);
        }
        posInfo.setCoordinate(coordinate);
    }

    if (speed) {
        posInfo.setAttribute(QGeoPositionInfo::GroundSpeed, speed);
    }

    if (track) {
        posInfo.setAttribute(QGeoPositionInfo::Direction, track);
    }

    if (climb) {
        posInfo.setAttribute(QGeoPositionInfo::VerticalSpeed, climb);
    }

    // Only positions with time (3D) are provided.
    if (time) {
        posInfo.setTimestamp(QDateTime::fromTime_t(time));
        return posInfo;
    }

    return QGeoPositionInfo();
}

void LiblocationWrapper::satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &satellites)
{
    satsInView = satellites;
}

void LiblocationWrapper::satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &satellites)
{
    satsInUse = satellites;
}

QList<QGeoSatelliteInfo> LiblocationWrapper::satellitesInView()
{
    return satsInView;
}

QList<QGeoSatelliteInfo> LiblocationWrapper::satellitesInUse()
{
    return satsInUse;
}

void LiblocationWrapper::start() {
    startcounter++;

    if ((locationState & LiblocationWrapper::Inited) &&
            !(locationState & LiblocationWrapper::Started)) {
        if (!errorHandlerId) {
            errorHandlerId =
                g_signal_connect(G_OBJECT(locationControl), "error-verbose",
                                 G_CALLBACK(&locationError),
                                 static_cast<void*>(this));
        }

        if (!posChangedId) {
            posChangedId =
                g_signal_connect(G_OBJECT(locationDevice), "changed",
                                 G_CALLBACK(&locationChanged),
                                 static_cast<void*>(this));
        }

        location_gpsd_control_start(locationControl);

        locationState |= LiblocationWrapper::Started;
        locationState &= ~LiblocationWrapper::Stopped;
    }
}

void LiblocationWrapper::stop() {
    startcounter--;

    if (startcounter > 0)
        return;
    
    if ((locationState & (LiblocationWrapper::Started |
                          LiblocationWrapper::Inited)) &&
            !(locationState & LiblocationWrapper::Stopped)) {
        if (errorHandlerId)
            g_signal_handler_disconnect(G_OBJECT(locationControl),
                                        errorHandlerId);
        if (posChangedId)
            g_signal_handler_disconnect(G_OBJECT(locationDevice),
                                        posChangedId);
        errorHandlerId = 0;
        posChangedId = 0;
        startcounter = 0;
        location_gpsd_control_stop(locationControl);

        locationState &= ~LiblocationWrapper::Started;
        locationState |= LiblocationWrapper::Stopped;
    }
}

bool LiblocationWrapper::isActive() {
    if (locationState & LiblocationWrapper::Started)
        return true;
    else
        return false;
}

#include "moc_liblocationwrapper_p.cpp"
QTM_END_NAMESPACE

