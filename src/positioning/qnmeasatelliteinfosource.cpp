/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
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

#include "qnmeasatelliteinfosource.h"
#include "private/qnmeasatelliteinfosource_p.h"
#include <QtPositioning/private/qgeosatelliteinfo_p.h>
#include <QtPositioning/private/qgeosatelliteinfosource_p.h>
#include <QtPositioning/private/qlocationutils_p.h>

#include <QIODevice>
#include <QBasicTimer>
#include <QTimerEvent>
#include <QTimer>
#include <array>
#include <QDebug>
#include <QtCore/QtNumeric>

QT_BEGIN_NAMESPACE

#if USE_NMEA_PIMPL
class QGeoSatelliteInfoPrivateNmea : public QGeoSatelliteInfoPrivate
{
public:
    QGeoSatelliteInfoPrivateNmea(const QGeoSatelliteInfoPrivate &other);
    QGeoSatelliteInfoPrivateNmea(const QGeoSatelliteInfoPrivateNmea &other);
    virtual ~QGeoSatelliteInfoPrivateNmea();

    QList<QByteArray> nmeaSentences;
};

QGeoSatelliteInfoPrivateNmea::QGeoSatelliteInfoPrivateNmea(const QGeoSatelliteInfoPrivate &other)
    : QGeoSatelliteInfoPrivate(other)
{
}

QGeoSatelliteInfoPrivateNmea::QGeoSatelliteInfoPrivateNmea(const QGeoSatelliteInfoPrivateNmea &other)
    : QGeoSatelliteInfoPrivate(other)
{
    nmeaSentences = other.nmeaSentences;
}

QGeoSatelliteInfoPrivateNmea::~QGeoSatelliteInfoPrivateNmea() {}
#else
typedef QGeoSatelliteInfoPrivate QGeoSatelliteInfoPrivateNmea;
#endif

QNmeaSatelliteInfoSourcePrivate::QNmeaSatelliteInfoSourcePrivate(QNmeaSatelliteInfoSource *parent, QNmeaSatelliteInfoSource::UpdateMode updateMode)
    : m_source(parent),
      m_updateMode(updateMode)
{
}

void QNmeaSatelliteInfoSourcePrivate::notifyNewUpdate()
{
    if (m_pendingUpdate.isValid() && m_pendingUpdate.isFresh()) {
        if (m_requestTimer && m_requestTimer->isActive()) { // User called requestUpdate()
            m_requestTimer->stop();
            emitUpdated(m_pendingUpdate, true);
        } else if (m_invokedStart) { // user called startUpdates()
            if (m_updateTimer && m_updateTimer->isActive()) { // update interval > 0
                // for periodic updates, only want the most recent update
                if (m_noUpdateLastInterval) {
                    // if the update was invalid when timerEvent was last called, a valid update
                    // should be sent ASAP
                    emitPendingUpdate(); // m_noUpdateLastInterval handled in there.
                }
            } else { // update interval <= 0, send anything new ASAP
                m_noUpdateLastInterval = !emitUpdated(m_pendingUpdate, false);
            }
        }
    }
}

