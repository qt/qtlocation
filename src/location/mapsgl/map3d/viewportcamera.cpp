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
#include "viewportcamera_p.h"

#include "cameradata.h"

#include "projection3d_p.h"

#include <qglcamera.h>

ViewportCamera::ViewportCamera()
    : camera_(new QGLCamera()){}

ViewportCamera::~ViewportCamera()
{
    delete camera_;
}

void ViewportCamera::setProjection(Projection3D *projection)
{
    projection_ = projection;
}

//void ViewportCamera::setSphere(const SphereGeometry &sphere)
//{
//    sphere_ = sphere;
//}

void ViewportCamera::setCameraData(const CameraData &cameraData)
{
//    sphere_.setZoomLevel(cameraData.zoomLevel());
    double altitude = projection_->radius() * cameraData.distance();

    QGeoCoordinate coord = cameraData.center();
    coord.setAltitude(0.0);
    center_ = projection_->coordToPoint(coord);
    coord.setAltitude(altitude);
    eye_ = projection_->coordToPoint(coord);

    QVector3D view = eye_ - center_;
    QVector3D side = QVector3D::normal(view, QVector3D(0.0, 1.0, 0.0));
    up_ = QVector3D::normal(side, view);

    QMatrix4x4 mBearing;
    mBearing.rotate(-1.0 * cameraData.bearing(), view);
    up_ = mBearing * up_;

    QVector3D side2 = QVector3D::normal(up_, view);
    QMatrix4x4 mTilt;
    mTilt.rotate(cameraData.tilt(), side2);
    eye_ = (mTilt * view) + center_;

    view = eye_ - center_;
    side = QVector3D::normal(view, QVector3D(0.0, 1.0, 0.0));
    up_ = QVector3D::normal(view, side2);

    QMatrix4x4 mRoll;
    mRoll.rotate(cameraData.roll(), view);
    up_ = mRoll * up_;

    nearPlane_ = 1.0;
    farPlane_ = 2.0 * altitude;

    viewSize_ = camera_->viewSize();
    aspectRatio_ = cameraData.aspectRatio();

    updateGLCamera();
    updateFrustum();
}

void ViewportCamera::updateGLCamera()
{
    camera_->setCenter(center_);
    camera_->setEye(eye_);
    camera_->setUpVector(up_);
    camera_->setNearPlane(nearPlane_);
    camera_->setFarPlane(farPlane_);
}

void ViewportCamera::updateFrustum()
{
    frustum_.update(*this);
}

QVector3D ViewportCamera::center() const
{
    return center_;
}

QVector3D ViewportCamera::eye() const
{
    return eye_;
}

QVector3D ViewportCamera::up() const
{
    return up_;
}

QSizeF ViewportCamera::viewSize() const
{
    return viewSize_;
}

double ViewportCamera::aspectRatio() const
{
    return aspectRatio_;
}

double ViewportCamera::nearPlane() const
{
    return nearPlane_;
}

double ViewportCamera::farPlane() const
{
    return farPlane_;
}

QGLCamera* ViewportCamera::toGLCamera() const
{
    return camera_;
}

Frustum ViewportCamera::toFrustum() const
{
    return frustum_;
}
