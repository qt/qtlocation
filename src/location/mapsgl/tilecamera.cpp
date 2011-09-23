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
#include "tilecamera.h"

#include "spheregeometry.h"

#include <QVector3D>
#include <qnumeric.h>
#include <qglcamera.h>

#include <cmath>

TileCamera::TileCamera()
        : latitude_(-27.5),
        longitude_(153),
        distance_(0.1),
        bearing_(0.0),
        tilt_(0.0),
        aspectRatio_(1.0),
        camera_(new QGLCamera())
{
}

TileCamera::~TileCamera()
{
    delete camera_;
}

// Up and right units are bit obscure still, fixme
void TileCamera::rotateCamera(const SphereGeometry & sphere, double up, double right)
{
    QVector3D view = camera_->eye() - camera_->center();
    QVector3D side = QVector3D::normal(view, camera_->upVector());
    QMatrix4x4 rotationMatrix;
    // First determine how big angle and about which axis' to rotate
    // and express them with a rotation matrix (spherical rotation around origin)
    rotationMatrix.rotate(right / sphere.zoomLevel(), camera_->upVector());
    rotationMatrix.rotate(up / sphere.zoomLevel(), side);
    // Then rotate the eye and center of the camera
    camera_->setCenter(camera_->center() * rotationMatrix);
    camera_->setEye(camera_->eye() * rotationMatrix);
}

void TileCamera::setCamera(const SphereGeometry &sphere,
                           double latitude,
                           double longitude,
                           double distance,
                           double bearing,
                           double tilt,
                           double aspectRatio)
{
    QVector3D c = sphere.coordToPoint(latitude, longitude);
    QVector3D e = sphere.coordToPoint(latitude, longitude, sphere.radius() * distance);

    camera_->setCenter(c);
    camera_->setEye(e);

    QVector3D view = camera_->eye() - camera_->center();
    QVector3D side = QVector3D::normal(view, QVector3D(0.0, 1.0, 0.0));
    QVector3D up = QVector3D::normal(side, view);

    // define rotation around view vector and apply it to up vector
    QMatrix4x4 mBearing;
    mBearing.rotate(-1.0 * bearing, view);
    camera_->setUpVector(mBearing * up);

    // define rotation around new side vector and  apply it to eye
    QVector3D side2 = QVector3D::normal(mBearing * up, view);
    QMatrix4x4 mTilt;
    mTilt.rotate(tilt, side2);
    camera_->setEye((mTilt * view) + c);

    // reset the up vector after tilting
    view = camera_->eye() - camera_->center();
    up = QVector3D::normal(view, side2);
    camera_->setUpVector(up);

    // set near and far planes
    camera_->setNearPlane(1);
    camera_->setFarPlane(2.0 * sphere.radius() * distance);
    zoom_ = sphere.zoomLevel();
}

/* TODO improve zoomCamera a lot. Current implementation is very hacky.

   @factor: fractions of a zoom level to zoom in. Doubling a zoom level
   roughly doubles the distance from the earth.

   @detailPreference: whether the distance from the earth should be that of the eye from
   closest point on earth, or from eye to center (center is where we are looking at).

  */
