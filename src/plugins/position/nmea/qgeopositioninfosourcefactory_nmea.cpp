/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
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

#include "qgeopositioninfosourcefactory_nmea.h"
#include <QtPositioning/QNmeaPositionInfoSource>
#include <QtPositioning/QNmeaSatelliteInfoSource>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QtNetwork/QTcpSocket>
#include <QLoggingCategory>
#include <QSet>
#include <QUrl>
#include <QFile>
#include <QSharedPointer>
#include "qiopipe_p.h"

Q_LOGGING_CATEGORY(lcNmea, "qt.positioning.nmea")

QT_BEGIN_NAMESPACE

static const auto sourceParameterName = QStringLiteral("nmea.source");
static const auto socketScheme = QStringLiteral("socket:");
static const auto serialScheme = QStringLiteral("serial:");

// This class is used only for SerialPort devices, because we can't open the
// same serial port twice.
// In case of files and sockets it's easier to explicitly create a QIODevice for
// each new instance of Nmea*InfoSource.
// Also QFile can't be directly used with QIOPipe, because QFile is not a
// sequential device.
// TcpSocket could be used with QIOPipe, but it complicates error handling
// dramatically, as we would need to somehow forward socket errors through
// QIOPipes to the clients.
class IODeviceContainer
{
public:
    IODeviceContainer() {}
    IODeviceContainer(IODeviceContainer const&) = delete;
    void operator=(IODeviceContainer const&)  = delete;

    QSharedPointer<QIOPipe> serial(const QString &portName)
    {
        if (m_serialPorts.contains(portName)) {
            m_serialPorts[portName].refs++;
            QIOPipe *endPipe = new QIOPipe(m_serialPorts[portName].proxy);
            m_serialPorts[portName].proxy->addChildPipe(endPipe);
            return QSharedPointer<QIOPipe>(endPipe);
        }
        IODevice device;
        QSerialPort *port = new QSerialPort(portName);
        port->setBaudRate(4800);
        qCDebug(lcNmea) << "Opening serial port" << portName;
        if (!port->open(QIODevice::ReadOnly)) {
            qWarning("nmea: Failed to open %s", qPrintable(portName));
            delete port;
            return {};
        }
        qCDebug(lcNmea) << "Opened successfully";
        device.device = port;
        device.refs = 1;
        device.proxy = new QIOPipe(port, QIOPipe::ProxyPipe);
        m_serialPorts[portName] = device;
        QIOPipe *endPipe = new QIOPipe(device.proxy);
        device.proxy->addChildPipe(endPipe);
        return QSharedPointer<QIOPipe>(endPipe);
    }

    void releaseSerial(const QString &portName, QSharedPointer<QIOPipe> &pipe)
    {
        if (!m_serialPorts.contains(portName))
            return;

        pipe.clear(); // make sure to release the pipe returned by getSerial, or else, if there are still refs, data will be leaked through it
        IODevice &device = m_serialPorts[portName];
        if (device.refs > 1) {
            device.refs--;
            return;
        }

        IODevice taken = m_serialPorts.take(portName);
        taken.device->deleteLater();
    }

private:

    struct IODevice {
        QIODevice *device = nullptr;
        QIOPipe *proxy = nullptr; // adding client pipes as children of proxy
                                  // allows to dynamically add clients to one device.
        unsigned int refs = 1;
    };

    QMap<QString, IODevice> m_serialPorts;
};

Q_GLOBAL_STATIC(IODeviceContainer, deviceContainer)

// We use a string prefix to distinguish between the different data sources.
// "socket:" means that we use a socket connection
// "serial:" means that we use a serial port connection
// "file:///", "qrc:///" and just plain strings mean that we try to use local
// file.
// Note: if we do not specify anything, or specify "serial:" without specifying
// the port name, then we will try to search for a well-known serial port
// device.
class NmeaSource : public QNmeaPositionInfoSource
{
    Q_OBJECT
public:
    NmeaSource(QObject *parent, const QVariantMap &parameters);
    NmeaSource(QObject *parent, const QString &fileName);
    ~NmeaSource() override;
    bool isValid() const
    {
        return !m_dataSource.isNull() || !m_fileSource.isNull() || !m_socket.isNull();
    }

private slots:
    void onSocketError(QAbstractSocket::SocketError error);

private:
    void parseSourceParameter(const QString &source);
    void addSerialDevice(const QString &requestedPort);
    void setFileName(const QString &fileName);
    void connectSocket(const QString &source);

