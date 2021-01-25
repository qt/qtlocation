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

#ifndef QGEOCAMERACAPABILITIES_P_H
#define QGEOCAMERACAPABILITIES_P_H

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

#include <QSharedDataPointer>

#include <QtLocation/private/qlocationglobal_p.h>

QT_BEGIN_NAMESPACE

class QGeoCameraCapabilitiesPrivate;

class Q_LOCATION_PRIVATE_EXPORT QGeoCameraCapabilities
{
public:
    QGeoCameraCapabilities();
    QGeoCameraCapabilities(const QGeoCameraCapabilities &other);
    ~QGeoCameraCapabilities();

    QGeoCameraCapabilities &operator = (const QGeoCameraCapabilities &other);

    bool operator == (const QGeoCameraCapabilities &other) const;
    bool operator != (const QGeoCameraCapabilities &other) const;

    void setTileSize(int tileSize);
    int tileSize() const;

    void setMinimumZoomLevel(double minimumZoomLevel);
    double minimumZoomLevel() const;
    double minimumZoomLevelAt256() const;

    void setMaximumZoomLevel(double maximumZoomLevel);
    double maximumZoomLevel() const;
    double maximumZoomLevelAt256() const;

    void setSupportsBearing(bool supportsBearing);
    bool supportsBearing() const;

    void setSupportsRolling(bool supportsRolling);
    bool supportsRolling() const;

    void setSupportsTilting(bool supportsTilting);
    bool supportsTilting() const;

    void setMinimumTilt(double minimumTilt);
    double minimumTilt() const;

    void setMaximumTilt(double maximumTilt);
    double maximumTilt() const;

    void setMinimumFieldOfView(double minimumFieldOfView);
    double minimumFieldOfView() const;

    void setMaximumFieldOfView(double maximumFieldOfView);
    double maximumFieldOfView() const;

    void setOverzoomEnabled(bool overzoomEnabled);
    bool overzoomEnabled() const;

    bool isValid() const;

private:
    QSharedDataPointer<QGeoCameraCapabilitiesPrivate> d;
};

QT_END_NAMESPACE

#endif // QGEOCAMERACAPABILITIES_P_H
