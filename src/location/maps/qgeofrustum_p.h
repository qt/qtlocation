/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: http://www.qt-project.org/
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
#ifndef QGEOFRUSTUM_P_H
#define QGEOFRUSTUM_P_H

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

#include <QHash>

#include <Qt3D/qplane3d.h>

#include "qdoublevector3d_p.h"

QT_BEGIN_NAMESPACE

class QGLCamera;

class Q_AUTOTEST_EXPORT QGeoFrustum
{
public:
    enum Plane {
        Near = 0x001,
        Far = 0x002,
        Right = 0x004,
        Left = 0x008,
        Top = 0x010,
        Bottom = 0x020,
        TopLeftNear = Top | Left | Near,
        TopLeftFar = Top | Left | Far,
        TopRightNear = Top | Right | Near,
        TopRightFar = Top | Right | Far,
        BottomLeftNear = Bottom | Left | Near,
        BottomLeftFar = Bottom | Left | Far,
        BottomRightNear = Bottom | Right | Near,
        BottomRightFar = Bottom | Right | Far
    };

    Q_DECLARE_FLAGS(Planes, Plane)

    QGeoFrustum();

    void update(const QGLCamera *camera, double aspectRatio, bool updatePlanes = false);

    bool contains(const QDoubleVector3D &center, double radius) const;

    QDoubleVector3D topLeftNear() const {
        return tln_;
    }
    QDoubleVector3D topLeftFar() const {
        return tlf_;
    }
    QDoubleVector3D bottomLeftNear() const {
        return bln_;
    }
    QDoubleVector3D bottomLeftFar() const {
        return blf_;
    }
    QDoubleVector3D topRightNear() const {
        return trn_;
    }
    QDoubleVector3D topRightFar() const {
        return trf_;
    }
    QDoubleVector3D bottomRightNear() const {
        return brn_;
    }
    QDoubleVector3D bottomRightFar() const {
        return brf_;
    }

    QPlane3D plane(Planes planes) const;

private:
    double hf_;
    double wf_;
    QDoubleVector3D cf_;
    QDoubleVector3D tlf_;
    QDoubleVector3D trf_;
    QDoubleVector3D blf_;
    QDoubleVector3D brf_;

    double hn_;
    double wn_;
    QDoubleVector3D cn_;
    QDoubleVector3D tln_;
    QDoubleVector3D trn_;
    QDoubleVector3D bln_;
    QDoubleVector3D brn_;

    QHash<Planes, QPlane3D> planeHash_;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGeoFrustum::Planes)

QT_END_NAMESPACE

#endif // QGEOFRUSTUM_P_H
