/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtPositioning/qgeopositioninfosource.h>
#include <QtPositioning/qgeopositioninfosourcefactory.h>
#include <QObject>
#include <QtPlugin>
#include <QTimer>

QT_USE_NAMESPACE

// This is a dummy plugin that is created mostly to test some features of
// QDeclarativePositionSource. It does not provide any position updates.
// Use plugin from positionplugin subdirectory, if you need to simulate some
// positioning updates.

class DummyPluginSource : public QGeoPositionInfoSource
{
    Q_OBJECT

public:
    DummyPluginSource(const QVariantMap &parameters, QObject *parent = 0);
    ~DummyPluginSource();

    void startUpdates() override;
    void stopUpdates() override;
    void requestUpdate(int timeout = 5000) override;

    QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly) const override;
    PositioningMethods supportedPositioningMethods() const override;

    void setUpdateInterval(int msec) override;
    int minimumUpdateInterval() const override;
    Error error() const override;

private:
    Error lastError = QGeoPositionInfoSource::NoError;
};

DummyPluginSource::DummyPluginSource(const QVariantMap &parameters, QObject *parent)
    : QGeoPositionInfoSource(parent)
{
    Q_UNUSED(parameters)
}

QGeoPositionInfoSource::Error DummyPluginSource::error() const
{
    return lastError;
}

void DummyPluginSource::setUpdateInterval(int msec)
{
    if (msec < minimumUpdateInterval())
        msec = minimumUpdateInterval();

    QGeoPositionInfoSource::setUpdateInterval(msec);
}

int DummyPluginSource::minimumUpdateInterval() const
{
    return 100;
}

QGeoPositionInfo
DummyPluginSource::lastKnownPosition(bool fromSatellitePositioningMethodsOnly) const
{
    Q_UNUSED(fromSatellitePositioningMethodsOnly);
    return QGeoPositionInfo();
}

QGeoPositionInfoSource::PositioningMethods DummyPluginSource::supportedPositioningMethods() const
{
    return QGeoPositionInfoSource::NonSatellitePositioningMethods;
}

void DummyPluginSource::startUpdates()
{
    lastError = QGeoPositionInfoSource::NoError;
}

void DummyPluginSource::stopUpdates() { }

void DummyPluginSource::requestUpdate(int timeout)
{
    lastError = QGeoPositionInfoSource::NoError;
    if (timeout < minimumUpdateInterval()) {
        lastError = QGeoPositionInfoSource::UpdateTimeoutError;
        emit QGeoPositionInfoSource::errorOccurred(lastError);
    }
}

DummyPluginSource::~DummyPluginSource() { }

class DummyPluginForTestsFactory : public QObject, public QGeoPositionInfoSourceFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.qt.position.sourcefactory/6.0" FILE "plugin.json")
    Q_INTERFACES(QGeoPositionInfoSourceFactory)

public:
    QGeoPositionInfoSource *positionInfoSource(QObject *parent,
                                               const QVariantMap &parameters) override;
    QGeoSatelliteInfoSource *satelliteInfoSource(QObject *parent,
                                                 const QVariantMap &parameters) override;
    QGeoAreaMonitorSource *areaMonitor(QObject *parent, const QVariantMap &parameters) override;
};

QGeoPositionInfoSource *
DummyPluginForTestsFactory::positionInfoSource(QObject *parent, const QVariantMap &parameters)
{
    return new DummyPluginSource(parameters, parent);
}

QGeoSatelliteInfoSource *
DummyPluginForTestsFactory::satelliteInfoSource(QObject *parent, const QVariantMap &parameters)
{
    Q_UNUSED(parent);
    Q_UNUSED(parameters);
    return nullptr;
}

QGeoAreaMonitorSource *DummyPluginForTestsFactory::areaMonitor(QObject *parent,
                                                               const QVariantMap &parameters)
{
    Q_UNUSED(parent);
    Q_UNUSED(parameters);
    return nullptr;
}

#include "plugin.moc"