    QSharedPointer<QIOPipe> m_dataSource;
    QScopedPointer<QFile> m_fileSource;
    QScopedPointer<QTcpSocket> m_socket;
    QString m_sourceName;
};

NmeaSource::NmeaSource(QObject *parent, const QVariantMap &parameters)
    : QNmeaPositionInfoSource(RealTimeMode, parent)
{
    const QString source = parameters.value(sourceParameterName).toString();
    parseSourceParameter(source);
}

NmeaSource::NmeaSource(QObject *parent, const QString &fileName)
    : QNmeaPositionInfoSource(SimulationMode, parent)
{
    setFileName(fileName);
}

NmeaSource::~NmeaSource()
{
    deviceContainer->releaseSerial(m_sourceName, m_dataSource);
}

void NmeaSource::onSocketError(QAbstractSocket::SocketError error)
{
    m_socket->close();

    switch (error) {
    case QAbstractSocket::UnknownSocketError:
        setError(QGeoPositionInfoSource::UnknownSourceError);
        break;
    case QAbstractSocket::SocketAccessError:
        setError(QGeoPositionInfoSource::AccessError);
        break;
    case QAbstractSocket::RemoteHostClosedError:
        setError(QGeoPositionInfoSource::ClosedError);
        break;
    default:
        qWarning() << "Connection failed! QAbstractSocket::SocketError" << error;
        // TODO - introduce new type of error. TransportError?
        setError(QGeoPositionInfoSource::UnknownSourceError);
        break;
    }
}

void NmeaSource::parseSourceParameter(const QString &source)
{
    if (source.startsWith(socketScheme)) {
        // This is a socket
        connectSocket(source);
    } else {
        // Last chance - this can be serial device.
        // Note: File is handled in a separate case.
        addSerialDevice(source);
    }
}

static QString tryFindSerialDevice(const QString &requestedPort)
{
    QString portName;
    if (requestedPort.isEmpty()) {
        const QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
        qCDebug(lcNmea) << "Found" << ports.count() << "serial ports";
        if (ports.isEmpty()) {
            qWarning("nmea: No serial ports found");
            return portName;
        }

        // Try to find a well-known device.
        QSet<int> supportedDevices;
        supportedDevices << 0x67b; // GlobalSat (BU-353S4 and probably others)
        supportedDevices << 0xe8d; // Qstarz MTK II
        for (const QSerialPortInfo& port : ports) {
            if (port.hasVendorIdentifier() && supportedDevices.contains(port.vendorIdentifier())) {
                portName = port.portName();
                break;
            }
        }

        if (portName.isEmpty()) {
            qWarning("nmea: No known GPS device found.");
        }
    } else {
        portName = requestedPort;
        if (portName.startsWith(serialScheme))
            portName.remove(0, 7);
    }
    return portName;
}

void NmeaSource::addSerialDevice(const QString &requestedPort)
{
    m_sourceName = tryFindSerialDevice(requestedPort);
    if (m_sourceName.isEmpty())
        return;

    m_dataSource = deviceContainer->serial(m_sourceName);
    if (!m_dataSource)
        return;

    setDevice(m_dataSource.data());
}

void NmeaSource::setFileName(const QString &fileName)
{
    m_sourceName = fileName;

    m_fileSource.reset(new QFile(fileName));
    qCDebug(lcNmea) << "Opening file" << fileName;
    if (!m_fileSource->open(QIODevice::ReadOnly)) {
        qWarning("nmea: failed to open file %s", qPrintable(fileName));
        m_fileSource.reset();
    }

    if (!m_fileSource)
        return;

    qCDebug(lcNmea) << "Opened successfully";

    setDevice(m_fileSource.data());
}