void QNmeaSatelliteInfoSourcePrivate::processNmeaData(QNmeaSatelliteInfoUpdate &updateInfo)
{
    char buf[1024];
    qint64 size = m_device->readLine(buf, sizeof(buf));
    QList<int> satInUse;
    const bool satInUseParsed = m_source->parseSatellitesInUseFromNmea(buf, size, satInUse);
    if (satInUseParsed) {
        updateInfo.setSatellitesInUse(satInUse);
#if USE_NMEA_PIMPL
        updateInfo.gsa = QByteArray(buf, size);
        if (updateInfo.m_satellitesInUse.size()) {
            for (auto &s : updateInfo.m_satellitesInUse) {
                static_cast<QGeoSatelliteInfoPrivateNmea *>(QGeoSatelliteInfoPrivate::get(s))
                        ->nmeaSentences.append(updateInfo.gsa);
            }
            for (auto &s : updateInfo.m_satellitesInView) {
                static_cast<QGeoSatelliteInfoPrivateNmea *>(QGeoSatelliteInfoPrivate::get(s))
                        ->nmeaSentences.append(updateInfo.gsa);
            }
        }
#endif
    } else {
        const auto parserStatus = m_source->parseSatelliteInfoFromNmea(
                buf, size, updateInfo.m_satellitesInView);
        if (parserStatus == QNmeaSatelliteInfoSource::PartiallyParsed) {
            updateInfo.m_updatingGsv = true;
#if USE_NMEA_PIMPL
            updateInfo.gsv.append(QByteArray(buf, size));
#endif
        } else if (parserStatus == QNmeaSatelliteInfoSource::FullyParsed) {
#if USE_NMEA_PIMPL
            updateInfo.gsv.append(QByteArray(buf, size));
            for (int i = 0; i < updateInfo.m_satellitesInView.size(); i++) {
                const QGeoSatelliteInfo &s = updateInfo.m_satellitesInView.at(i);
                QGeoSatelliteInfoPrivateNmea *pimpl =
                        new QGeoSatelliteInfoPrivateNmea(*QGeoSatelliteInfoPrivate::get(s));
                pimpl->nmeaSentences.append(updateInfo.gsa);
                pimpl->nmeaSentences.append(updateInfo.gsv);
                updateInfo.m_satellitesInView.replace(i, QGeoSatelliteInfo(*pimpl));
            }
            updateInfo.gsv.clear();
#endif
            updateInfo.setSatellitesInView(updateInfo.m_satellitesInView);
        }
    }
}

QNmeaSatelliteInfoSourcePrivate::~QNmeaSatelliteInfoSourcePrivate()
{
    delete m_updateTimer;
}

void QNmeaSatelliteInfoSourcePrivate::startUpdates()
{
    if (m_invokedStart)
        return;

    m_satelliteError = QGeoSatelliteInfoSource::NoError;

    m_invokedStart = true;
    m_pendingUpdate.clear();
    m_noUpdateLastInterval = false;

    bool initialized = initialize();
    if (!initialized)
        return;

    if (m_updateMode == QNmeaSatelliteInfoSource::UpdateMode::RealTimeMode) {
        // skip over any buffered data - we only want the newest data.
        // Don't do this in requestUpdate. In that case bufferedData is good to have/use.
        if (m_device->bytesAvailable()) {
            if (m_device->isSequential())
                m_device->readAll();
            else
                m_device->seek(m_device->bytesAvailable());
        }
    }

    if (m_updateTimer)
        m_updateTimer->stop();

    if (m_source->updateInterval() > 0) {
        if (!m_updateTimer)
            m_updateTimer = new QBasicTimer;
        m_updateTimer->start(m_source->updateInterval(), this);
    }

    if (initialized)
        prepareSourceDevice();
}

void QNmeaSatelliteInfoSourcePrivate::stopUpdates()
{
    m_invokedStart = false;
    if (m_updateTimer)
        m_updateTimer->stop();
    m_pendingUpdate.clear();
    m_noUpdateLastInterval = false;
}

void QNmeaSatelliteInfoSourcePrivate::requestUpdate(int msec)
{
    if (m_requestTimer && m_requestTimer->isActive())
        return;

    m_satelliteError = QGeoSatelliteInfoSource::NoError;

    if (msec <= 0 || msec < m_source->minimumUpdateInterval()) {
        m_source->setError(QGeoSatelliteInfoSource::UpdateTimeoutError);
        return;
    }

    if (!m_requestTimer) {
        m_requestTimer = new QTimer(this);
        connect(m_requestTimer, SIGNAL(timeout()), SLOT(updateRequestTimeout()));
    }

    bool initialized = initialize();
    if (!initialized) {
        m_source->setError(QGeoSatelliteInfoSource::UpdateTimeoutError);
        return;
    }

    m_requestTimer->start(msec);
    prepareSourceDevice();
}

