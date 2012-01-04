/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qgeopositioninfosource.h>
#include <qgeopositioninfosourcefactory.h>
#include <QObject>
#include <QtPlugin>
#include <QTimer>

QT_USE_NAMESPACE

class DummySource : public QGeoPositionInfoSource
{
    Q_OBJECT

public:
    DummySource(QObject *parent=0);
    ~DummySource();

    void startUpdates();
    void stopUpdates();
    void requestUpdate(int timeout=5000);

    QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly) const;
    PositioningMethods supportedPositioningMethods() const;

    void setUpdateInterval(int msec);
    int minimumUpdateInterval() const;
    Error error() const;

private:
    QTimer *timer;
    QTimer *timeoutTimer;
    QTimer *singleTimer;
    QGeoPositionInfo lastPosition;

private slots:
    void updatePosition();
    void doTimeout();
};

DummySource::DummySource(QObject *parent) :
    QGeoPositionInfoSource(parent),
    timer(new QTimer(this)),
    timeoutTimer(new QTimer(this)),
    singleTimer(new QTimer(this)),
    lastPosition(QGeoCoordinate(0,0), QDateTime::currentDateTime())
{
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()),
            this, SLOT(updatePosition()));
    connect(singleTimer, SIGNAL(timeout()),
            this, SLOT(updatePosition()));
    connect(timeoutTimer, SIGNAL(timeout()),
            this, SLOT(doTimeout()));
}

QGeoPositionInfoSource::Error DummySource::error() const
{
    return QGeoPositionInfoSource::UnknownSourceError;
}


void DummySource::setUpdateInterval(int msec)
{
    if (msec == 0) {
        timer->setInterval(1000);
    } else if (msec < 1000) {
        msec = 1000;
        timer->setInterval(msec);
    } else {
        timer->setInterval(msec);
    }

    QGeoPositionInfoSource::setUpdateInterval(msec);
}

int DummySource::minimumUpdateInterval() const
{
    return 1000;
}

QGeoPositionInfo DummySource::lastKnownPosition(bool fromSatellitePositioningMethodsOnly) const
{
    Q_UNUSED(fromSatellitePositioningMethodsOnly);
    return lastPosition;
}

QGeoPositionInfoSource::PositioningMethods DummySource::supportedPositioningMethods() const
{
    return QGeoPositionInfoSource::AllPositioningMethods;
}

void DummySource::startUpdates()
{
    timer->start();
}

void DummySource::stopUpdates()
{
    timer->stop();
}

void DummySource::requestUpdate(int timeout)
{
    if (timeout == 0)
        timeout = 5000;
    if (timeout < 0)
        timeout = 0;

    timeoutTimer->setInterval(timeout);
    timeoutTimer->start();

    if (timer->isActive()) {
        timer->stop();
        timer->start();
    }

    singleTimer->setInterval(1000);
    singleTimer->start();
}

DummySource::~DummySource()
{}

void DummySource::updatePosition()
{
    timeoutTimer->stop();
    singleTimer->stop();
    QGeoCoordinate coord(lastPosition.coordinate().latitude() + 0.1,
                         lastPosition.coordinate().longitude() + 0.1);
    QGeoPositionInfo info(coord, QDateTime::currentDateTime());
    lastPosition = info;
    emit positionUpdated(info);
}

void DummySource::doTimeout()
{
    timeoutTimer->stop();
    singleTimer->stop();
    emit updateTimeout();
}


class QGeoPositionInfoSourceFactoryTest : public QObject, public QGeoPositionInfoSourceFactory
{
    Q_OBJECT
    Q_INTERFACES(QGeoPositionInfoSourceFactory:QFactoryInterface)

public:
    QString sourceName() const;
    int sourceVersion() const;

    QStringList keys() const { return QStringList() << QLatin1String("testposition.source"); }

    QGeoPositionInfoSource *positionInfoSource(QObject *parent);
    QGeoSatelliteInfoSource *satelliteInfoSource(QObject *parent);
};

QString QGeoPositionInfoSourceFactoryTest::sourceName() const
{
    return QLatin1String("test.source");
}

int QGeoPositionInfoSourceFactoryTest::sourceVersion() const
{
    return 1;
}

QGeoPositionInfoSource *QGeoPositionInfoSourceFactoryTest::positionInfoSource(QObject *parent)
{
    return new DummySource(parent);
}

QGeoSatelliteInfoSource *QGeoPositionInfoSourceFactoryTest::satelliteInfoSource(QObject *parent)
{
    Q_UNUSED(parent)
    // not implemented
    return 0;
}

Q_EXPORT_PLUGIN2(qtposition_testplugin, QGeoPositionInfoSourceFactoryTest)

#include "plugin.moc"
