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
#ifndef TILECAMERA_H
#define TILECAMERA_H

#include "qglobal.h"

QT_BEGIN_NAMESPACE

class SphereGeometry;
class QGLCamera;
class QGLSceneNode;
class QVector3D;

class Q_LOCATION_EXPORT TileCamera
{
public:
    TileCamera();
    ~TileCamera();

    enum DetailPreference {
        DetailPreferenceNear,
        DetailPreferenceCenter
    };

    // Reset the camera (to look at lon,lat, with given distance, tilt etc.)
    void setCamera(const SphereGeometry &sphere,
                   double latitude,
                   double longitude,
                   double distance,
                   double bearing,
                   double tilt,
                   double aspectRatio);
    void tiltCamera(const SphereGeometry& sphere, double tilt);
    void rollCamera(const SphereGeometry& sphere, double roll);
    void panCamera(const SphereGeometry& sphere, double pan);
    void rotateCamera(const SphereGeometry& sphere, double up, double right);
    void zoomCamera(SphereGeometry& sphere, double factor, DetailPreference detailPreference = DetailPreferenceNear);

    QGLCamera* camera() const;

    QVector3D eye() const;
    QVector3D view(double x, double y) const;

private:
    double latitude_;
    double longitude_;
    double distance_;
    double bearing_;
    double tilt_;
    double zoom_;
    double aspectRatio_;
    QGLCamera* camera_;
};

QT_END_NAMESPACE

#endif // TILECAMERA_H
