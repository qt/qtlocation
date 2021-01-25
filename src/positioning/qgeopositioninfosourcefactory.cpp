/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtPositioning module of the Qt Toolkit.
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
**
**
**
****************************************************************************/

#include "qgeopositioninfosourcefactory.h"

QT_BEGIN_NAMESPACE

/*!
  \class QGeoPositionInfoSourceFactory
  \inmodule QtPositioning
  \since 5.2

  \brief The QGeoPositionInfoSourceFactory class is a factory class used
  as the plugin interface for external providers of positioning data.

  The other functions must be overridden by all plugins, other than
  sourcePriority() which defaults to returning 0. Higher values of
  priority will be preferred to lower ones.
*/

/*!
  \fn QGeoPositionInfoSource *QGeoPositionInfoSourceFactory::positionInfoSource(QObject *parent)

  Returns a new QGeoPositionInfoSource associated with this plugin
  with parent \a parent. Can also return 0, in which case the plugin
  loader will use the factory with the next highest priority.
  */

/*!
  \fn QGeoSatelliteInfoSource *QGeoPositionInfoSourceFactory::satelliteInfoSource(QObject *parent)

  Returns a new QGeoSatelliteInfoSource associated with this plugin
  with parent \a parent. Can also return 0, in which case the plugin
  loader will use the factory with the next highest priority.
  */

/*!
  \fn QGeoAreaMonitorSource *QGeoPositionInfoSourceFactory::areaMonitor(QObject *parent);

  Returns a new QGeoAreaMonitorSource associated with this plugin with parent \a parent.
  Can also return 0, in which case the plugin loader will use the factory with the
  next highest priority.
  */

/*!
    Destroys the position info source factory.
*/
QGeoPositionInfoSourceFactory::~QGeoPositionInfoSourceFactory()
{}

/*!
  \class QGeoPositionInfoSourceFactoryV2
  \inmodule QtPositioning
  \since 5.14

  \brief The QGeoPositionInfoSourceFactoryV2 class extends
  QGeoPositionInfoSourceFactory, adding factory methods taking a parameters argument.

  This interface has the priority over the older one with plugins implementing it,
  when creating a positioning source (position, satellite or area monitor)
*/

/*!
  \fn QGeoPositionInfoSource *QGeoPositionInfoSourceFactoryV2::positionInfoSourceWithParameters(QObject *parent, const QVariantMap &parameters)

  Returns a new QGeoPositionInfoSource associated with this plugin
  with parent \a parent, and using \a parameters as configuration parameters.
  Can also return 0, in which case the plugin loader will use the factory with
  the next highest priority.
  */

/*!
  \fn QGeoSatelliteInfoSource *QGeoPositionInfoSourceFactoryV2::satelliteInfoSourceWithParameters(QObject *parent, const QVariantMap &parameters)

  Returns a new QGeoSatelliteInfoSource associated with this plugin
  with parent \a parent, and using \a parameters as configuration parameters.
  Can also return 0, in which case the plugin loader will use the factory with
  the next highest priority.
  */

/*!
  \fn QGeoAreaMonitorSource *QGeoPositionInfoSourceFactoryV2::areaMonitorWithParameters(QObject *parent, const QVariantMap &parameters);

  Returns a new QGeoAreaMonitorSource associated with this plugin with parent
  \a parent, and using \a parameters as configuration parameters.
  Can also return 0, in which case the plugin loader will use the factory with
  the next highest priority.
  */

QGeoPositionInfoSourceFactoryV2::~QGeoPositionInfoSourceFactoryV2()
{}

QT_END_NAMESPACE

