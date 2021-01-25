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
#ifndef QGEOAREAMONITORSOURCE_H
#define QGEOAREAMONITORSOURCE_H

#include <QtPositioning/QGeoCoordinate>
#include <QtPositioning/QGeoAreaMonitorInfo>
#include <QtPositioning/QGeoPositionInfoSource>

#include <QtCore/QObject>
#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE

class QGeoPositionInfo;
class QGeoAreaMonitorSourcePrivate;
class Q_POSITIONING_EXPORT QGeoAreaMonitorSource : public QObject
{
    Q_OBJECT

public:
    enum Error {
        AccessError = 0,
        InsufficientPositionInfo = 1,
        UnknownSourceError = 2,
        NoError = 3
    };
    Q_ENUMS(Error)

    enum AreaMonitorFeature {
        PersistentAreaMonitorFeature = 0x00000001,
        AnyAreaMonitorFeature = 0xffffffff
    };
    Q_DECLARE_FLAGS(AreaMonitorFeatures, AreaMonitorFeature)

    explicit QGeoAreaMonitorSource(QObject *parent);
    virtual ~QGeoAreaMonitorSource();

    static QGeoAreaMonitorSource *createDefaultSource(QObject *parent);
    static QGeoAreaMonitorSource *createSource(const QString& sourceName, QObject *parent);
    static QStringList availableSources();

    virtual void setPositionInfoSource(QGeoPositionInfoSource *source);
    virtual QGeoPositionInfoSource* positionInfoSource() const;

    QString sourceName() const;

    virtual Error error() const = 0;
    virtual AreaMonitorFeatures supportedAreaMonitorFeatures() const = 0;

    virtual bool startMonitoring(const QGeoAreaMonitorInfo &monitor) = 0;
    virtual bool stopMonitoring(const QGeoAreaMonitorInfo &monitor) = 0;
    virtual bool requestUpdate(const QGeoAreaMonitorInfo &monitor, const char *signal) = 0;

    virtual QList<QGeoAreaMonitorInfo> activeMonitors() const = 0;
    virtual QList<QGeoAreaMonitorInfo> activeMonitors(const QGeoShape &lookupArea) const = 0;

Q_SIGNALS:
    void areaEntered(const QGeoAreaMonitorInfo &monitor, const QGeoPositionInfo &update);
    void areaExited(const QGeoAreaMonitorInfo &monitor, const QGeoPositionInfo &update);
    void monitorExpired(const QGeoAreaMonitorInfo &monitor);
    void error(QGeoAreaMonitorSource::Error error);

private:
    Q_DISABLE_COPY(QGeoAreaMonitorSource)
    QGeoAreaMonitorSourcePrivate *d;
};


QT_END_NAMESPACE

#endif