void QNmeaSatelliteInfoSourcePrivate::readyRead()
{
    if (m_nmeaReader)
        m_nmeaReader->readAvailableData();
}

void QNmeaSatelliteInfoSourcePrivate::emitPendingUpdate()
{
    if (m_pendingUpdate.isValid() && m_pendingUpdate.isFresh()) {
        m_updateTimeoutSent = false;
        m_noUpdateLastInterval = false;
        if (!emitUpdated(m_pendingUpdate, false))
            m_noUpdateLastInterval = true;
        // m_pendingUpdate.clear(); // Do not clear, it will be incrementally updated
    } else { // invalid or not fresh update
        if (m_noUpdateLastInterval && !m_updateTimeoutSent) {
            m_updateTimeoutSent = true;
            m_source->setError(QGeoSatelliteInfoSource::UpdateTimeoutError);
        }
        m_noUpdateLastInterval = true;
    }
}

void QNmeaSatelliteInfoSourcePrivate::sourceDataClosed()
{
    if (m_nmeaReader && m_device && m_device->bytesAvailable())
        m_nmeaReader->readAvailableData();
}

void QNmeaSatelliteInfoSourcePrivate::updateRequestTimeout()
{
    m_requestTimer->stop();
    m_source->setError(QGeoSatelliteInfoSource::UpdateTimeoutError);
}

bool QNmeaSatelliteInfoSourcePrivate::openSourceDevice()
{
    if (!m_device) {
        qWarning("QNmeaSatelliteInfoSource: no QIODevice data source, call setDevice() first");
        return false;
    }

    if (!m_device->isOpen() && !m_device->open(QIODevice::ReadOnly)) {
        qWarning("QNmeaSatelliteInfoSource: cannot open QIODevice data source");
        return false;
    }

    connect(m_device, SIGNAL(aboutToClose()), SLOT(sourceDataClosed()));
    connect(m_device, SIGNAL(readChannelFinished()), SLOT(sourceDataClosed()));
    connect(m_device, SIGNAL(destroyed()), SLOT(sourceDataClosed()));

    return true;
}

bool QNmeaSatelliteInfoSourcePrivate::initialize()
{
    if (m_nmeaReader)
        return true;

    if (!openSourceDevice())
        return false;

    if (m_updateMode == QNmeaSatelliteInfoSource::UpdateMode::RealTimeMode)
        m_nmeaReader.reset(new QNmeaSatelliteRealTimeReader(this));
    else
        m_nmeaReader.reset(new QNmeaSatelliteSimulationReader(this));

    return true;
}

void QNmeaSatelliteInfoSourcePrivate::prepareSourceDevice()
{
    // some data may already be available
    if (m_updateMode == QNmeaSatelliteInfoSource::UpdateMode::SimulationMode) {
        if (m_nmeaReader && m_device->bytesAvailable())
            m_nmeaReader->readAvailableData();
    }

    if (!m_connectedReadyRead) {
        connect(m_device, SIGNAL(readyRead()), SLOT(readyRead()));
        m_connectedReadyRead = true;
    }
}

bool QNmeaSatelliteInfoSourcePrivate::emitUpdated(QNmeaSatelliteInfoUpdate &update,
                                                  bool fromRequestUpdate)
{
    bool emitted = false;
    if (!update.isFresh())
        return emitted;

    update.consume();
    const bool inUseUpdated = update.m_satellitesInUse != m_lastUpdate.m_satellitesInUse;
    const bool inViewUpdated = update.m_satellitesInView != m_lastUpdate.m_satellitesInView;

    // if we come here from requestUpdate(), we need to emit, even if the data
    // didn't really change
    m_lastUpdate = update;
    if (update.m_validInUse && (inUseUpdated || fromRequestUpdate)) {
        emit m_source->satellitesInUseUpdated(update.m_satellitesInUse);
        emitted = true;
    }
    if (update.m_validInView && (inViewUpdated || fromRequestUpdate)) {
        emit m_source->satellitesInViewUpdated(update.m_satellitesInView);
        emitted = true;
    }
    return emitted;
}

