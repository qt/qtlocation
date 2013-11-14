/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd, author: Aaron McCarthy <aaron.mccarthy@jollamobile.com>
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

#include "qgeosatelliteinfosource_geocluemaster.h"

#define MINIMUM_UPDATE_INTERVAL 1000

QT_BEGIN_NAMESPACE

namespace
{

void satellite_changed(GeoclueSatellite *satellite, int timestamp, int satellite_used,
                       int satellite_visible, GArray *used_prn, GPtrArray *sat_info,
                       gpointer userdata)
{
    Q_UNUSED(satellite)

    QGeoSatelliteInfoSourceGeoclueMaster *source =
        static_cast<QGeoSatelliteInfoSourceGeoclueMaster *>(userdata);

    QList<int> usedPrns;
    for (unsigned int i = 0; i < used_prn->len; ++i)
        usedPrns.append(g_array_index(used_prn, int, i));

    QList<QGeoSatelliteInfo> satInfos;
    for (unsigned int i = 0; i < sat_info->len; ++i) {
        GValueArray *a = static_cast<GValueArray *>(g_ptr_array_index(sat_info, i));

        QGeoSatelliteInfo satInfo;

        satInfo.setSatelliteIdentifier(g_value_get_int(g_value_array_get_nth(a, 0)));
        satInfo.setAttribute(QGeoSatelliteInfo::Elevation,
                             g_value_get_int(g_value_array_get_nth(a, 1)));
        satInfo.setAttribute(QGeoSatelliteInfo::Azimuth,
                             g_value_get_int(g_value_array_get_nth(a, 2)));
        satInfo.setSignalStrength(g_value_get_int(g_value_array_get_nth(a, 3)));

        satInfos.append(satInfo);
    }

    source->satelliteChanged(timestamp, satellite_used, satellite_visible, usedPrns, satInfos);
}

void satellite_callback(GeoclueSatellite *satellite, int timestamp, int satellite_used,
                        int satellite_visible, GArray *used_prn, GPtrArray *sat_info,
                        GError *error, gpointer userdata)
{
    Q_UNUSED(satellite)

    if (error)
        g_error_free(error);

    QGeoSatelliteInfoSourceGeoclueMaster *source =
        static_cast<QGeoSatelliteInfoSourceGeoclueMaster *>(userdata);

    QList<int> usedPrns;
    for (unsigned int i = 0; i < used_prn->len; ++i)
        usedPrns.append(g_array_index(used_prn, int, i));

    QList<QGeoSatelliteInfo> satInfos;
    for (unsigned int i = 0; i < sat_info->len; ++i) {
        GValueArray *a = static_cast<GValueArray *>(g_ptr_array_index(sat_info, i));

        QGeoSatelliteInfo satInfo;

        satInfo.setSatelliteIdentifier(g_value_get_int(g_value_array_get_nth(a, 0)));
        satInfo.setAttribute(QGeoSatelliteInfo::Elevation,
                             g_value_get_int(g_value_array_get_nth(a, 1)));
        satInfo.setAttribute(QGeoSatelliteInfo::Azimuth,
                             g_value_get_int(g_value_array_get_nth(a, 2)));
        satInfo.setSignalStrength(g_value_get_int(g_value_array_get_nth(a, 3)));

        satInfos.append(satInfo);
    }

    source->requestUpdateFinished(timestamp, satellite_used, satellite_visible, usedPrns, satInfos);
}

}

QGeoSatelliteInfoSourceGeoclueMaster::QGeoSatelliteInfoSourceGeoclueMaster(QObject *parent)
:   QGeoSatelliteInfoSource(parent), QGeoclueMaster(this), m_sat(0)
{
    m_requestTimer.setSingleShot(true);
    connect(&m_requestTimer, SIGNAL(timeout()), this, SIGNAL(requestTimeout()));
}

QGeoSatelliteInfoSourceGeoclueMaster::~QGeoSatelliteInfoSourceGeoclueMaster()
{
    cleanupSatelliteSource();
}