void TileCamera::zoomCamera(SphereGeometry& sphere, double factor, DetailPreference detailPreference)
{
    if (!(-1.0 < factor && factor < 1.0)) {
        qWarning() << "Zoom factor must be between -1.0 and 1.0, you gave: " << factor;
        return;
    }

    QVector3D view = camera_->eye() - camera_->center();
    Q_ASSERT(view.length() > 0.0);
    double viewVectorLength = view.length();
    double eyeVectorLength = camera_->eye().length();
    double sphereRadius = sphere.radius();
    double zoomDistance;
    if (detailPreference == DetailPreferenceNear) {
        qDebug() << "Zooming, preferring details/tiles near, factor: " << factor;
        double viewEyeDotProduct = QVector3D::dotProduct(view, camera_->eye());
        // Dodgy, fixme
        bool zoomOut = false;
        if (factor < 0.0) {
            factor = -factor;
            zoomOut = true;
        }
        factor += 1.0;
        // Look for the multiplication factor so that multiplying the view with the factor
        // will result in eye height changing according to given factor.
        double sqrtResult = sqrt(pow(viewEyeDotProduct, 2) - 4 * pow(viewVectorLength, 2) * (pow(eyeVectorLength, 2) - pow(sphereRadius + factor * eyeVectorLength - factor * sphereRadius , 2)));
        double viewVectorFactor = (-viewEyeDotProduct + sqrtResult) / (2 * pow(viewVectorLength, 2));
        // Dodgy, fixme
        if (!zoomOut)
            camera_->setEye(camera_->eye() - view * viewVectorFactor);
        else
            camera_->setEye(view * viewVectorFactor + camera_->eye());
        // Fixme, far too big farplane (no pun intended)
        view = camera_->eye() - camera_->center();
        camera_->setFarPlane(sphere.radius() + view.length());
        zoomDistance = camera_->eye().length() - sphere.radius();
    } else if (detailPreference == DetailPreferenceCenter) {
        qDebug() << "Zooming, preferring details/tiles at center, factor: " << factor;
        factor = -factor;
        factor += 1;
        camera_->setEye(camera_->center() + view * factor);
        view = camera_->eye() - camera_->center();
        camera_->setFarPlane(sphere.radius() + view.length());
        zoomDistance = view.length();
    } else {
        qWarning("Unsupported detail preference for zoom.");
        return;
    }
    // Adjust sphere zoom level if necessary
    double maxZoomLevel = 18; // TODO this needs to come from the current plugin
    double zoomCoefficient = 0.5; // TODO just initial guess
    int zoomLevel = int(maxZoomLevel - log(zoomDistance / zoomCoefficient) / log(2.0));
    qDebug() << "Zoom, tilesphere zoom was : " << sphere.zoomLevel();
    qDebug() << "Zoom level (fractional) would be : " << maxZoomLevel - log2(zoomDistance / zoomCoefficient);
    if ((int)zoomLevel != sphere.zoomLevel() && zoomLevel < maxZoomLevel && zoomLevel >= 0) {
        sphere.setZoomLevel(int(zoomLevel));
//        sphere.update(*this);
    }
}

// Can be done like this if quaternions ease the animation
//void TileCamera::tiltCamera(double tilt, QQuaternion = QQuaternion()) {
// {
// if (!quaternion.isIdentity()) // use quaternion. However, keeping track of 'tilt_' needs to be done
// ...
// else // use tilt angle

void TileCamera::tiltCamera(const SphereGeometry& sphere, double tilt)
{
    Q_ASSERT(!qIsNaN(tilt));
    camera_->rotateCenter(camera_->tilt(tilt));
    // Don't fall through the sphere surface
    if (camera_->eye().length() - sphere.radius() < 0.0) {
        qDebug() << "Camera tilt safestop, would go through earth.";
        camera_->rotateCenter(camera_->tilt(-tilt));
    }
}

void TileCamera::rollCamera(const SphereGeometry& sphere, double roll)
{
    Q_UNUSED(sphere);
    Q_ASSERT(!qIsNaN(roll));
    camera_->rotateCenter(camera_->roll(roll));
}

void TileCamera::panCamera(const SphereGeometry& sphere, double pan)
{
    Q_ASSERT(!qIsNaN(pan));
    camera_->rotateCenter(camera_->pan(pan));
    if (camera_->eye().length() - sphere.radius() < 0.0) {
        qDebug() << "Camera pan safety stop, would go through earth.";
        camera_->rotateCenter(camera_->pan(-pan));
    }
}

QGLCamera* TileCamera::camera() const
{
    return camera_;
}

QVector3D TileCamera::eye() const
{
    return camera_->eye();
}

QVector3D TileCamera::view(double x, double y) const
{
    double fov = atan(camera_->viewSize().height() / (2 * camera_->nearPlane()));

    double hf = 2 * tan(fov) * camera_->farPlane();
    double wf = hf * aspectRatio_;

    QVector3D p = camera_->eye();
    QVector3D d = camera_->center() - camera_->eye();
    d.normalize();

    QVector3D up = camera_->upVector();
    up.normalize();

    QVector3D right = QVector3D::normal(d, up);

    double x1 = qMin(qMax(x, 0.0), 1.0) - 0.5;
    double y1 = qMin(qMax(y, 0.0), 1.0) - 0.5;

    QVector3D c = d * camera_->farPlane();
    c += up * hf * y1;
    c += right * wf * x1;

    return c.normalized();
}