void QNmeaSatelliteInfoSourcePrivate::timerEvent(QTimerEvent * /*event*/)
{
    emitPendingUpdate();
}

/*!
    \class QNmeaSatelliteInfoSource
    \inmodule QtPositioning
    \ingroup QtPositioning-positioning
    \since 6.2

    \brief The \l QNmeaSatelliteInfoSource class provides satellite information
    using an NMEA data source.

    NMEA is a commonly used protocol for the specification of one's global
    position at a certain point in time. The \l QNmeaSatelliteInfoSource class
    reads NMEA data and uses it to provide information about satellites in view
    and satellites in use in form of lists of \l QGeoSatelliteInfo objects.

    A \l QNmeaSatelliteInfoSource instance operates in either \l {RealTimeMode}
    or \l {SimulationMode}. These modes allow NMEA data to be read from either a
    live source of data, or replayed for simulation purposes from previously
    recorded NMEA data.

    The source of NMEA data is set via \l setDevice().

    Use \l startUpdates() to start receiving regular satellite information
    updates and  \l stopUpdates() to stop these updates. If you only require
    updates occasionally, you can call \l requestUpdate() to request a single
    update of both satellites in view and satellites in use.

    The information about satellites in view is received via the
    \l satellitesInViewUpdated() signal.

    The information about satellites in use is received via the
    \l satellitesInUseUpdated() signal.
*/

/*!
    \enum QNmeaSatelliteInfoSource::UpdateMode
    Defines the available update modes.

    \value RealTimeMode Satellite information is read and distributed from the
           data source as it becomes available. Use this mode if you are using
           a live source of NMEA data (for example a GPS hardware device).
    \value SimulationMode Satellite information is read and distributed from the
           data source at the given rate. The rate is determined by the
           \l {QNmeaSatelliteInfoSource::}{SimulationUpdateInterval} parameter.
           Use this mode if the data source contains previously recorded NMEA
           data and you want to replay the data for simulation purposes.
*/

/*!
    \variable QNmeaSatelliteInfoSource::SimulationUpdateInterval
    \brief The backend property name for data read rate in the
    \l SimulationMode. The value for this property is the integer number
    representing the amount of milliseconds between the subsequent reads.
    Use this parameter in the \l {QNmeaSatelliteInfoSource::}
    {setBackendProperty()} and \l {QNmeaSatelliteInfoSource::}{backendProperty()}
    methods.

    \note This property is different from the interval that can be set via
    \l {setUpdateInterval()}. The value set via \l {setUpdateInterval()}
    denotes an interval for the user notification, while this parameter
    specifies the internal frequency of reading the data from source file. It
    means that one can have multiple (or none) reads during the
    \l {updateInterval()} period.
*/
QString QNmeaSatelliteInfoSource::SimulationUpdateInterval =
        QStringLiteral("nmea.satellite_info_simulation_interval");

/*!
    Constructs a \l QNmeaSatelliteInfoSource instance with the given \a parent
    and \a mode.
*/
QNmeaSatelliteInfoSource::QNmeaSatelliteInfoSource(UpdateMode mode, QObject *parent)
    : QGeoSatelliteInfoSource(parent),
      d(new QNmeaSatelliteInfoSourcePrivate(this, mode))
{
}

/*!
    Destroys the satellite information source.
*/
QNmeaSatelliteInfoSource::~QNmeaSatelliteInfoSource()
{
    delete d;
}

