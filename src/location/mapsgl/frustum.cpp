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
#include "frustum_p.h"

#include "viewportcamera_p.h"

#include <Qt3D/qglcamera.h>

#include <cmath>

Frustum::Frustum() {}

void Frustum::update(const QGLCamera *camera, double aspectRatio, bool updatePlanes)
{
    if (aspectRatio > 1.0) {
        double fov = atan2(camera->viewSize().height() , (2 * camera->nearPlane()));

        hn_ = 2 * tan(fov) * camera->nearPlane();
        wn_ = hn_ * aspectRatio;

        hf_ = 2 * tan(fov) * camera->farPlane();
        wf_ = hf_ * aspectRatio;
    } else {
        double fov = atan2(camera->viewSize().width() , (2 * camera->nearPlane()));

        wn_ = 2 * tan(fov) * camera->nearPlane();
        hn_ = wn_ / aspectRatio;

        wf_ = 2 * tan(fov) * camera->farPlane();
        hf_ = wf_ / aspectRatio;
    }

    QVector3D p = camera->eye();
    QVector3D d = camera->center() - camera->eye();
    d.normalize();

    QVector3D up = camera->upVector();
    up.normalize();

    QVector3D right = QVector3D::normal(d, up);

    cf_ = p + d * camera->farPlane();
    tlf_ = cf_ + (up * hf_ / 2) - (right * wf_ / 2);
    trf_ = cf_ + (up * hf_ / 2) + (right * wf_ / 2);
    blf_ = cf_ - (up * hf_ / 2) - (right * wf_ / 2);
    brf_ = cf_ - (up * hf_ / 2) + (right * wf_ / 2);

    cn_ = p + d * camera->nearPlane();
    tln_ = cn_ + (up * hn_ / 2) - (right * wn_ / 2);
    trn_ = cn_ + (up * hn_ / 2) + (right * wn_ / 2);
    bln_ = cn_ - (up * hn_ / 2) - (right * wn_ / 2);
    brn_ = cn_ - (up * hn_ / 2) + (right * wn_ / 2);

    if (!updatePlanes)
        return;

    QPlane3D pn = QPlane3D(bln_, tln_, brn_);
    pn.setNormal(pn.normal().normalized());
    planeHash_.insert(Frustum::Planes(Frustum::Near), pn);

    QPlane3D pf = QPlane3D(blf_, brf_, tlf_);
    pf.setNormal(pf.normal().normalized());
    planeHash_.insert(Frustum::Planes(Frustum::Far), pf);

    QPlane3D pl = QPlane3D(blf_, tlf_, bln_);
    pl.setNormal(pl.normal().normalized());
    planeHash_.insert(Frustum::Planes(Frustum::Left), pl);

    QPlane3D pr = QPlane3D(brf_, brn_, trf_);
    pr.setNormal(pr.normal().normalized());
    planeHash_.insert(Frustum::Planes(Frustum::Right), pr);

    QPlane3D pt = QPlane3D(tlf_, trf_, tln_);
    pt.setNormal(pt.normal().normalized());
    planeHash_.insert(Frustum::Planes(Frustum::Top), pt);

    QPlane3D pb = QPlane3D(blf_, bln_, brf_);
    pb.setNormal(pb.normal().normalized());
    planeHash_.insert(Frustum::Planes(Frustum::Bottom), pb);
}

void Frustum::update(const ViewportCamera &camera)
{
    if (camera.aspectRatio() > 1.0) {
        double fov = atan2(camera.viewSize().height() , (2 * camera.nearPlane()));

        hn_ = 2 * tan(fov) * camera.nearPlane();
        wn_ = hn_ * camera.aspectRatio();

        hf_ = 2 * tan(fov) * camera.farPlane();
        wf_ = hf_ * camera.aspectRatio();
    } else {
        double fov = atan2(camera.viewSize().width() , (2 * camera.nearPlane()));

        wn_ = 2 * tan(fov) * camera.nearPlane();
        hn_ = wn_ / camera.aspectRatio();

        wf_ = 2 * tan(fov) * camera.farPlane();
        hf_ = wf_ / camera.aspectRatio();
    }

    QVector3D p = camera.eye();
    QVector3D d = camera.center() - camera.eye();
    d.normalize();

    QVector3D up = camera.up();
    up.normalize();

    QVector3D right = QVector3D::normal(d, up);

    cf_ = p + d * camera.farPlane();
    tlf_ = cf_ + (up * hf_ / 2) - (right * wf_ / 2);
    trf_ = cf_ + (up * hf_ / 2) + (right * wf_ / 2);
    blf_ = cf_ - (up * hf_ / 2) - (right * wf_ / 2);
    brf_ = cf_ - (up * hf_ / 2) + (right * wf_ / 2);

    cn_ = p + d * camera.nearPlane();
    tln_ = cn_ + (up * hn_ / 2) - (right * wn_ / 2);
    trn_ = cn_ + (up * hn_ / 2) + (right * wn_ / 2);
    bln_ = cn_ - (up * hn_ / 2) - (right * wn_ / 2);
    brn_ = cn_ - (up * hn_ / 2) + (right * wn_ / 2);

    QPlane3D pn = QPlane3D(bln_, tln_, brn_);
    pn.setNormal(pn.normal().normalized());
    planeHash_.insert(Frustum::Planes(Frustum::Near), pn);

    QPlane3D pf = QPlane3D(blf_, brf_, tlf_);
    pf.setNormal(pf.normal().normalized());
    planeHash_.insert(Frustum::Planes(Frustum::Far), pf);

    QPlane3D pl = QPlane3D(blf_, tlf_, bln_);
    pl.setNormal(pl.normal().normalized());
    planeHash_.insert(Frustum::Planes(Frustum::Left), pl);

    QPlane3D pr = QPlane3D(brf_, brn_, trf_);
    pr.setNormal(pr.normal().normalized());
    planeHash_.insert(Frustum::Planes(Frustum::Right), pr);

    QPlane3D pt = QPlane3D(tlf_, trf_, tln_);
    pt.setNormal(pt.normal().normalized());
    planeHash_.insert(Frustum::Planes(Frustum::Top), pt);

    QPlane3D pb = QPlane3D(blf_, bln_, brf_);
    pb.setNormal(pb.normal().normalized());
    planeHash_.insert(Frustum::Planes(Frustum::Bottom), pb);
}

bool Frustum::contains(const QVector3D &center, double radius) const
{
    if (planeHash_.isEmpty())
        return false;

    QHash<Frustum::Planes, QPlane3D>::const_iterator i = planeHash_.constBegin();
    QHash<Frustum::Planes, QPlane3D>::const_iterator end = planeHash_.constEnd();

    while (i != end) {
        if (i.value().distanceTo(center) < -1.0 * radius)
            return false;
        ++i;
    }

    return true;
}

QPlane3D Frustum::plane(Planes planes) const
{
    return planeHash_.value(planes);
}
