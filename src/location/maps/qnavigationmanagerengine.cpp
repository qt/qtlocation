/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include "qgeoroute.h"
#include "qnavigationmanagerengine_p.h"

QT_BEGIN_NAMESPACE

class QNavigationManagerEnginePrivate
{
public:
    QString managerName;
    int managerVersion;
    QLocale locale;
    QLocale::MeasurementSystem measurementSystem;
    bool initialized = false;
};

class QAbstractNavigatorPrivate
{
public:
    QLocale locale;
    QLocale::MeasurementSystem measurementSystem;
};

QAbstractNavigator::QAbstractNavigator(QObject *parent)
    : QObject(parent)
    , d(new QAbstractNavigatorPrivate)
{
}

QAbstractNavigator::~QAbstractNavigator()
{
}

void QAbstractNavigator::setLocale(const QLocale &locale)
{
    d->locale = locale;
}

QLocale QAbstractNavigator::locale() const
{
    return  d->locale;
}

void QAbstractNavigator::setMeasurementSystem(QLocale::MeasurementSystem system)
{
    d->measurementSystem = system;
}

QLocale::MeasurementSystem QAbstractNavigator::measurementSystem() const
{
    return d->measurementSystem;
}

QVariant QAbstractNavigator::nextManeuverIcon() const
{
    return QVariant();
}

double QAbstractNavigator::distanceToNextManeuver() const
{
    return qQNaN();
}

int QAbstractNavigator::timeToNextManeuver() const
{
    return -1;
}

int QAbstractNavigator::remainingTravelTime() const
{
    return -1;
}

double QAbstractNavigator::remainingTravelDistance() const
{
    return qQNaN();
}

int QAbstractNavigator::remainingTravelTimeToNextWaypoint() const
{
    return -1;
}

double QAbstractNavigator::remainingTravelDistanceToNextWaypoint() const
{
    return qQNaN();
}

double QAbstractNavigator::traveledDistance() const
{
    return 0;
}

int QAbstractNavigator::traveledTime() const
{
    return 0;
}

QGeoRoute QAbstractNavigator::currentRoute() const
{
    return QGeoRoute();
}

QGeoRouteLeg QAbstractNavigator::currentRouteLeg() const
{
    return QGeoRouteLeg();
}

QList<QGeoRoute> QAbstractNavigator::alternativeRoutes() const
{
    return QList<QGeoRoute>();
}

int QAbstractNavigator::currentSegment() const
{
    return 0;
}

QNavigationManagerEngine::QNavigationManagerEngine(const QVariantMap &parameters, QObject *parent)
    : QObject(parent)
    , d(new QNavigationManagerEnginePrivate)
{
    Q_UNUSED(parameters);
}

QNavigationManagerEngine::~QNavigationManagerEngine()
{
}

void QNavigationManagerEngine::setManagerName(const QString &name)
{
    d->managerName = name;
}

QString QNavigationManagerEngine::managerName() const
{
    return d->managerName;
}

void QNavigationManagerEngine::setManagerVersion(int version)
{
    d->managerVersion = version;
}

int QNavigationManagerEngine::managerVersion() const
{
    return d->managerVersion;
}

void QNavigationManagerEngine::setLocale(const QLocale &locale)
{
    d->locale = locale;
}

QLocale QNavigationManagerEngine::locale() const
{
    return d->locale;
}

void QNavigationManagerEngine::setMeasurementSystem(QLocale::MeasurementSystem system)
{
    d->measurementSystem = system;
}

QLocale::MeasurementSystem QNavigationManagerEngine::measurementSystem() const
{
    return d->measurementSystem;
}

bool QNavigationManagerEngine::isInitialized() const
{
    return d->initialized;
}

void QNavigationManagerEngine::engineInitialized()
{
    d->initialized = true;
    emit initialized();
}

QT_END_NAMESPACE