void NmeaSource::connectSocket(const QString &source)
{
    const QUrl url(source);
    const QString host = url.host();
    const int port = url.port();
    if (!host.isEmpty() && (port > 0)) {
        m_socket.reset(new QTcpSocket);
        // no need to explicitly connect to connected() signal
        connect(m_socket.get(), &QTcpSocket::errorOccurred, this, &NmeaSource::onSocketError);
        m_socket->connectToHost(host, port, QTcpSocket::ReadOnly);
        m_sourceName = source;

        setDevice(m_socket.data());
    } else {
        qWarning("nmea: incorrect socket parameters %s:%d", qPrintable(host), port);
    }
}

class NmeaSatelliteSource : public QNmeaSatelliteInfoSource
{
    Q_OBJECT
public:
    NmeaSatelliteSource(QObject *parent, const QVariantMap &parameters);
    NmeaSatelliteSource(QObject *parent, const QString &fileName, const QVariantMap &parameters);
    ~NmeaSatelliteSource();

    bool isValid() const { return !m_port.isNull() || !m_file.isNull() || !m_socket.isNull(); }

private slots:
    void onSocketError(QAbstractSocket::SocketError error);

private:
    void parseRealtimeSource(const QString &source);
    void parseSimulationSource(const QString &localFileName);

    QSharedPointer<QIOPipe> m_port;
    QScopedPointer<QFile> m_file;
    QScopedPointer<QTcpSocket> m_socket;
    QString m_sourceName;
};

NmeaSatelliteSource::NmeaSatelliteSource(QObject *parent, const QVariantMap &parameters)
    : QNmeaSatelliteInfoSource(QNmeaSatelliteInfoSource::UpdateMode::RealTimeMode, parent)
{
    const QString source = parameters.value(sourceParameterName).toString();
    parseRealtimeSource(source);
}

// We can use a QNmeaSatelliteInfoSource::SimulationUpdateInterval parameter to
// set the file read frequency in simulation mode. We use setBackendProperty()
// for it. The value can't be smaller than minimumUpdateInterval().
// This check is done on the QNmeaSatelliteInfoSource level
NmeaSatelliteSource::NmeaSatelliteSource(QObject *parent, const QString &fileName,
                                         const QVariantMap &parameters)
    : QNmeaSatelliteInfoSource(QNmeaSatelliteInfoSource::UpdateMode::SimulationMode, parent)
{
    bool ok = false;
    const int interval =
            parameters.value(QNmeaSatelliteInfoSource::SimulationUpdateInterval).toInt(&ok);
    if (ok)
        setBackendProperty(QNmeaSatelliteInfoSource::SimulationUpdateInterval, interval);
    parseSimulationSource(fileName);
}

NmeaSatelliteSource::~NmeaSatelliteSource()
{
    deviceContainer->releaseSerial(m_sourceName, m_port);
}

void NmeaSatelliteSource::onSocketError(QAbstractSocket::SocketError error)
{
    m_socket->close();

    switch (error) {
    case QAbstractSocket::UnknownSocketError:
        setError(QGeoSatelliteInfoSource::UnknownSourceError);
        break;
    case QAbstractSocket::SocketAccessError:
        setError(QGeoSatelliteInfoSource::AccessError);
        break;
    case QAbstractSocket::RemoteHostClosedError:
        setError(QGeoSatelliteInfoSource::ClosedError);
        break;
    default:
        qWarning() << "Connection failed! QAbstractSocket::SocketError" << error;
        // TODO - introduce new type of error. TransportError?
        setError(QGeoSatelliteInfoSource::UnknownSourceError);
        break;
    }
}

