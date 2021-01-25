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

#ifndef QGEOSATELLITEINFO_P_H
#define QGEOSATELLITEINFO_P_H

#include <QtPositioning/private/qpositioningglobal_p.h>
#include <QtPositioning/qgeosatelliteinfo.h>
#include <QHash>

QT_BEGIN_NAMESPACE

class Q_POSITIONING_PRIVATE_EXPORT QGeoSatelliteInfoPrivate
{
public:
    QGeoSatelliteInfoPrivate();
    QGeoSatelliteInfoPrivate(const QGeoSatelliteInfoPrivate &other);
    virtual ~QGeoSatelliteInfoPrivate();
    virtual QGeoSatelliteInfoPrivate *clone() const;
    virtual bool operator==(const QGeoSatelliteInfoPrivate &other) const;
    static QGeoSatelliteInfoPrivate *get(const QGeoSatelliteInfo &info);

    int signal;
    int satId;
    QGeoSatelliteInfo::SatelliteSystem system;
    QHash<int, qreal> doubleAttribs;
};

QT_END_NAMESPACE

#endif // QGEOSATELLITEINFO_P_H