bool QGeoSatelliteInfoSourceGeoclueMaster::init()
{
    g_type_init();

    return configureSatelliteSource();
}

int QGeoSatelliteInfoSourceGeoclueMaster::minimumUpdateInterval() const
{
    return MINIMUM_UPDATE_INTERVAL;
}

QGeoSatelliteInfoSource::Error QGeoSatelliteInfoSourceGeoclueMaster::error() const
{
    return NoError;
}

void QGeoSatelliteInfoSourceGeoclueMaster::startUpdates()
{
    if (!m_sat)
        return;

    g_signal_connect(G_OBJECT(m_sat), "satellite-changed", G_CALLBACK(satellite_changed), this);
}

void QGeoSatelliteInfoSourceGeoclueMaster::stopUpdates()
{
    if (!m_sat)
        g_signal_handlers_disconnect_by_func(G_OBJECT(m_sat), gpointer(satellite_changed), this);
}

void QGeoSatelliteInfoSourceGeoclueMaster::requestUpdate(int timeout)
{
    if ((timeout < minimumUpdateInterval() && timeout != 0) || !m_sat) {
        emit requestTimeout();
        return;
    }

    if (m_requestTimer.isActive())
        return;

    m_requestTimer.start(qMax(timeout, minimumUpdateInterval()));
    geoclue_satellite_get_satellite_async(m_sat, satellite_callback, this);
}

void QGeoSatelliteInfoSourceGeoclueMaster::satelliteChanged(int timestamp, int satellitesUsed,
                                                            int satellitesVisible,
                                                            const QList<int> &usedPrn,
                                                            const QList<QGeoSatelliteInfo> &satInfos)
{
    Q_UNUSED(timestamp)

    QList<QGeoSatelliteInfo> inUse;

    foreach (const QGeoSatelliteInfo &si, satInfos)
        if (usedPrn.contains(si.satelliteIdentifier()))
            inUse.append(si);

    if (satInfos.length() != satellitesVisible) {
        qWarning("QGeoSatelliteInfoSourceGeoclueMaster number of in view QGeoSatelliteInfos (%d) "
                 "does not match expected number of in view satellites (%d).", satInfos.length(),
                 satellitesVisible);
    }

    if (inUse.length() != satellitesUsed) {
        qWarning("QGeoSatelliteInfoSourceGeoclueMaster number of in use QGeoSatelliteInfos (%d) "
                 "does not match expected number of in use satellites (%d).", inUse.length(),
                 satellitesUsed);
    }

    m_inView = satInfos;
    emit satellitesInViewUpdated(m_inView);

    m_inUse = inUse;
    emit satellitesInUseUpdated(m_inUse);
}

void QGeoSatelliteInfoSourceGeoclueMaster::requestUpdateFinished(int timestamp, int satellitesUsed,
                                                                 int satellitesVisible,
                                                                 const QList<int> &usedPrn,
                                                                 const QList<QGeoSatelliteInfo> &satInfos)
{
    m_requestTimer.stop();
    satelliteChanged(timestamp, satellitesUsed, satellitesVisible, usedPrn, satInfos);
}

void QGeoSatelliteInfoSourceGeoclueMaster::positionProviderChanged(const QByteArray &service, const QByteArray &path)
{
    if (m_sat)
        cleanupSatelliteSource();

    m_sat = geoclue_satellite_new(service.constData(), path.constData());
    if (m_sat) {
        g_signal_connect(G_OBJECT(m_sat), "satellite-changed",
                         G_CALLBACK(satellite_changed), this);
    }
}

bool QGeoSatelliteInfoSourceGeoclueMaster::configureSatelliteSource()
{
    cleanupSatelliteSource();
    releaseMasterClient();

    return createMasterClient(GEOCLUE_ACCURACY_LEVEL_DETAILED, GEOCLUE_RESOURCE_GPS);
}

void QGeoSatelliteInfoSourceGeoclueMaster::cleanupSatelliteSource()
{
    if (m_sat) {
        g_object_unref(m_sat);
        m_sat = 0;
    }
}

QT_END_NAMESPACE