void NmeaSatelliteSource::parseRealtimeSource(const QString &source)
{
    if (source.startsWith(socketScheme)) {
        // This is a socket.
        const QUrl url(source);
        const QString host = url.host();
        const int port = url.port();
        if (!host.isEmpty() && (port > 0)) {
            m_socket.reset(new QTcpSocket);
            // no need to explicitly connect to connected() signal
            connect(m_socket.get(), &QTcpSocket::errorOccurred,
                    this, &NmeaSatelliteSource::onSocketError);
            m_socket->connectToHost(host, port, QTcpSocket::ReadOnly);
            m_sourceName = source;

            setDevice(m_socket.data());
        } else {
            qWarning("nmea: incorrect socket parameters %s:%d", qPrintable(host), port);
        }
    } else {
        // Last chance - this can be serial device.
        m_sourceName = tryFindSerialDevice(source);
        if (m_sourceName.isEmpty())
            return;

        m_port = deviceContainer->serial(m_sourceName);
        if (!m_port)
            return;

        setDevice(m_port.data());
    }
}

void NmeaSatelliteSource::parseSimulationSource(const QString &localFileName)
{
    // This is a text file.
    m_sourceName = localFileName;

    qCDebug(lcNmea) << "Opening file" << localFileName;
    m_file.reset(new QFile(localFileName));
    if (!m_file->open(QIODevice::ReadOnly)) {
        qWarning("nmea: failed to open file %s", qPrintable(localFileName));
        m_file.reset();
        return;
    }
    qCDebug(lcNmea) << "Opened successfully";

    setDevice(m_file.data());
}

/*!
    \internal
    Returns a local file name if \a source represents it.
    The returned value can be different from \a source, as the method tries to
    modify the path
*/
static QString checkSourceIsFile(const QString &source)
{
    if (source.isEmpty())
        return QString();

    QString localFileName = source;

    if (!QFile::exists(localFileName)) {
        if (localFileName.startsWith(QStringLiteral("qrc:///")))
            localFileName.remove(0, 7);
        else if (localFileName.startsWith(QStringLiteral("file:///")))
            localFileName.remove(0, 7);
        else if (localFileName.startsWith(QStringLiteral("qrc:/")))
            localFileName.remove(0, 5);

        if (!QFile::exists(localFileName) && localFileName.startsWith(QLatin1Char('/')))
            localFileName.remove(0, 1);
    }
    if (!QFile::exists(localFileName))
        localFileName.prepend(QLatin1Char(':'));

    const bool isLocalFile = QFile::exists(localFileName);
    return isLocalFile ? localFileName : QString();
}

/*!
    \internal
    Returns a local file name if file exists, or an empty string otherwise
*/
static QString extractLocalFileName(const QVariantMap &parameters)
{
    QString localFileName = parameters.value(sourceParameterName).toString();
    return checkSourceIsFile(localFileName);
}

QGeoPositionInfoSource *QGeoPositionInfoSourceFactoryNmea::positionInfoSource(QObject *parent, const QVariantMap &parameters)
{
    std::unique_ptr<NmeaSource> src = nullptr;

    const QString localFileName = extractLocalFileName(parameters);
    if (localFileName.isEmpty())
        src = std::make_unique<NmeaSource>(parent, parameters); // use RealTimeMode
    else
        src = std::make_unique<NmeaSource>(parent, localFileName); // use SimulationMode

    return (src && src->isValid()) ? src.release() : nullptr;
}

QGeoSatelliteInfoSource *QGeoPositionInfoSourceFactoryNmea::satelliteInfoSource(QObject *parent, const QVariantMap &parameters)
{
    std::unique_ptr<NmeaSatelliteSource> src = nullptr;

    const QString localFileName = extractLocalFileName(parameters);
    if (localFileName.isEmpty()) {
        // use RealTimeMode
        src = std::make_unique<NmeaSatelliteSource>(parent, parameters);
    } else {
        // use SimulationMode
        src = std::make_unique<NmeaSatelliteSource>(parent, localFileName, parameters);
    }
    return (src && src->isValid()) ? src.release() : nullptr;
}

QGeoAreaMonitorSource *QGeoPositionInfoSourceFactoryNmea::areaMonitor(QObject *parent, const QVariantMap &parameters)
{
    Q_UNUSED(parent);
    Q_UNUSED(parameters);
    return nullptr;
}

QT_END_NAMESPACE

#include "qgeopositioninfosourcefactory_nmea.moc"
