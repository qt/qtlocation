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
#ifndef QGEOCAMERADATA_P_H
#define QGEOCAMERADATA_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtLocation/private/qlocationglobal_p.h>
#include <QtPositioning/qgeocoordinate.h>

#include <QMetaType>

#include <QSharedDataPointer>

QT_BEGIN_NAMESPACE

class QGeoCameraDataPrivate;

class Q_LOCATION_PRIVATE_EXPORT QGeoCameraData
{
public:
    QGeoCameraData();
    QGeoCameraData(const QGeoCameraData &other);
    ~QGeoCameraData();

    QGeoCameraData &operator = (const QGeoCameraData &other);

    bool operator == (const QGeoCameraData &other) const;
    bool operator != (const QGeoCameraData &other) const;

    void setCenter(const QGeoCoordinate &coordinate);
    QGeoCoordinate center() const;

    void setBearing(double bearing);
    double bearing() const;

    void setTilt(double tilt);
    double tilt() const;

    void setRoll(double roll);
    double roll() const;

    void setFieldOfView(double fieldOfView);
    double fieldOfView() const;

    // Zoom level is intended to be relative to a tileSize of 256^2 pixels.
    // E.g., a zoom level of 0 must result in a mapWidth of 256, and so on.
    void setZoomLevel(double zoomLevel);
    double zoomLevel() const;

private:
    QSharedDataPointer<QGeoCameraDataPrivate> d;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QGeoCameraData)

#endif // QGEOCAMERADATA_P_H