/*!
    Sets the NMEA data source to \a device. If the device is not open, it
    will be opened in QIODevice::ReadOnly mode.

    The source device can only be set once and must be set before calling
    \l startUpdates() or \l requestUpdate().

    \note The \a device must emit \l {QIODevice::readyRead()} for the
    source to be notified when data is available for reading.
    \l QNmeaSatelliteInfoSource does not assume the ownership of the device,
    and hence does not deallocate it upon destruction.
*/
void QNmeaSatelliteInfoSource::setDevice(QIODevice *device)
{
    if (device != d->m_device) {
        if (!d->m_device)
            d->m_device = device;
        else
            qWarning("QNmeaSatelliteInfoSource: source device has already been set");
    }
}

/*!
    Returns the NMEA data source.
*/
QIODevice *QNmeaSatelliteInfoSource::device() const
{
    return d->m_device;
}

/*!
    \reimp
*/
void QNmeaSatelliteInfoSource::setUpdateInterval(int msec)
{
    int interval = msec;
    if (interval != 0)
        interval = qMax(msec, minimumUpdateInterval());
    QGeoSatelliteInfoSource::setUpdateInterval(interval);
    if (d->m_invokedStart) {
        d->stopUpdates();
        d->startUpdates();
    }
}

/*!
    \reimp
*/
int QNmeaSatelliteInfoSource::minimumUpdateInterval() const
{
    return 2; // Some chips are capable of over 100 updates per seconds.
}

/*!
    \reimp
*/
QGeoSatelliteInfoSource::Error QNmeaSatelliteInfoSource::error() const
{
    return d->m_satelliteError;
}

/*!
    \reimp
*/
bool QNmeaSatelliteInfoSource::setBackendProperty(const QString &name, const QVariant &value)
{
    if (name == SimulationUpdateInterval && d->m_updateMode == UpdateMode::SimulationMode) {
        bool ok = false;
        const int interval = value.toInt(&ok);
        if (ok) {
            auto *reader = dynamic_cast<QNmeaSatelliteSimulationReader *>(d->m_nmeaReader.get());
            if (reader) {
                reader->setUpdateInterval(interval);
            } else {
                // d->m_nmeaReader will use it in constructor
                d->m_simulationUpdateInterval = interval;
            }
            return true;
        }
    }
    return false;
}

/*!
    \reimp
*/
QVariant QNmeaSatelliteInfoSource::backendProperty(const QString &name) const
{
    if (name == SimulationUpdateInterval && d->m_updateMode == UpdateMode::SimulationMode) {
        auto *reader = dynamic_cast<QNmeaSatelliteSimulationReader *>(d->m_nmeaReader.get());
        if (reader)
            return reader->updateInterval();
        else
            return d->m_simulationUpdateInterval;
    }
    return QVariant();
}

/*!
    \reimp
*/
void QNmeaSatelliteInfoSource::startUpdates()
{
    d->startUpdates();
}

/*!
    \reimp
*/
void QNmeaSatelliteInfoSource::stopUpdates()
{
    d->stopUpdates();
}

/*!
    \reimp
*/
void QNmeaSatelliteInfoSource::requestUpdate(int msec)
{
    d->requestUpdate(msec == 0 ? 60000 * 5 : msec); // 5min default timeout
}

/*!
    Parses an NMEA sentence string to extract the IDs of satelites in use.

    The default implementation will parse standard NMEA $GPGSA sentences.
    This method should be reimplemented in a subclass whenever the need to deal
    with non-standard NMEA sentences arises.

    The parser reads \a size bytes from \a data and uses that information to
    fill \a pnrsInUse list.

    Returns \c true if the sentence was successfully parsed, otherwise returns
    \c false and should not modifiy \a pnrsInUse.
*/
bool QNmeaSatelliteInfoSource::parseSatellitesInUseFromNmea(const char *data, int size,
                                                            QList<int> &pnrsInUse)
{
    return QLocationUtils::getSatInUseFromNmea(data, size, pnrsInUse);
}

