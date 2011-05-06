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

#include <QList>
#include <QHash>

#include <qgeosatelliteinfo.h>

#include "qgeosatelliteinfosource_wince_p.h"
#include "qgeoinfothread_wince_p.h"

QTM_BEGIN_NAMESPACE

// ========== QGeoSatelliteInfoValidator ==========

QGeoSatelliteInfoValidator::QGeoSatelliteInfoValidator() : QGeoInfoValidator() {}

QGeoSatelliteInfoValidator::~QGeoSatelliteInfoValidator() {}

// Returns true if data contains at least the minimal amount of data we need to produce one of the
// QGeoSatelliteInfoSource signals, otherwise returns false.
bool QGeoSatelliteInfoValidator::valid(const GPS_POSITION &data) const
{
    if (((data.dwValidFields & GPS_VALID_SATELLITE_COUNT) == 0)
            || ((data.dwValidFields & GPS_VALID_SATELLITES_IN_VIEW) == 0)
            || ((data.dwValidFields & GPS_VALID_SATELLITES_IN_VIEW_PRNS) == 0)
            || ((data.dwValidFields & GPS_VALID_SATELLITES_IN_VIEW_SIGNAL_TO_NOISE_RATIO) == 0)) {
        return false;
    }
    return true;
}

// ========== QGeoSatelliteInfoSourceWinCE ==========

QGeoSatelliteInfoSourceWinCE::QGeoSatelliteInfoSourceWinCE(QObject *parent) : QGeoSatelliteInfoSource(parent)
{
    QGeoSatelliteInfoValidator *validator = new QGeoSatelliteInfoValidator();

    // The QGeoInfoThreadWinCE instance takes ownership of the validator.
    infoThread = new QGeoInfoThreadWinCE(validator, false, this);
    infoThread->setUpdateInterval(DefaultUpdateInterval);
    infoThread->start();

    // QGeoInfoThreadWinCE takes care of registering GPS_POSITION as a metatype.
    connect(infoThread, SIGNAL(dataUpdated(GPS_POSITION)), this, SLOT(dataUpdated(GPS_POSITION)));
    connect(infoThread, SIGNAL(updateTimeout()), this, SIGNAL(requestTimeout()));
}

QGeoSatelliteInfoSourceWinCE::~QGeoSatelliteInfoSourceWinCE()
{
    delete infoThread;
}

void QGeoSatelliteInfoSourceWinCE::startUpdates()
{
    infoThread->startUpdates();
}

void QGeoSatelliteInfoSourceWinCE::stopUpdates()
{
    infoThread->stopUpdates();
}

void QGeoSatelliteInfoSourceWinCE::requestUpdate(int timeout)
{
    if (timeout < 0)
        emit requestTimeout();
    else
        infoThread->requestUpdate(timeout);
}

/*
 This is _only_ called when QGeoSatelliteInfoValidator::valid() returns true for the position.
 This means that it is implied that:
 - (data.dwValidFields & GPS_VALID_SATELLITE_COUNT) != 0
 - (data.dwValidFields & GPS_VALID_SATELLITES_IN_VIEW) != 0
 - (data.dwValidFields & GPS_VALID_SATELLITES_IN_VIEW_PRNS) != 0
 - (data.dwValidFields & GPS_VALID_SATELLITES_IN_VIEW_SIGNAL_TO_NOISE_RATIO) != 0

 This guarantees that the newly created position will be valid.
 If the code is changed such that this is no longer guaranteed then this method will need to be
 updated to test for those conditions.
*/
void QGeoSatelliteInfoSourceWinCE::dataUpdated(GPS_POSITION data)
{
    // Satellites in view are hashed on the PRN values since the PRN value is how we
    // determine which of the satellites are in use.
    QHash<int, QGeoSatelliteInfo> satellitesInView;

    for (unsigned int i = 0; i < data.dwSatellitesInView; ++i) {
        QGeoSatelliteInfo satellite;

        satellite.setPrnNumber(data.rgdwSatellitesInViewPRNs[i]);
        satellite.setSignalStrength(data.rgdwSatellitesInViewSignalToNoiseRatio[i]);

        // The following properties are optional, and so are set if the data is present and valid
        // in the GPS_POSITION structure.
        if ((data.dwValidFields & GPS_VALID_SATELLITES_IN_VIEW_AZIMUTH) != 0) {
            satellite.setAttribute(QGeoSatelliteInfo::Azimuth,
                                   data.rgdwSatellitesInViewAzimuth[i]);
        }

        if ((data.dwValidFields & GPS_VALID_SATELLITES_IN_VIEW_ELEVATION) != 0) {
            satellite.setAttribute(QGeoSatelliteInfo::Elevation,
                                   data.rgdwSatellitesInViewElevation[i]);
        }

        satellitesInView.insert(satellite.prnNumber(), satellite);
    }

    emit satellitesInViewUpdated(satellitesInView.values());

    // If the PRN data for the satellites which were used is unavailable we are done...
    if ((data.dwValidFields & GPS_VALID_SATELLITES_USED_PRNS) == 0)
        return;

    // ...otherwise we construct the list of satellites which are in use and emit the appropriate
    // signal
    QList<QGeoSatelliteInfo> satellitesInUse;

    for (unsigned int i = 0; i < data.dwSatelliteCount; ++i) {
        int inUsePRN = data.rgdwSatellitesUsedPRNs[i];
        if (satellitesInView.contains(inUsePRN))
            satellitesInUse << satellitesInView.value(inUsePRN);
    }

    emit satellitesInUseUpdated(satellitesInUse);
}

#include "moc_qgeosatelliteinfosource_wince_p.cpp"
QTM_END_NAMESPACE
