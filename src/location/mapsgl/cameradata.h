/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtLocation module of the Qt Toolkit.
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
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef CAMERADATA_H
#define CAMERADATA_H

#include "qgeocoordinate.h"

#include <QMetaType>

#include <QSharedPointer>
#include <QSharedDataPointer>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE



class Projection;
class CameraDataPrivate;

class Q_LOCATION_EXPORT CameraData
{
public:
    CameraData();
    CameraData(const CameraData &other);
    ~CameraData();

    CameraData& operator = (const CameraData &other);

    bool operator == (const CameraData &other) const;
    bool operator != (const CameraData &other) const;

    void setCenter(const QGeoCoordinate &coordinate);
    QGeoCoordinate center() const;

    void setBearing(double bearing);
    double bearing() const;

    void setTilt(double tilt);
    double tilt() const;

    void setRoll(double roll);
    double roll() const;

    void setAspectRatio(double aspectRatio);
    double aspectRatio() const;

    void setDistance(double distance);
    double distance() const;

    void setZoomLevel(int zoomLevel);
    int zoomLevel() const;

    void setZoomFactor(double zoomFactor);
    double zoomFactor() const;

    void setProjection(QSharedPointer<Projection> projection);
    QSharedPointer<Projection> projection() const;

private:
    QSharedDataPointer<CameraDataPrivate> d;
};

Q_DECLARE_METATYPE(CameraData)

QT_END_NAMESPACE

QT_END_HEADER

#endif // CAMERADATA_H