/*!
    \enum QNmeaSatelliteInfoSource::SatelliteInfoParseStatus
    Defines the parse status of satellite information. The satellite information
    can be split into multiple sentences, and we need to parse all of them.
    \value NotParsed The data does not contain information about satellites.
    \value PartiallyParsed A valid satellite information is received and parsed,
           but it's not complete, so we need to wait for another NMEA sentence.
    \value FullyParsed Satellite information was fully collected and parsed.
*/

/*!
    Parses an NMEA sentence string to extract the information about satellites
    in view.

    The default implementation will parse standard NMEA $GPGSV sentences.
    This method should be reimplemented in a subclass whenever the need to deal
    with non-standard NMEA sentences arises.

    The parser reads \a size bytes from \a data and uses that information to
    fill \a infos list.

    Returns \l SatelliteInfoParseStatus with parse result.
    Modifies \a infos list in case \l SatelliteInfoParseStatus::PartiallyParsed
    or \l SatelliteInfoParseStatus::FullyParsed is returned.
*/
QNmeaSatelliteInfoSource::SatelliteInfoParseStatus
QNmeaSatelliteInfoSource::parseSatelliteInfoFromNmea(const char *data, int size,
                                                     QList<QGeoSatelliteInfo> &infos)
{
    return static_cast<SatelliteInfoParseStatus>(
            QLocationUtils::getSatInfoFromNmea(data, size, infos));
}

void QNmeaSatelliteInfoSource::setError(QGeoSatelliteInfoSource::Error satelliteError)
{
    d->m_satelliteError = satelliteError;
    if (d->m_satelliteError != QGeoSatelliteInfoSource::NoError)
        emit QGeoSatelliteInfoSource::errorOccurred(satelliteError);
}

void QNmeaSatelliteInfoUpdate::setSatellitesInView(const QList<QGeoSatelliteInfo> &inView)
{
    m_updatingGsv = false;
    m_satellitesInView = inView;
    m_validInView = m_fresh = true;
    if (m_inUse.size()) {
        m_satellitesInUse.clear();
        m_validInUse = false;
        bool corrupt = false;
        for (const auto i : m_inUse) {
            bool found = false;
            for (const auto &s : m_satellitesInView) {
                if (s.satelliteIdentifier() == i) {
                    m_satellitesInUse.append(s);
                    found = true;
                }
            }
            if (!found) {
                // received a GSA before a GSV, but it was incorrect or something.
                // Unrelated to this GSV at least.
                m_satellitesInUse.clear();
                corrupt = true;
                break;
            }
        }
        m_validInUse = !corrupt;
        m_inUse.clear();
    }
}

bool QNmeaSatelliteInfoUpdate::setSatellitesInUse(const QList<int> &inUse)
{
    m_satellitesInUse.clear();
    m_validInUse = false;
    m_inUse = inUse;
    if (m_updatingGsv) {
        m_satellitesInUse.clear();
        m_validInView = false;
        return false;
    }
    for (const auto i : inUse) {
        bool found = false;
        for (const auto &s : m_satellitesInView) {
            if (s.satelliteIdentifier() == i) {
                m_satellitesInUse.append(s);
                found = true;
            }
        }
        if (!found) {
            // if satellites in use aren't in view, the related GSV is still to be received.
            m_inUse = inUse; // So clear outdated data, buffer the info, and set it later.
            m_satellitesInUse.clear();
            m_satellitesInView.clear();
            m_validInView = false;
            return false;
        }
    }
    m_validInUse = m_fresh = true;
    return true;
}

void QNmeaSatelliteInfoUpdate::consume()
{
    m_fresh = false;
}

bool QNmeaSatelliteInfoUpdate::isFresh() const
{
    return m_fresh;
}

QSet<int> QNmeaSatelliteInfoUpdate::inUse() const
{
    QSet<int> res;
    for (const auto &s : m_satellitesInUse)
        res.insert(s.satelliteIdentifier());
    return res;
}

void QNmeaSatelliteInfoUpdate::clear()
{
    m_satellitesInView.clear();
    m_satellitesInUse.clear();
    m_validInView = m_validInUse = false;
}

bool QNmeaSatelliteInfoUpdate::isValid() const
{
    // GSV without GSA is valid. GSA with outdated but still matching GSV also valid.
    return m_validInView || m_validInUse;
}

QNmeaSatelliteReader::QNmeaSatelliteReader(QNmeaSatelliteInfoSourcePrivate *sourcePrivate)
    : m_proxy(sourcePrivate)
{
}

QNmeaSatelliteReader::~QNmeaSatelliteReader()
{
}

QNmeaSatelliteRealTimeReader::QNmeaSatelliteRealTimeReader(QNmeaSatelliteInfoSourcePrivate *sourcePrivate)
    : QNmeaSatelliteReader(sourcePrivate)
{
}

void QNmeaSatelliteRealTimeReader::readAvailableData()
{
    while (m_proxy->m_device->canReadLine())
        m_proxy->processNmeaData(m_proxy->m_pendingUpdate);
    m_proxy->notifyNewUpdate();
}

QNmeaSatelliteSimulationReader::QNmeaSatelliteSimulationReader(QNmeaSatelliteInfoSourcePrivate *sourcePrivate)
    : QNmeaSatelliteReader(sourcePrivate)
{
    m_timer.reset(new QTimer);
    QObject::connect(m_timer.get(), &QTimer::timeout, [this]() {
        readAvailableData();
    });
    m_updateInterval =
            qMax(m_proxy->m_simulationUpdateInterval, m_proxy->m_source->minimumUpdateInterval());
}

void QNmeaSatelliteSimulationReader::readAvailableData()
{
    if (!m_timer->isActive()) {
        // At the very first start we just start a timer to simulate a short
        // delay for overlapping requestUpdate() calls.
        // See TestQGeoSatelliteInfoSource::requestUpdate_overlappingCalls and
        // TestQGeoSatelliteInfoSource::requestUpdate_overlappingCallsWithTimeout
        m_timer->start(m_updateInterval);
    } else {
        // Here we try to get both satellites in view and satellites in use.
        // We behave like that because according to the QGeoSatelliteInfoSource
        // tests each call to requestUpdate() should return both satellites in
        // view and satellites in use. Same is expected on each interval for
        // startUpdates().
        // However user-provided NMEA logs might not contain some of the
        // messages, so we will try not to get stuck here infinitely.
        int numSatInUseMsgs = 0;
        int numSatInViewMsgs = 0;
        while (!numSatInUseMsgs || !numSatInViewMsgs) {
            m_proxy->processNmeaData(m_proxy->m_pendingUpdate);
            if (m_proxy->m_pendingUpdate.m_validInUse)
                numSatInUseMsgs++;
            if (m_proxy->m_pendingUpdate.m_validInView)
                numSatInViewMsgs++;
            // if we got the second message for one of them, but still didn't
            // receive any for the other - break.
            // We use 2 in the comparison, because, as soon as the m_validIn*
            // flag is set, it will stay true until we receive invalid message.
            if (numSatInUseMsgs > 2 || numSatInViewMsgs > 2) {
                const QString msgType = (numSatInUseMsgs > numSatInViewMsgs)
                        ? QStringLiteral("GSA")
                        : QStringLiteral("GSV");
                qWarning() << "nmea simulation reader: possibly incorrect message order. Got too "
                              "many consecutive"
                           << msgType << "messages";
                break;
            }
        }
        m_proxy->notifyNewUpdate();
    }
}

void QNmeaSatelliteSimulationReader::setUpdateInterval(int msec)
{
    // restart the timer with new interval
    m_updateInterval = qMax(msec, m_proxy->m_source->minimumUpdateInterval());
    if (m_timer->isActive())
        m_timer->start(m_updateInterval);
}

int QNmeaSatelliteSimulationReader::updateInterval() const
{
    return m_updateInterval;
}

QT_END_NAMESPACE
